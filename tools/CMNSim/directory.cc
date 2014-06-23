/*
 * directory.cc
 *
 *  Created on: Mar 20, 2014
 *      Author: Joonmoo Huh
 */

#include "directory.h"

void Directory_MESI::do_request(unsigned long cache_id, unsigned long addr, unsigned char op, Cache_MESI **L2s, bool *memRead, ReqPrevCacheReadManager **reqRead, ReqPrevCacheInvalidateManager **reqInv) {
	unsigned long i;

	assert(cache_id < numCore);
	requests[cache_id]++;

	if (dir.find(addr) == dir.end()) {
		pair<unsigned char *, enum coherence> *dir_element = new pair<unsigned char *, enum coherence>;
		dir_element->first = new unsigned char[numCore];
		dir_element->second = INVALID;
		for (i = 0; i < numCore; i++)
			dir_element->first[i] = 'I';
		dir[addr] = dir_element;
	}

	switch (dir[addr]->second) {
	case INVALID:
		if (op == 'r') {
			*memRead = true;
			memReads++;
		}
		dir[addr]->first[cache_id] = 'V';
		dir[addr]->second = EXCLUSIVE;
		break;
	case SHARED:
		if (op == 'r') {
			*reqRead = new ReqPrevCacheReadManager(id, numCore, addr, dir[addr]->first);
			dir[addr]->first[cache_id] = 'V';
		} else {
			dir[addr]->first[cache_id] = 'I';
			*reqInv = new ReqPrevCacheInvalidateManager(id, numCore, addr, 0, dir[addr]->first);
			for (i = 0; i < numCore; i++)
				if (dir[addr]->first[i] == 'V')
					dir[addr]->first[i] = 'I';
			dir[addr]->first[cache_id] = 'V';
			dir[addr]->second = EXCLUSIVE;
		}
		break;
	case EXCLUSIVE:
		for (i = 0; i < numCore; i++)
			if (dir[addr]->first[i] == 'V')
				break;
		assert(i < numCore);
		assert(i != cache_id);
		switch (L2s[i]->refer_state(addr)) {
		case EXCLUSIVE:
			if (op == 'r') {
				*reqRead = new ReqPrevCacheReadManager(id, numCore, addr, dir[addr]->first);
				dir[addr]->second = SHARED;
			} else {
				*reqInv = new ReqPrevCacheInvalidateManager(id, numCore, addr, 0, dir[addr]->first);
				dir[addr]->first[i] = 'I';
			}
			break;
		case MODIFIED:
			if (op == 'r') {
				*reqRead = new ReqPrevCacheReadManager(id, numCore, addr, dir[addr]->first);
				dir[addr]->second = SHARED;
			} else {
				*reqInv = new ReqPrevCacheInvalidateManager(id, numCore, addr, 0, dir[addr]->first);
				dir[addr]->first[i] = 'I';
			}
			memWrites++;
			break;
		default:
			assert(0);
		}
		dir[addr]->first[cache_id] = 'V';
		break;
	default:
		assert(0);
	}
}

void Directory_MESI::do_request_WB(unsigned long cache_id, unsigned long addr, unsigned char op, ReqPrevCacheInvalidateManager **reqInv) {
	unsigned long i, valid;

	assert(dir.find(addr) != dir.end());
	if (op == 'r')
		invalidates[cache_id]++;
	else
		writebacks[cache_id]++;

	switch (dir[addr]->second) {
	case SHARED:
		assert(op == 'r'); // cannot be writeback operation
		assert(dir[addr]->first[cache_id] == 'V');
		dir[addr]->first[cache_id] = 'I';
		for (i = 0, valid = 0; i < numCore; i++)
			if (dir[addr]->first[i] == 'V')
				valid++;
		assert(0 < valid && valid < numCore);
		if (valid == 1) {
			*reqInv = new ReqPrevCacheInvalidateManager(id, numCore, addr, 0, dir[addr]->first);
			dir[addr]->second = EXCLUSIVE;
		}
		break;
	case EXCLUSIVE:
		for (i = 0; i < numCore; i++)
			if (dir[addr]->first[i] == 'V')
				break;
		assert(i < numCore);
		if (op == 'w')
			memWrites++;
		for (i = 0; i < numCore; i++)
			dir[addr]->first[i] = 'I';
		dir[addr]->second = INVALID;
		break;
	case INVALID:
	default:
		assert(0);
	}
}

enum coherence Directory_MESI::refer_state(unsigned long addr) {
	unsigned long i;

	if (dir.find(addr) == dir.end()) {
		pair<unsigned char *, enum coherence> *dir_element = new pair<unsigned char *, enum coherence>;
		dir_element->first = new unsigned char[numCore];
		dir_element->second = INVALID;
		for (i = 0; i < numCore; i++)
			dir_element->first[i] = 'I';
		dir[addr] = dir_element;
	}

	return dir[addr]->second;
}

void Directory_Manager_Interleave::set_busy(unsigned long addr, void *argReqAddr) {
	unsigned long idx = (addr >> log2blockSize) & idxMask;
	dir[idx].set_busy(argReqAddr);
}

void Directory_Manager_Interleave::clear_busy(unsigned long addr) {
	unsigned long idx = (addr >> log2blockSize) & idxMask;
	dir[idx].clear_busy();
}

void Directory_Manager_Interleave::do_request(unsigned long cache_id, unsigned long addr, unsigned char op, Cache_MESI **L2s, bool *memRead, ReqPrevCacheReadManager **reqRead, ReqPrevCacheInvalidateManager **reqInv) {
	unsigned long idx = (addr >> log2blockSize) & idxMask;
	dir[idx].do_request(cache_id, addr & ~blkMask, op, L2s, memRead, reqRead, reqInv);
}

void Directory_Manager_Interleave::do_request_WB(unsigned long cache_id, unsigned long addr, unsigned char op, ReqPrevCacheInvalidateManager **reqInv) {
	unsigned long idx = (addr >> log2blockSize) & idxMask;
	dir[idx].do_request_WB(cache_id, addr & ~blkMask, op, reqInv);
}

enum coherence Directory_Manager_Interleave::refer_state(unsigned long addr) {
	unsigned long idx = (addr >> log2blockSize) & idxMask;
	return dir[idx].refer_state(addr & ~blkMask);
}

void Directory_Manager_Interleave::simLatency() {
	for (unsigned long i = 0; i < numDir; i++)
		dir[i].simLatency();
}

unsigned long Directory_Manager_Interleave::getId(unsigned long addr) {
	unsigned long idx = (addr >> log2blockSize) & idxMask;
	return dir[idx].getId();
}
unsigned long Directory_Manager_Interleave::getBusyCycle(unsigned long addr) {
	unsigned long idx = (addr >> log2blockSize) & idxMask;
	return dir[idx].getBusyCycle();
}
void *Directory_Manager_Interleave::getReqAddr(unsigned long addr) {
	unsigned long idx = (addr >> log2blockSize) & idxMask;
	return dir[idx].getReqAddr();
}

bool Directory_Manager_Interleave::isBusy(unsigned long addr) {
	unsigned long idx = (addr >> log2blockSize) & idxMask;
	return dir[idx].isBusy();
}

string Directory_Manager_Interleave::printLine(unsigned long addr) {
	unsigned long idx = (addr >> log2blockSize) & idxMask;
	return dir[idx].printLine(addr & ~blkMask);
}

bool Directory_MESI::printStatus_nextLine() {
	bool emptyLine = false;

	while (!emptyLine && printStatusLine != dir.end()) {
		if (printStatusLine->second->second != INVALID) {
			emptyLine = true;
		}
		printStatusLine++;
	}
	if (emptyLine) {
		printStatusLine--;
		cout << hex << setw(8) << (printStatusLine)->first << dec << " ";
		switch ((printStatusLine)->second->second) {
		case INVALID:
			cout << "I ";
			break;
		case SHARED:
			cout << "S ";
			break;
		case EXCLUSIVE:
			cout << "E ";
			break;
		default:
			assert(0);
		}
		for (unsigned long i = 0; i < numCore; i++)
			cout << (printStatusLine)->second->first[i];
		cout << "            ";
		printStatusLine++;
	} else
		cout << "                                       ";
	if (printStatusLine != dir.end())
		return true;
	else
		return false;
}

string Directory_MESI::printLine(unsigned long addr) {
	string str;
	if (dir.find(addr) != dir.end()) {
		switch (dir[addr]->second) {
		case INVALID:
			str = "I ";
			break;
		case SHARED:
			str = "S ";
			break;
		case EXCLUSIVE:
			str = "E ";
			break;
		default:
			assert(0);
		}
		for (unsigned long i = 0; i < numCore; i++)
			str += dir[addr]->first[i];
		return str;
	}
	return " ";
}

void Directory_MESI::printStat() {
	cout << "MemReads       " << memReads << "\n";
	cout << "MemWrites      " << memWrites << "\n";
	cout << "MemFootPrint   " << dir.size() << "\n";
	for (unsigned long i = 0; i < numCore; i++) {
		if (!i)
			cout << "req.s from L2  ";
		cout << setw(10) << requests[i];
	}
	cout << "\n";
	for (unsigned long i = 0; i < numCore; i++) {
		if (!i)
			cout << "WB.s  from L2  ";
		cout << setw(10) << writebacks[i];
	}
	cout << "\n";
	for (unsigned long i = 0; i < numCore; i++) {
		if (!i)
			cout << "inv.s from L2  ";
		cout << setw(10) << invalidates[i];
	}
	cout << "\n";
	for (unsigned long i = 0; i < numCore; i++) {
		if (!i)
			cout << "inv.s   to L2  ";
		cout << setw(10) << invalidated[i];
	}
	cout << "\n";
	for (unsigned long i = 0; i < numCore; i++) {
		if (!i)
			cout << "read.s  to L2  ";
		cout << setw(10) << read[i];
	}
	cout << "\n";
	for (unsigned long i = 0; i < numCore; i++) {
		if (!i)
			cout << "exclu.s to L2  ";
		cout << setw(10) << exclusived[i];
	}
	cout << "\n";
}
