/*
 * cache.cc
 *
 *  Created on: Mar 18, 2014
 *      Author: Joonmoo Huh
 */

#include "cache.h"

unsigned long LRU::getLRU(unsigned long argIdx) {
	unsigned long i;

	for (i = 0; i < assoc; i++)
		if (!seq[argIdx][i])
			break;
	assert(i != assoc);

	return i;
}

void LRU::updateMRU(unsigned long argIdx, unsigned long argPos) {
	unsigned long i;

	if (seq[argIdx][argPos] != assoc - 1) {
		for (i = 0; i < assoc; i++) {
			if (seq[argIdx][argPos] < seq[argIdx][i])
				seq[argIdx][i]--;
		}
		seq[argIdx][argPos] = assoc - 1;
	}
}

void LRU::updateLRU(unsigned long argIdx, unsigned long argPos) {
	unsigned long i;

	if (seq[argIdx][argPos] != 0) {
		for (i = 0; i < assoc; i++) {
			if (seq[argIdx][argPos] > seq[argIdx][i])
				seq[argIdx][i]++;
		}
		seq[argIdx][argPos] = 0;
	}
}

unsigned long Cache::findBlock(unsigned long argTag, unsigned long argIndex) {
	unsigned long i, pos;

	pos = assoc;
	for (i = 0; i < assoc; i++)
		if (cache[argIndex][i].isValid())
			if (cache[argIndex][i].getTag() == argTag) {
				pos = i;
				break;
			}

	return pos;
}

void Cache_WT::do_access(unsigned long addr, unsigned char op,
		ReqNextCacheAccess **req) {
	unsigned long tag, idx, pos;

	tag = calcTag(addr);
	idx = calcIndex(addr);

	assert(!getP1BusyCycle());

	if (op == 'r')
		reads++;
	else
		writes++;

	accessesSets[idx]++;
	pos = findBlock(tag, idx);
	if (pos == assoc) { // Miss case
		if (op == 'r')
			readMisses++;
		else
			writeMisses++;

		*req = new ReqNextCacheAccess(addr, op);

		pos = lru->getLRU(idx);
		cache[idx][pos].setTag(tag);
		cache[idx][pos].setCoherence(VALID);

		lru->updateMRU(idx, pos);
	} else { // Hit case
		if (op == 'w') {
			*req = new ReqNextCacheAccess(addr, op);
		}
		lru->updateMRU(idx, pos);
	}
}

void Cache_WT::do_invalidate(unsigned long addr, unsigned long tagMask_L2) {
	unsigned long tag, idx, pos;

	tag = calcTag(addr);
	idx = calcIndex(addr);

	assert(!getP2BusyCycle());
	assert(tagMask < tagMask_L2);

	pos = findBlock(tag, idx);
	//assert(pos!=assoc);

	if (pos != assoc) {
		assert(cache[idx][pos].getCoherence() != INVALID);
		cache[idx][pos].setCoherence(INVALID);
		invalidated++;
	}
}

void Cache_MESI::do_access(unsigned long addr, unsigned char op,
		Directory_Manager_Interleave *dir, ReqDirectoryAccess **req,
		ReqDirectoryAccess **reqWB, ReqPrevCacheInvalidate **inv) {
	unsigned long tag, idx, pos;

	tag = calcTag(addr);
	idx = calcIndex(addr);

#ifdef DEBUG_DETAIL
	/*if (tag==calcTag(6298176)) {
		cout << "REQUEST: " << getID() << " " << op << " 0x" << hex << addr << dec << "\n";
	}*/
#endif
	assert(!getP1BusyCycle());

	if (op == 'r')
		reads++;
	else
		writes++;

	accessesSets[idx]++;
	pos = findBlock(tag, idx);
	if (pos == assoc) { // Miss case
		if (op == 'r')
			readMisses++;
		else
			writeMisses++;

		pos = lru->getLRU(idx);
		switch (cache[idx][pos].getCoherence()) {
		case INVALID:
			break;
		case SHARED:
		case EXCLUSIVE:
			*reqWB = new ReqDirectoryAccess(id,
					calcAddr(cache[idx][pos].getTag(), idx), 'r');
			*inv = new ReqPrevCacheInvalidate(id,
					calcAddr(cache[idx][pos].getTag(), idx), tagMask);
			invalidates++;
			break;
		case MODIFIED:
			*reqWB = new ReqDirectoryAccess(id,
					calcAddr(cache[idx][pos].getTag(), idx), 'w');
			*inv = new ReqPrevCacheInvalidate(id,
					calcAddr(cache[idx][pos].getTag(), idx), tagMask);
			writeBacks++;
			break;
		default:
			assert(0);
		}

		cache[idx][pos].setTag(tag);
		if (op == 'r') {
			switch (dir->refer_state(addr)) {
			case INVALID:
				cache[idx][pos].setCoherence(EXCLUSIVE);
				break;
			case EXCLUSIVE:
			case SHARED:
				cache[idx][pos].setCoherence(SHARED);
				break;
			default:
				assert(0);
			}
		} else
			cache[idx][pos].setCoherence(MODIFIED);
		*req = new ReqDirectoryAccess(id, addr, op);

		lru->updateMRU(idx, pos);
	} else { // Hit case
		if (op == 'w') {
			switch (cache[idx][pos].getCoherence()) {
			case SHARED:
				*req = new ReqDirectoryAccess(id, addr, op);
				break;
			case EXCLUSIVE:
			case MODIFIED:
				break;
			case INVALID:
			default:
				assert(0);
			}
			cache[idx][pos].setCoherence(MODIFIED);
		}
		lru->updateMRU(idx, pos);
	}

#ifdef DEBUG_DETAIL
	/*if (idx==550) {
	 for (unsigned long i=0; i<assoc; i++) {
	 cout << hex << setw(5) << cache[idx][i].getTag() << dec << " ";
	 switch (cache[idx][i].getCoherence()) {
	 case INVALID:   cout << "I "; break;
	 case SHARED:    cout << "S "; break;
	 case MODIFIED:  cout << "M "; break;
	 case EXCLUSIVE: cout << "E "; break;
	 default:
	 assert(0);
	 }
	 }
	 cout << "\n";
	 for (unsigned long i=0; i<assoc; i++) {
	 cout << hex << setw(5) << cache[idx][i].getTag() << dec << " ";
	 switch (cache[idx][i].getCoherence()) {
	 case INVALID:   cout << "I "; break;
	 case SHARED:    cout << "S "; break;
	 case MODIFIED:  cout << "M "; break;
	 case EXCLUSIVE: cout << "E "; break;
	 default:
	 assert(0);
	 }
	 }
	 cout << "\n";
	 }*/
#endif
}

unsigned long Cache_MESI::check_WB(unsigned long addr, unsigned char op) {
	unsigned long tag, idx, pos;

	tag = calcTag(addr);
	idx = calcIndex(addr);
	pos = findBlock(tag, idx);

	if (pos == assoc) { // Miss case
		pos = lru->getLRU(idx);
		switch (cache[idx][pos].getCoherence()) {
		case INVALID:
			break;
		case SHARED:
		case EXCLUSIVE:
		case MODIFIED:
			return calcAddr(cache[idx][pos].getTag(), idx);
			break;
		default:
			assert(0);
		}
	}

	return 0;
}

void Cache_MESI::do_read(unsigned long addr) {
	unsigned long tag, idx, pos;

	tag = calcTag(addr);
	idx = calcIndex(addr);

	assert(!getP2BusyCycle());

	pos = findBlock(tag, idx);
	assert(pos != assoc);

	assert(cache[idx][pos].getCoherence() != INVALID);
	cache[idx][pos].setCoherence(SHARED);
	read++;
}

void Cache_MESI::do_invalidate(unsigned long addr, unsigned long tagMask0, ReqPrevCacheInvalidate **req) {
	unsigned long tag, idx, pos;

	tag = calcTag(addr);
	idx = calcIndex(addr);

	assert(!getP2BusyCycle());
	assert(!tagMask0);

	pos = findBlock(tag, idx);
	assert(pos != assoc);

	assert(cache[idx][pos].getCoherence() != INVALID);
	cache[idx][pos].setCoherence(INVALID);
	*req = new ReqPrevCacheInvalidate(id, addr, tagMask);
	invalidated++;
}

void Cache_MESI::do_exclusive(unsigned long addr) {
	unsigned long tag, idx, pos;

	tag = calcTag(addr);
	idx = calcIndex(addr);

	assert(!getP2BusyCycle());

	pos = findBlock(tag, idx);
	assert(pos != assoc);

	if (cache[idx][pos].getCoherence() != SHARED) {
		cerr << "DEBUG: " << addr << " " << hex << addr << dec << "\n";
	}
	assert(cache[idx][pos].getCoherence() == SHARED);
	cache[idx][pos].setCoherence(EXCLUSIVE);
	exclusived++;
}

enum coherence Cache_MESI::refer_state(unsigned long addr) {
	unsigned long tag, idx, pos;

	tag = calcTag(addr);
	idx = calcIndex(addr);
	pos = findBlock(tag, idx);
	assert(pos < assoc);

	return cache[idx][pos].getCoherence();
}

void Cache_WB_SCL::do_access(unsigned long addr, unsigned char op, ReqNextCacheAccess **reqWB, ReqNextCacheAccess **req) {
	unsigned long tag, idx, pos;

	tag = calcTag(addr);
	idx = calcIndex(addr);

	assert(!getP1BusyCycle());

	if (op == 'r')
		reads++;
	else if (op == 'w')
		writes++;
	else
		assert(0);

	if (op == 'r' || op == 'w') {
		accessesSets[idx]++;
		pos = findBlock(tag, idx);
		if (pos == assoc) { // Miss case
			if (op == 'r')
				readMisses++;
			else
				writeMisses++;

			pos = lru->getLRU(idx);
			switch (cache[idx][pos].getCoherence()) {
			case MODIFIED:
				*reqWB = new ReqNextCacheAccess(
						calcAddr(cache[idx][pos].getTag(), idx), 'w');
				writeBacks++;
				break;
			case INVALID:
			case VALID:
				break;
			default:
				assert(0);
			}

			cache[idx][pos].setTag(tag);
			if (op == 'r') {
				cache[idx][pos].setCoherence(VALID);
				*req = new ReqNextCacheAccess(addr, op);
			} else
				cache[idx][pos].setCoherence(MODIFIED);
			lru->updateMRU(idx, pos);
		} else { // Hit case
			if (op == 'w') {
				cache[idx][pos].setCoherence(MODIFIED);
			}
			lru->updateMRU(idx, pos);
		}
	} else {
		assert(0);
	}
}

unsigned long Cache_WB_SCL::do_flush(unsigned long addr, unsigned long size, bool isSend, unsigned long *wb) {
	unsigned long i, k, k_max, tag, idx;

	if (!size)
		return 0;

	k_max = size/blockSize;
	if (addr%blockSize)
		k_max += 1;
	for (k = 0; k < k_max; k++) {
		tag = calcTag(addr);
		idx = calcIndex(addr);

		for (i = 0; i < assoc; i++)
			if (cache[idx][i].isValid())
				if (cache[idx][i].getTag() == tag) {
					if (isSend) {
						if (cache[idx][i].getCoherence()==MODIFIED) {
							cache[idx][i].setCoherence(VALID);
							mp_writebacks++;
							(*wb)++;
						}
					}
					else {
						cache[idx][i].setCoherence(INVALID);
						lru->updateLRU(idx, i);
						mp_flushed++;
					}
					break;
				}
		addr += blockSize;
	}

	return p1_latency*k_max;
}

unsigned long Cache_WB_SCL::do_send(unsigned long addr, unsigned long size, unsigned long *fromMem) {
	unsigned long i, k, k_max, tag, idx;

	if (!size)
		return 0;

	k_max = size/blockSize;
	if (addr%blockSize)
		k_max += 1;
	for (k = 0; k < k_max; k++) {
		tag = calcTag(addr);
		idx = calcIndex(addr);

		for (i = 0; i < assoc; i++) {
			if (cache[idx][i].isValid()) {
				if (cache[idx][i].getTag() == tag) {
					mp_fromCache++;
					break;
				}
			}
		}
		if (i == assoc) {
			mp_fromMem++;
			(*fromMem)++;
		}
		addr += blockSize;
	}

	return p1_latency*k_max;
}

unsigned long Cache_WB_SCL::do_recv(unsigned long addr, unsigned long size, unsigned long *toMem) {
	unsigned long i, k, k_max, tag, idx;

	if (!size)
		return 0;

	k_max = size/blockSize;
	if (addr%blockSize)
		k_max += 1;
	for (k = 0; k < k_max; k++) {
		tag = calcTag(addr);
		idx = calcIndex(addr);

		for (i = 0; i < assoc; i++) {
			if (cache[idx][i].isValid()) {
				if (cache[idx][i].getTag() == tag) {
					cout << "THIS SHOULD NOT BE SHOWN\n";
					break;
				}
			}
		}
		if (i == assoc) {
			unsigned long pos = lru->getLRU(idx);
			mp_toCache++;
			if (cache[idx][pos].getCoherence()==MODIFIED) {
				mp_toMem++;
				(*toMem)++;
			}
			else if (cache[idx][pos].getCoherence()==EXCLUSIVE) {
				(*toMem)++;
				mp_toMem++;
			}
			cache[idx][pos].setCoherence(EXCLUSIVE);
			cache[idx][pos].setTag(tag);
			lru->updateMRU(idx, pos);
		}
		addr += blockSize;
	}
	for (unsigned long i = 0; i < sets; i++) {
		for (unsigned long j = 0; j < assoc; j++) {
			if (cache[i][j].getCoherence()==EXCLUSIVE)
				cache[i][j].setCoherence(VALID);
		}
	}

	return p1_latency*k_max;
}

bool Cache_WT::printStatus_nextLine() {
	bool emptyLine = false;

	while (!emptyLine && printStatusLine < sets) {
		for (unsigned long i = 0; i < assoc; i++) {
			if (cache[printStatusLine][i].isValid()) {
				emptyLine = true;
			}
		}
		printStatusLine++;
	}
	if (emptyLine) {
		cout << hex << setw(3) << printStatusLine - 1 << ": ";
		for (unsigned long i = 0; i < assoc; i++) {
			if (cache[printStatusLine - 1][i].isValid())
				cout << hex << setw(5) << cache[printStatusLine - 1][i].getTag()
						<< dec << " ";
			else
				cout << "      ";
			switch (cache[printStatusLine - 1][i].getCoherence()) {
			case INVALID:
				cout << "I ";
				break;
			case VALID:
				cout << "V ";
				break;
			default:
				assert(0);
			}
		}
		cout << "                  ";
	} else
		cout << "                                       ";
	if (printStatusLine < sets)
		return true;
	else
		return false;
}

bool Cache_MESI::printStatus_nextLine() {
	bool emptyLine = false;

	while (!emptyLine && printStatusLine < sets) {
		for (unsigned long i = 0; i < assoc; i++) {
			if (cache[printStatusLine][i].isValid()) {
				emptyLine = true;
			}
		}
		printStatusLine++;
	}
	if (emptyLine) {
		cout << hex << setw(3) << printStatusLine - 1 << ": ";
		for (unsigned long i = 0; i < assoc; i++) {
			if (cache[printStatusLine - 1][i].isValid())
				cout << hex << setw(5) << cache[printStatusLine - 1][i].getTag()
						<< dec << " ";
			else
				cout << "      ";
			switch (cache[printStatusLine - 1][i].getCoherence()) {
			case INVALID:
				cout << "I ";
				break;
			case SHARED:
				cout << "S ";
				break;
			case MODIFIED:
				cout << "M ";
				break;
			case EXCLUSIVE:
				cout << "E ";
				break;
			default:
				assert(0);
			}
		}
		cout << "  ";
	} else
		cout << "                                       ";
	if (printStatusLine < sets)
		return true;
	else
		return false;
}

string Cache_MESI::printLine(unsigned long addr) {
	unsigned long j, tag, idx;

	tag = calcTag(addr);
	idx = calcIndex(addr);

	for (j = 0; j < assoc; j++)
		if (cache[idx][j].getTag() == tag) {
			switch (cache[idx][j].getCoherence()) {
			case INVALID:
				return "I";
				break;
			case SHARED:
				return "S";
				break;
			case MODIFIED:
				return "M";
				break;
			case EXCLUSIVE:
				return "E";
				break;
			default:
				assert(0);
			}
		}
	return " ";
}

bool Cache_WB_SCL::printStatus_nextLine() {
	bool emptyLine = false;

	while (!emptyLine && printStatusLine < sets) {
		for (unsigned long i = 0; i < assoc; i++) {
			if (cache[printStatusLine][i].isValid()) {
				emptyLine = true;
			}
		}
		printStatusLine++;
	}
	if (emptyLine) {
		cout << hex << setw(3) << printStatusLine - 1 << ": ";
		for (unsigned long i = 0; i < assoc; i++) {
			if (cache[printStatusLine - 1][i].isValid())
				cout << hex << setw(5) << cache[printStatusLine - 1][i].getTag()
						<< dec << " ";
			else
				cout << "      ";
			switch (cache[printStatusLine - 1][i].getCoherence()) {
			case INVALID:
				cout << "I ";
				break;
			case VALID:
				cout << "V ";
				break;
			case MODIFIED:
				cout << "I ";
				break;
			default:
				assert(0);
			}
		}
		cout << "                  ";
	} else
		cout << "                                       ";
	if (printStatusLine < sets)
		return true;
	else
		return false;
}

void Cache_WT::printStat() {
	unsigned long uses = 0;

	for (unsigned long i = 0; i < sets; i++)
		for (unsigned long j = 0; j < assoc; j++)
			if (!cache[i][j].isValid() && cache[i][j].getTag() == 0)
				uses++;
	cout << setw(20) << reads << setw(20) << writes << setw(20) << readMisses
			<< setw(20) << writeMisses << setw(20) << writeBacks << setw(20)
			<< invalidates << setw(20) << invalidated << setw(20) << read
			<< "\n";
}

void Cache_MESI::printStat() {
	unsigned long uses = 0;

	for (unsigned long i = 0; i < sets; i++)
		for (unsigned long j = 0; j < assoc; j++)
			if (!cache[i][j].isValid() && cache[i][j].getTag() == 0)
				uses++;
	cout << setw(20) << reads << setw(20) << writes << setw(20) << readMisses
			<< setw(20) << writeMisses << setw(20) << writeBacks << setw(20)
			<< invalidates << setw(20) << invalidated << setw(20) << read
			<< "\n";
}
