/*
 * sim.cc
 *
 *  Created on: Mar 22, 2014
 *      Author: Joonmoo Huh
 */

#include "sim.h"

void Sim_16core_Shared_HW::simLatency() {
	for (unsigned long i = 0; i < 16; i++) {
		L1_cache[i]->simLatency();
		L2_cache[i]->simLatency();
	}
	directory->simLatency();
	mem->simLatency();
}

void Sim_16core_Shared_HW::printStatus_L1s() {
	unsigned long i;
	bool remainLines = true;

	for (i = 0; i < 16; i++)
		if (printStatus[i])
			L1_cache[i]->printStatus_reset();
	for (i = 0; i < 16; i++)
		if (printStatus[i]) {
			cout << "[L1Cache(" << setw(2) << i << ") Status: ";
			if (L1_cache[i]->isP1Busy())
				cout << "P1Busy ";
			else
				cout << "P1Idle ";
			if (L1_cache[i]->isP2Busy())
				cout << "P2Busy ]   ";
			else
				cout << "P2Idle ]   ";
		}
	cout << "\n";
	while (remainLines) {
		remainLines = false;
		for (i = 0; i < 16; i++)
			if (printStatus[i]) {
				if (L1_cache[i]->printStatus_nextLine())
					remainLines = true;
			}
		cout << "\n";
	}
}

void Sim_16core_Shared_HW::printStatus_L2s() {
	unsigned long i;
	bool remainLines = true;

	for (i = 0; i < 16; i++)
		if (printStatus[i])
			L2_cache[i]->printStatus_reset();
	for (i = 0; i < 16; i++)
		if (printStatus[i]) {
			cout << "[L2Cache(" << setw(2) << i << ") Status: ";
			if (L2_cache[i]->isP1Busy())
				cout << "P1Busy ";
			else
				cout << "P1Idle ";
			if (L2_cache[i]->isP2Busy())
				cout << "P2Busy ]   ";
			else
				cout << "P2Idle ]   ";
		}
	cout << "\n";
	while (remainLines) {
		remainLines = false;
		for (i = 0; i < 16; i++)
			if (printStatus[i]) {
				if (L2_cache[i]->printStatus_nextLine())
					remainLines = true;
			}
		cout << "\n";
	}
}

void Sim_16core_Shared_HW::printStatus_Dirs() {
	unsigned long i;
	bool remainLines = true;

	for (i = 0; i < 16; i++)
		if (printStatus[i])
			directory->getDirectoryIdx(i)->printStatus_reset();
	for (i = 0; i < 16; i++)
		if (printStatus[i]) {
			cout << "[Directory(" << setw(2) << i << ") Status: ";
			if (directory->getDirectoryIdx(i)->isBusy())
				cout << "Busy        ]   ";
			else
				cout << "Idle        ]   ";
		}
	cout << "\n";
	while (remainLines) {
		remainLines = false;
		for (i = 0; i < 16; i++)
			if (printStatus[i]) {
				if (directory->getDirectoryIdx(i)->printStatus_nextLine())
					remainLines = true;
			}
		cout << "\n";
	}
}

void Sim_16core_Shared_HW::printStatus_Inflight() {
	unsigned int i = 0;
	cout << "[Inflight access Status]\n";
	for (map<unsigned long, unsigned long>::iterator I = inflight->getBegin(),
			E = inflight->getEnd(); I != E; I++, i++) {
		cout << hex << setw(8) << I->first << " " << dec;
		if (i % 8 == 0)
			cout << "\n";
	}
}

void Sim_16core_Message_HW::printStatus_L1s() {
	unsigned long i;
	bool remainLines = true;

	for (i = 0; i < 16; i++)
		if (printStatus[i])
			L1_cache[i]->printStatus_reset();
	for (i = 0; i < 16; i++)
		if (printStatus[i]) {
			cout << "[L1Cache(" << setw(2) << i << ") Status: ";
			if (L1_cache[i]->isP1Busy())
				cout << "P1Busy ";
			else
				cout << "P1Idle ";
			if (L1_cache[i]->isP2Busy())
				cout << "P2Busy ]   ";
			else
				cout << "P2Idle ]   ";
		}
	cout << "\n";
	while (remainLines) {
		remainLines = false;
		for (i = 0; i < 16; i++)
			if (printStatus[i]) {
				if (L1_cache[i]->printStatus_nextLine())
					remainLines = true;
			}
		cout << "\n";
	}
}

void Sim_16core_Message_HW::printStatus_L2s() {
	unsigned long i;
	bool remainLines = true;

	for (i = 0; i < 16; i++)
		if (printStatus[i])
			L2_cache[i]->printStatus_reset();
	for (i = 0; i < 16; i++)
		if (printStatus[i]) {
			cout << "[L2Cache(" << setw(2) << i << ") Status: ";
			if (L2_cache[i]->isP1Busy())
				cout << "P1Busy ";
			else
				cout << "P1Idle ";
			if (L2_cache[i]->isP2Busy())
				cout << "P2Busy ]   ";
			else
				cout << "P2Idle ]   ";
		}
	cout << "\n";
	while (remainLines) {
		remainLines = false;
		for (i = 0; i < 16; i++)
			if (printStatus[i]) {
				if (L2_cache[i]->printStatus_nextLine())
					remainLines = true;
			}
		cout << "\n";
	}
}

void Sim_16core_Shared_HW::printStat() {
	cout << "[L1Caches]\n";
	cout << "Reads          ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printStatReads();
	cout << "\nRead Misses    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printStatReadMisses();
	cout << "\nWrites         ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printStatWrites();
	cout << "\nWrites Misses  ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printStatWriteMisses();
	cout << "\nInvalidated    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printStatInvalidated();
	cout << "\n";

	cout << "[L2Caches]\n";
	cout << "Reads          ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatReads();
	cout << "\nRead Misses    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatReadMisses();
	cout << "\nWrites         ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatWrites();
	cout << "\nWrites Misses  ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatWriteMisses();
	cout << "\nWritesBacks    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatWriteBacks();
	cout << "\nInvalidates    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatInvalidates();
	cout << "\nInvalidated    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatInvalidated();
	cout << "\nRead           ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatRead();
	cout << "\nExclusived     ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatExclusived();
	cout << "\nDirtyBlocks    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printNumDirtyBlocks();
	cout << "\n";

	for (unsigned long i = 0; i < 16; i++) {
		cout << "[Dir " << setw(2) << i << "  ]\n";
		directory->getDirectoryIdx(i)->printStat();
	}

	/*for (unsigned long i = 0; i < 16; i++) {
		cout << "\n[L2(" << i << ") HeatMap]\n";
		L2_cache[i]->printStatAccessesSets();
	}*/
}

void Sim_16core_Message_HW::printStat() {
	cout << "[L1Caches]\n";
	cout << "Reads          ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printStatReads();
	cout << "\nRead Misses    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printStatReadMisses();
	cout << "\nWrites         ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printStatWrites();
	cout << "\nWrites Misses  ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printStatWriteMisses();
	cout << "\nWritesBacks    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printStatWriteBacks();
	cout << "\nDirtyBlocks    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printNumDirtyBlocks();
	cout << "\nMP Flushed     ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printStatMPflushed();
	cout << "\nMP Writebacks  ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L1_cache[i]->printStatMPwritebacks();
	cout << "\n";

	cout << "[L2Caches]\n";
	cout << "Reads          ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatReads();
	cout << "\nRead Misses    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatReadMisses();
	cout << "\nWrites         ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatWrites();
	cout << "\nWrites Misses  ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatWriteMisses();
	cout << "\nWritesBacks    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatWriteBacks();
	cout << "\nDirtyBlocks    ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printNumDirtyBlocks();
	cout << "\nMP Flushed     ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatMPflushed();
	cout << "\nMP Writebacks  ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatMPwritebacks();
#ifdef CTOC
	cout << "\nMP fromCache   ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatMPfromCache();
	cout << "\nMP fromMem     ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatMPfromMem();
	cout << "\nMP toCache     ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatMPtoCache();
	cout << "\nMP toMem       ";
	for (unsigned long i = 0; i < 16; i++)
		cout << setw(10) << L2_cache[i]->printStatMPtoMem();
#endif
	cout << "\n";
}

unsigned long Trace_Manager::string2hex(char *addr) {
	int i;
	unsigned long inc = 1;
	int len = strlen(addr) - 1;
	unsigned long dec_sum = 0;

	for (i = 0; i < len + 1; i++) {
		switch (*(addr + len - i)) {
		case '1':
			dec_sum += 1 * inc;
			break;
		case '2':
			dec_sum += 2 * inc;
			break;
		case '3':
			dec_sum += 3 * inc;
			break;
		case '4':
			dec_sum += 4 * inc;
			break;
		case '5':
			dec_sum += 5 * inc;
			break;
		case '6':
			dec_sum += 6 * inc;
			break;
		case '7':
			dec_sum += 7 * inc;
			break;
		case '8':
			dec_sum += 8 * inc;
			break;
		case '9':
			dec_sum += 9 * inc;
			break;
		case 'a':
		case 'A':
			dec_sum += 10 * inc;
			break;
		case 'b':
		case 'B':
			dec_sum += 11 * inc;
			break;
		case 'c':
		case 'C':
			dec_sum += 12 * inc;
			break;
		case 'd':
		case 'D':
			dec_sum += 13 * inc;
			break;
		case 'e':
		case 'E':
			dec_sum += 14 * inc;
			break;
		case 'f':
		case 'F':
			dec_sum += 15 * inc;
			break;
		default:
			break;
		}
		if (*(addr + len - i)!='\n')
			inc = inc * 16;
	}
	return dec_sum;
}

void Sim_16core_Shared_Request::do_L1_access() {
	if (L1_access == NULL)
		return;
	if (hw->getL1Cache(pid)->isP1Busy()
			&& hw->getL1Cache(pid)->getP1ReqAddr() != (void *) L1_access)
		return;

	if (!hw->getL1Cache(pid)->isP1Busy())
		hw->getL1Cache(pid)->set_P1busy((void *) L1_access);
	if (hw->getL1Cache(pid)->getP1BusyCycle())
		return;

	hw->getL1Cache(pid)->do_access(L1_access->getAddr(), L1_access->getOp(), &L2_access);
	delete L1_access;
	L1_access = NULL;

	if (L2_access == NULL)
		hw->getL1Cache(pid)->clear_P1busy();
}

void Sim_16core_Shared_Request::do_L2_access() {
	if (L2_access == NULL)
		return;
	if (hw->getL2Cache(pid)->isP1Busy() && hw->getL2Cache(pid)->getP1ReqAddr() != (void *) L2_access)
		return;

	unsigned long addr_wb_tmp = 0;
	if (!hw->getL2Cache(pid)->isP1Busy())
		hw->getL2Cache(pid)->set_P1busy((void *) L2_access);
	if (hw->getL2Cache(pid)->getP1BusyCycle())
		return;
	if (hw->getDirectoryManager()->isBusy(addr))
		return;
	if (hw->getInflight()->is_inflight(addr))
		return;
	addr_wb_tmp = hw->getL2Cache(pid)->check_WB(L2_access->getAddr(), L2_access->getOp());
	if (addr_wb_tmp) {
		assert(addr != addr_wb_tmp);
		if (hw->getInflight()->is_inflight(addr_wb_tmp))
			return;
		hw->getInflight()->add_access(addr_wb_tmp);
	}
	hw->getInflight()->add_access(addr);

	assert(addr == L2_access->getAddr());
	hw->getL2Cache(pid)->do_access(L2_access->getAddr(), L2_access->getOp(), hw->getDirectoryManager(), &dir_access, &dir_access_WB, &L1_inv_self);
	delete L2_access;
	L2_access = NULL;

	assert(dir_access!=NULL || L1_inv_self==NULL);
	assert(dir_access!=NULL || dir_access_WB==NULL);
	assert(
			(dir_access_WB==NULL && L1_inv_self==NULL) || (dir_access_WB!=NULL && L1_inv_self!=NULL));
	if (dir_access == NULL) {
		hw->getInflight()->drop_access(addr);
		hw->getL2Cache(pid)->clear_P1busy();
		hw->getL1Cache(pid)->clear_P1busy();
	}
	if (dir_access_WB) {
		addr_wb = dir_access_WB->getAddr();
		assert(addr_wb_tmp == addr_wb);
	}
}

void Sim_16core_Shared_Request::do_dir_request() {
	if (dir_access == NULL)
		return;
	if (hw->getDirectoryManager()->isBusy(addr) && hw->getDirectoryManager()->getReqAddr(addr) != (void *) dir_access)
		return;

	if (!hw->getDirectoryManager()->isBusy(addr))
		hw->getDirectoryManager()->set_busy(addr, (void *) dir_access);
	if (hw->getDirectoryManager()->getBusyCycle(addr))
		return;

	hw->getDirectoryManager()->do_request(dir_access->getCacheId(), dir_access->getAddr(), dir_access->getOp(), hw->getL2CacheList(), &mem_read, &L2_read, &L2_inv);
	delete dir_access;
	dir_access = NULL;

	assert(L2_read==NULL || L2_inv==NULL);
	assert(L2_inv==NULL || !mem_read);
	assert(L2_read==NULL || !mem_read);

	if (L2_read == NULL && L2_inv == NULL && !mem_read) {
		hw->getDirectoryManager()->clear_busy(addr);
		hw->getInflight()->drop_access(addr);
		hw->getL2Cache(pid)->clear_P1busy();
		hw->getL1Cache(pid)->clear_P1busy();
	}
	if (mem_read)
		hw->getMemory()->add_request(&mem_read);
}

void Sim_16core_Shared_Request::do_dir_request_WB() {
	if (dir_access_WB == NULL)
		return;
	if (hw->getDirectoryManager()->isBusy(dir_access_WB->getAddr()) && hw->getDirectoryManager()->getReqAddr(dir_access_WB->getAddr()) != (void *) dir_access_WB)
		return;

	if (!hw->getDirectoryManager()->isBusy(dir_access_WB->getAddr()))
		hw->getDirectoryManager()->set_busy(dir_access_WB->getAddr(), (void *) dir_access_WB);
	if (hw->getDirectoryManager()->getBusyCycle(dir_access_WB->getAddr()))
		return;

	assert(addr_wb == dir_access_WB->getAddr());
	hw->getDirectoryManager()->do_request_WB(dir_access_WB->getCacheId(), dir_access_WB->getAddr(), dir_access_WB->getOp(), &L2_inv_WB);
	delete dir_access_WB;
	dir_access_WB = NULL;

	if (L2_inv_WB == NULL) {
		hw->getDirectoryManager()->clear_busy(addr_wb);
		hw->getInflight()->drop_access(addr_wb);
		addr_wb = 0;
	}
}

void Sim_16core_Shared_Request::do_L2_read() {
	if (L2_read == NULL)
		return;
	if (L2_read->getShortestI() == L2_read->getInvolveCoresEnd()) {
		unsigned long shortest_L2 = hw->getNetwork()->getShortest(L2_read->getDirId(), L2_read->getInvolveCores());
		list<pair<bool, unsigned long> *>::iterator I, E;
		for (I = L2_read->getInvolveCoresBegin(), E = L2_read->getInvolveCoresEnd(); I != E; I++)
			if ((*I)->second == shortest_L2) {
				L2_read->setShortestI(I);
				break;
			}
		assert(I != E);
	}

	if (hw->getL2Cache((*L2_read->getShortestI())->second)->isP2Busy() && hw->getL2Cache((*L2_read->getShortestI())->second)->getP2ReqAddr() != (void *) L2_read)
		return;

	if (!hw->getL2Cache((*L2_read->getShortestI())->second)->isP2Busy())
		hw->getL2Cache((*L2_read->getShortestI())->second)->set_P2busy((void *) L2_read);
	if (hw->getL2Cache((*L2_read->getShortestI())->second)->getP2BusyCycle())
		return;
	hw->getL2Cache((*L2_read->getShortestI())->second)->do_read(L2_read->getAddr());
	hw->getL2Cache((*L2_read->getShortestI())->second)->clear_P2busy();
	hw->getDirectoryManager()->getDirectoryIdx(L2_read->getDirId())->incRead((*L2_read->getShortestI())->second);
	delete L2_read;
	L2_read = NULL;

	hw->getDirectoryManager()->clear_busy(addr);
	hw->getInflight()->drop_access(addr);
	hw->getL2Cache(pid)->clear_P1busy();
	hw->getL1Cache(pid)->clear_P1busy();
}

void Sim_16core_Shared_Request::do_L2_inv() {
	if (L2_inv == NULL)
		return;

	list<pair<bool, unsigned long> *>::iterator I, E;
	for (I = L2_inv->getInvolveCoresBegin(), E = L2_inv->getInvolveCoresEnd(); I != E; I++) {
		if (!(*I)->first)
			continue;
		if (hw->getL2Cache((*I)->second)->isP2Busy() && hw->getL2Cache((*I)->second)->getP2ReqAddr() != (void *) L2_inv)
			continue;

		if (!hw->getL2Cache((*I)->second)->isP2Busy())
			hw->getL2Cache((*I)->second)->set_P2busy((void *) L2_inv);
		if (hw->getL2Cache((*I)->second)->getP2BusyCycle())
			continue;
		pair<bool, ReqPrevCacheInvalidate *> *req;
		req = new pair<bool, ReqPrevCacheInvalidate *>;
		req->first = true;
		hw->getL2Cache((*I)->second)->do_invalidate(L2_inv->getAddr(), L2_inv->getTagMask(), &(req->second));
		hw->getL2Cache((*I)->second)->clear_P2busy();
		hw->getDirectoryManager()->getDirectoryIdx(L2_inv->getDirId())->incInvalidated((*I)->second);
		(*I)->first = false;

		assert(req->second!=NULL);
		L1_inv.push_back(req);
	}

	for (I = L2_inv->getInvolveCoresBegin(), E = L2_inv->getInvolveCoresEnd(); I != E; I++)
		if ((*I)->first)
			break;
	if (I == E) {
		if (L1_inv.empty()) {
			delete L2_inv;
			L2_inv = NULL;
			hw->getDirectoryManager()->clear_busy(addr);
			hw->getInflight()->drop_access(addr);
			hw->getL2Cache(pid)->clear_P1busy();
			hw->getL1Cache(pid)->clear_P1busy();
		}
	}
}

void Sim_16core_Shared_Request::do_L1_inv() {
	if (L1_inv.empty())
		return;

	list<pair<bool, ReqPrevCacheInvalidate *> *>::iterator I, E;
	for (I = L1_inv.begin(), E = L1_inv.end(); I != E; I++) {
		if (!(*I)->first)
			continue;
		if (hw->getL1Cache((*I)->second->getCacheId())->isP2Busy() && hw->getL1Cache((*I)->second->getCacheId())->getP2ReqAddr() != (void *) &L1_inv)
			continue;

		if (!hw->getL1Cache((*I)->second->getCacheId())->isP2Busy())
			hw->getL1Cache((*I)->second->getCacheId())->set_P2busy((void *) &L1_inv);
		if (hw->getL1Cache((*I)->second->getCacheId())->getP2BusyCycle())
			continue;
		hw->getL1Cache((*I)->second->getCacheId())->do_invalidate((*I)->second->getAddr(), (*I)->second->getTagMask());
		hw->getL1Cache((*I)->second->getCacheId())->clear_P2busy();
		(*I)->first = false;
		delete (*I)->second;
	}

	for (I = L1_inv.begin(), E = L1_inv.end(); I != E; I++)
		if ((*I)->first)
			break;
	if (I == E) {
		L1_inv.clear();
	}
}

void Sim_16core_Shared_Request::do_L2_inv_WB() {
	if (L2_inv_WB == NULL)
		return;
	assert(L2_inv_WB->getInvolveCoresNum() == 1);
	if (hw->getL2Cache((*(L2_inv_WB->getInvolveCoresBegin()))->second)->isP2Busy() && hw->getL2Cache((*(L2_inv_WB->getInvolveCoresBegin()))->second)->getP2ReqAddr() != (void *) L2_inv_WB)
		return;

	if (!hw->getL2Cache((*(L2_inv_WB->getInvolveCoresBegin()))->second)->isP2Busy())
		hw->getL2Cache((*(L2_inv_WB->getInvolveCoresBegin()))->second)->set_P2busy((void *) L2_inv_WB);
	if (hw->getL2Cache((*(L2_inv_WB->getInvolveCoresBegin()))->second)->getP2BusyCycle())
		return;

	assert(addr_wb);
	hw->getL2Cache((*(L2_inv_WB->getInvolveCoresBegin()))->second)->do_exclusive(L2_inv_WB->getAddr());
	hw->getL2Cache((*(L2_inv_WB->getInvolveCoresBegin()))->second)->clear_P2busy();
	hw->getDirectoryManager()->getDirectoryIdx(L2_inv_WB->getDirId())->incExclusived((*(L2_inv_WB->getInvolveCoresBegin()))->second);
	delete L2_inv_WB;
	L2_inv_WB = NULL;

	hw->getDirectoryManager()->clear_busy(addr_wb);
	hw->getInflight()->drop_access(addr_wb);
	addr_wb = 0;
}

void Sim_16core_Shared_Request::do_L1_inv_self() {
	if (L1_inv_self == NULL)
		return;

	assert((!hw->getL1Cache(pid)->isP2Busy() && hw->getL1Cache(pid)->getP2ReqAddr()==NULL) || (hw->getL1Cache(pid)->isP2Busy() && hw->getL1Cache(pid)->getP2ReqAddr()!=NULL));

	if (!hw->getL1Cache(pid)->isP2Busy())
		hw->getL1Cache(pid)->set_P2busy((void *) L1_inv_self);
	if (hw->getL1Cache(pid)->getP2BusyCycle())
		return;

	hw->getL1Cache(pid)->do_invalidate(L1_inv_self->getAddr(), L1_inv_self->getTagMask());
	delete L1_inv_self;
	L1_inv_self = NULL;

	hw->getL1Cache(pid)->clear_P2busy();
}

void Sim_16core_Shared_Request::do_mem_read() {
	if (!mem_read)
		return;

	if (hw->getMemory()->getBusyCycle((void *) &mem_read))
		return;

	hw->getMemory()->drop_request((void *) &mem_read);
	mem_read = false;

	hw->getDirectoryManager()->clear_busy(addr);
	hw->getInflight()->drop_access(addr);
	hw->getL2Cache(pid)->clear_P1busy();
	hw->getL1Cache(pid)->clear_P1busy();
}

void Sim_16core_Message_Request::do_L1_access() {
	if (L1_access == NULL)
		return;
	if (hw->getL1Cache(pid)->isP1Busy() && hw->getL1Cache(pid)->getP1ReqAddr() != (void *) L1_access)
		return;

	if (!hw->getL1Cache(pid)->isP1Busy())
		hw->getL1Cache(pid)->set_P1busy((void *) L1_access);
	if (hw->getL1Cache(pid)->getP1BusyCycle())
		return;

	hw->getL1Cache(pid)->do_access(L1_access->getAddr(), L1_access->getOp(), &L2_access_WB, &L2_access);
	delete L1_access;
	L1_access = NULL;

	if (L2_access_WB == NULL && L2_access == NULL)
		hw->getL1Cache(pid)->clear_P1busy();
}

void Sim_16core_Message_Request::do_L2_access_WB() {
	if (L2_access_WB == NULL)
		return;
	if (hw->getL2Cache(pid)->isP1Busy() && hw->getL2Cache(pid)->getP1ReqAddr() != (void *) L2_access_WB)
		return;

	if (!hw->getL2Cache(pid)->isP1Busy())
		hw->getL2Cache(pid)->set_P1busy((void *) L2_access_WB);
	if (hw->getL2Cache(pid)->getP1BusyCycle())
		return;

	assert(addr != L2_access_WB->getAddr());
	ReqNextCacheAccess *mem_access = NULL;
	ReqNextCacheAccess *mem_access_WB = NULL;
	hw->getL2Cache(pid)->do_access(L2_access_WB->getAddr(), L2_access_WB->getOp(), &mem_access_WB, &mem_access);
	delete L2_access_WB;
	L2_access_WB = NULL;

	if (mem_access) {
		assert(mem_access->getOp() == 'w');
		delete mem_access;
	}
	if (mem_access_WB) {
		delete mem_access_WB;
	}

	hw->getL2Cache(pid)->clear_P1busy();

	if (L2_access == NULL)
		hw->getL1Cache(pid)->clear_P1busy();
}

void Sim_16core_Message_Request::do_L2_access() {
	if (L2_access == NULL)
		return;
	if (hw->getL2Cache(pid)->isP1Busy() && hw->getL2Cache(pid)->getP1ReqAddr() != (void *) L2_access)
		return;

	if (!hw->getL2Cache(pid)->isP1Busy())
		hw->getL2Cache(pid)->set_P1busy((void *) L2_access);
	if (hw->getL2Cache(pid)->getP1BusyCycle())
		return;

	assert(addr == L2_access->getAddr());
	ReqNextCacheAccess *mem_access = NULL;
	ReqNextCacheAccess *mem_access_WB = NULL;
	hw->getL2Cache(pid)->do_access(L2_access->getAddr(), L2_access->getOp(), &mem_access_WB, &mem_access);
	delete L2_access;
	L2_access = NULL;

	if (mem_access_WB) {
		delete mem_access_WB;
	}

	if (mem_access) {
		if (mem_access->getOp() == 'r')
			mem_read = true;
		delete mem_access;
	}
	if (!mem_read) {
		hw->getL2Cache(pid)->clear_P1busy();
		hw->getL1Cache(pid)->clear_P1busy();
	}
}

void Sim_16core_Message_Request::do_mem_read() {
	if (!mem_read)
		return;

	if (hw->getMemory(pid)->getBusyCycle((void *) &mem_read))
		return;

	hw->getMemory(pid)->drop_request((void *) &mem_read);
	mem_read = false;

	hw->getL2Cache(pid)->clear_P1busy();
	hw->getL1Cache(pid)->clear_P1busy();
}

void Sim_16core_Message_Request::do_MP_access() {
	unsigned int i;
	unsigned long long commWith;
	unsigned long involvedCores, cycleL1Send, cycleL1wb, cycleL1Recv, cycleL2Send, cycleL2wb, cycleL2Recv, cycleTotal, cycleMax;

	if (!MP_access)
		return;

	if (hw->getMPIControl()->isEnd(pid)) {
		if (hw->getMPIControl()->getEndCycle(pid))
			return;
		hw->getMPIControl()->clear_end(pid);
		delete MP_access;
		MP_access = NULL;
		return;
	}

	if (hw->getMPIControl()->isBusy(pid) && hw->getMPIControl()->getReqAddr(pid) != (void *) MP_access)
		return;

	if (!hw->getMPIControl()->isBusy(pid))
		hw->getMPIControl()->set_busy(pid, (void *) MP_access);
	if (hw->getMPIControl()->getBusyCycle(pid))
		return;

	hw->getMPIControl()->setReadyMessage(pid, MP_access->getMpiKind());
	switch (hw->getMPIControl()->getReadyMessage(pid)) {
	case kd_MPI_Send:
	case kd_MPI_Recv:
		if (hw->getMPIControl()->getReadyMessage(MP_access->getSrcDest()) != kd_MPI_Send && hw->getMPIControl()->getReadyMessage(MP_access->getSrcDest()) != kd_MPI_Recv)
			return;
		cerr << "MPI PtoP : " << pid << " & " << MP_access->getSrcDest() << "\n";
		cout << "MPI PtoP : " << pid << " & " << MP_access->getSrcDest() << "\n";
		hw->getMPIControl()->clear_busy(MP_access->getSrcDest());
		hw->getMPIControl()->set_end(MP_access->getSrcDest(), 0);
		hw->getMPIControl()->clear_busy(pid);
		delete MP_access;
		MP_access = NULL;
		break;
	case kd_MPI_Barrier:
		commWith = MP_access->getCommWith();
		involvedCores = 1;
		for (i=0; i<16; i++) {
			if (pid != i) {
				if (commWith & 1) {
					if (hw->getMPIControl()->getReqAddr(i)==NULL)
						return;
					if (MP_access->getMpiKind() != ((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getMpiKind()) {
						unsigned long long temp = 1;
						assert (((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getCommWith() & (temp <<= pid));
						return;
					}
					involvedCores++;
				}
			}
			commWith >>= 1;
		}

		cerr << ">>>>>>>>>> MPI Barrier (" << involvedCores << ")<<<<<<<<<<\n";
		cout << ">>>>>>>>>> MPI Barrier (" << involvedCores << ")<<<<<<<<<<\n";
		commWith = MP_access->getCommWith();
		for (i=0; i<16; i++) {
			if (commWith & 1) {
				cerr << "  p" << i << " \n";
				cout << "  p" << i << " \n";
			}
			commWith >>= 1;
		}

		commWith = MP_access->getCommWith();
		for (i=0; i<16; i++) {
			if (commWith & 1) {
				hw->getMPIControl()->clear_busy(i);
				hw->getMPIControl()->set_end(i, 1);
			}
			commWith >>= 1;
		}
		break;
	case kd_MPI_Bcast:
		commWith = MP_access->getCommWith();
		involvedCores = 1;
		for (i=0; i<16; i++) {
			if (pid != i) {
				if (commWith & 1) {
					if (hw->getMPIControl()->getReqAddr(i)==NULL)
						return;
					if (MP_access->getMpiKind() != ((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getMpiKind()) {
						unsigned long long temp = 1;
						assert (((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getCommWith() & (temp <<= pid));
						return;
					}
					involvedCores++;
				}
			}
			commWith >>= 1;
		}

		cerr << ">>>>>>>>>> MPI Bcast (" << involvedCores << "," << MP_access->getBytes() << ")<<<<<<<<<<\n";
		cout << ">>>>>>>>>> MPI Bcast (" << involvedCores << "," << MP_access->getBytes() << ")<<<<<<<<<<\n";
		commWith = MP_access->getCommWith();
		cycleMax = 0;
		for (i=0; i<16; i++) {
			cycleTotal = 0;
			if (commWith & 1) {
#ifndef CTOC
				if (i == MP_access->getRoot()) {
					cycleL1wb = cycleL2wb = 0;
					cycleL1Send = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), true, &cycleL1wb);
					cycleL2Send = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), true, &cycleL2wb);
					cycleTotal = cycleL1Send + cycleL1wb*10 + cycleL2Send + cycleL2wb*150;
					cycleTotal += (150)*(MP_access->getBytes()/64);
					cerr << "  p" << i << " (Root " << cycleL1Send << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2wb << "," << cycleTotal << ")\n";
					cout << "  p" << i << " (Root " << cycleL1Send << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2wb << "," << cycleTotal << ")\n";
				}
				else {
					cycleL1wb = cycleL2wb = 0;
					cycleL1Recv = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), false, &cycleL1wb);
					assert(!cycleL1wb);
					cycleL2Recv = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), false, &cycleL2wb);
					assert(!cycleL2wb);
					cycleTotal = cycleL2Recv;
					cycleTotal += (150)*(MP_access->getBytes()/64);
					cerr << "  p" << i << " (" << cycleL1Recv << "," << cycleL2Recv << "," << cycleTotal << ")\n";
					cout << "  p" << i << " (" << cycleL1Recv << "," << cycleL2Recv << "," << cycleTotal << ")\n";
				}
#endif
#ifdef CTOC
				if (i == MP_access->getRoot()) {
					cycleL1wb = cycleL2wb = 0;
					cycleL1Send = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), true, &cycleL1wb);
					cycleL2Send = hw->getL2Cache(i)->do_send(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), &cycleL2wb);
					cycleTotal = cycleL2Send + cycleL2wb*150;
					cerr << "  p" << i << " (Root " << cycleL1Send << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2wb << "," << cycleTotal << ")\n";
					cout << "  p" << i << " (Root " << cycleL1Send << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2wb << "," << cycleTotal << ")\n";

				}
				else {
					cycleL1wb = cycleL2wb = 0;
					cycleL1Recv = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), false, &cycleL1wb);
					cycleL2Recv = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), false, &cycleL2wb);
					assert(!cycleL1wb && !cycleL2wb);
					cycleL2Recv += hw->getL2Cache(i)->do_recv(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), &cycleL2wb);
					cycleTotal = cycleL2Recv + cycleL2wb*150;

					cerr << "  p" << i << " (" << cycleL1Recv << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
					cout << "  p" << i << " (" << cycleL1Recv << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
				}
#endif
			}
			commWith >>= 1;
			if (cycleMax<cycleTotal)
				cycleMax = cycleTotal;
		}
		cerr << "  MP Cycles: " << cycleMax << "\n";
		cout << "  MP Cycles: " << cycleMax << "\n";

		commWith = MP_access->getCommWith();
		for (i=0; i<16; i++) {
			if (commWith & 1) {
				hw->getMPIControl()->clear_busy(i);
				hw->getMPIControl()->set_end(i, cycleMax);
			}
			commWith >>= 1;
		}
		break;
	case kd_MPI_Reduce:
		commWith = MP_access->getCommWith();
		involvedCores = 1;
		for (i=0; i<16; i++) {
			if (pid != i) {
				if (commWith & 1) {
					if (hw->getMPIControl()->getReqAddr(i)==NULL)
						return;
					if (MP_access->getMpiKind() != ((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getMpiKind()) {
						unsigned long long temp = 1;
						assert (((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getCommWith() & (temp <<= pid));
						return;
					}
					involvedCores++;
				}
			}
			commWith >>= 1;
		}

		cerr << ">>>>>>>>>> MPI Reduce (" << involvedCores << "," << MP_access->getBytes() <<  ")<<<<<<<<<<\n";
		cout << ">>>>>>>>>> MPI Reduce (" << involvedCores << "," << MP_access->getBytes() <<  ")<<<<<<<<<<\n";
		commWith = MP_access->getCommWith();
		cycleMax = 0;
		for (i=0; i<16; i++) {
			cycleTotal = 0;
			if (commWith & 1) {
#ifndef CTOC
				if (i == MP_access->getRoot()) {
					cycleL1wb = cycleL2wb = 0;
					cycleL1Recv = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), MP_access->getBytes(), false, &cycleL1wb);
					assert(!cycleL1wb);
					cycleL2Recv = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), MP_access->getBytes(), false, &cycleL2wb);
					assert(!cycleL2wb);
					cycleTotal = cycleL2Recv;
					cycleTotal += (150)*(MP_access->getBytes()/64);
					cerr << "  p" << i << " (Root " << cycleL1Recv << "," << cycleL2Recv << "," << cycleTotal << ")\n";
					cout << "  p" << i << " (Root " << cycleL1Recv << "," << cycleL2Recv << "," << cycleTotal << ")\n";
				}
				else {
					cycleL1wb = cycleL2wb = 0;
					cycleL1Send = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), true, &cycleL1wb);
					cycleL2Send = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), true, &cycleL2wb);
					cycleTotal = cycleL1Send + cycleL1wb*10 + cycleL2Send + cycleL2wb*150;
					cycleTotal += (150)*(MP_access->getBytes()/64);
					cerr << "  p" << i << " (" << cycleL1Send << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2wb << "," << cycleTotal << ")\n";
					cout << "  p" << i << " (" << cycleL1Send << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2wb << "," << cycleTotal << ")\n";}
#endif
#ifdef CTOC
				if (i == MP_access->getRoot()) {
					cycleL1wb = cycleL2wb = 0;
					cycleL1Recv = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), MP_access->getBytes(), false, &cycleL1wb);
					cycleL2Recv = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), MP_access->getBytes(), false, &cycleL2wb);
					assert(!cycleL1wb && !cycleL2wb);
					cycleL2Recv += hw->getL2Cache(i)->do_recv(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), MP_access->getBytes(), &cycleL2wb);
					cycleTotal = cycleL2Recv + cycleL2wb*150;
					cerr << "  p" << i << " (Root " << cycleL1Recv << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
					cout << "  p" << i << " (Root " << cycleL1Recv << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";

				}
				else {
					cycleL1wb = cycleL2wb = 0;
					cycleL1Send = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), true, &cycleL1wb);
					cycleL2Send = hw->getL2Cache(i)->do_send(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), &cycleL2wb);
					cycleTotal = cycleL2Send + cycleL2wb*150;
					cerr << "  p" << i << " (" << cycleL1Send << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2wb << "," << cycleTotal << ")\n";
					cout << "  p" << i << " (" << cycleL1Send << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2wb << "," << cycleTotal << ")\n";
				}
#endif
			}
			commWith >>= 1;
			if (cycleMax<cycleTotal)
				cycleMax = cycleTotal;
		}
		cerr << "  MP Cycles: " << cycleMax << "\n";
		cout << "  MP Cycles: " << cycleMax << "\n";

		commWith = MP_access->getCommWith();
		for (i=0; i<16; i++) {
			if (commWith & 1) {
				hw->getMPIControl()->clear_busy(i);
				hw->getMPIControl()->set_end(i, cycleMax);
			}
			commWith >>= 1;
		}

		break;
	case kd_MPI_Allreduce:
		commWith = MP_access->getCommWith();
		involvedCores = 1;
		for (i=0; i<16; i++) {
			if (pid != i) {
				if (commWith & 1) {
					if (hw->getMPIControl()->getReqAddr(i)==NULL)
						return;
					if (MP_access->getMpiKind() != ((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getMpiKind()) {
						unsigned long long temp = 1;
						assert (((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getCommWith() & (temp <<= pid));
						return;
					}
					involvedCores++;
				}
			}
			commWith >>= 1;
		}

		cerr << ">>>>>>>>>> MPI AllReduce (" << involvedCores << "," << MP_access->getBytes() <<  ")<<<<<<<<<<\n";
		cout << ">>>>>>>>>> MPI AllReduce (" << involvedCores << "," << MP_access->getBytes() <<  ")<<<<<<<<<<\n";
		commWith = MP_access->getCommWith();
		cycleMax = 0;
		for (i=0; i<16; i++) {
			cycleTotal = 0;
			if (commWith & 1) {
#ifndef CTOC
				cycleL1wb = cycleL2wb = 0;
				cycleL1Recv = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), MP_access->getBytes(), false, &cycleL1wb);
				assert(!cycleL1wb);
				cycleL1Send = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), true, &cycleL1wb);
				cycleL2Recv = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), MP_access->getBytes(), false, &cycleL2wb);
				assert(!cycleL2wb);
				cycleL2Send = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), true, &cycleL2wb);
				cycleTotal = cycleL1Send + cycleL1wb*10 + cycleL2Send;
				cycleTotal += cycleL2Recv;
				cycleTotal += cycleL2wb*150;
				cycleTotal += (2*150)*(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBytes())/64;
				cerr << "  p" << i << " (" << cycleL1Send << "," << cycleL1Recv << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
				cout << "  p" << i << " (" << cycleL1Send << "," << cycleL1Recv << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
#endif
#ifdef CTOC
				cycleL1wb = cycleL2wb = 0;
				cycleL1Recv = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), MP_access->getBytes(), false, &cycleL1wb);
				cycleL2Recv = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), MP_access->getBytes(), false, &cycleL2wb);
				assert(!cycleL1wb && !cycleL2wb);
				cycleL1Send = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), true, &cycleL1wb);
				cycleL2Send = hw->getL2Cache(i)->do_send(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), &cycleL2wb);
				cycleL2Send += cycleL2wb*150;
				cycleL2wb = 0;
				cycleL2Recv += hw->getL2Cache(i)->do_recv(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), MP_access->getBytes(), &cycleL2wb);
				cycleL2Recv += cycleL2wb*150;

				cycleTotal = cycleL1Send + cycleL1wb*10 + cycleL2Send + cycleL2Recv;

				cerr << "  p" << i << " (" << cycleL1Send << "," << cycleL1Recv << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
				cout << "  p" << i << " (" << cycleL1Send << "," << cycleL1Recv << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
#endif
			}
			commWith >>= 1;
			if (cycleMax<cycleTotal)
				cycleMax = cycleTotal;
		}
		cerr << "  MP Cycles: " << cycleMax << "\n";
		cout << "  MP Cycles: " << cycleMax << "\n";

		commWith = MP_access->getCommWith();
		for (i=0; i<16; i++) {
			if (commWith & 1) {
				hw->getMPIControl()->clear_busy(i);
				hw->getMPIControl()->set_end(i, cycleMax);
			}
			commWith >>= 1;
		}

		break;
	case kd_MPI_Allgather:
		commWith = MP_access->getCommWith();
		involvedCores = 1;
		for (i=0; i<16; i++) {
			if (pid != i) {
				if (commWith & 1) {
					if (hw->getMPIControl()->getReqAddr(i)==NULL)
						return;
					if (MP_access->getMpiKind() != ((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getMpiKind()) {
						unsigned long long temp = 1;
						assert (((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getCommWith() & (temp <<= pid));
						return;
					}
					involvedCores++;
				}
			}
			commWith >>= 1;
		}

		cerr << ">>>>>>>>>> MPI Allgather (" << involvedCores << "," << MP_access->getBytes() << "," << involvedCores*MP_access->getBytes() << ")<<<<<<<<<<\n";
		cout << ">>>>>>>>>> MPI Allgather (" << involvedCores << "," << MP_access->getBytes() << "," << involvedCores*MP_access->getBytes() << ")<<<<<<<<<<\n";
		commWith = MP_access->getCommWith();
		cycleMax = 0;
		for (i=0; i<16; i++) {
			cycleTotal = 0;
			if (commWith & 1) {
#ifndef CTOC
				cycleL1wb = cycleL2wb = 0;
				cycleL1Recv = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), involvedCores*MP_access->getBytes(), false, &cycleL1wb);
				assert(!cycleL1wb);
				cycleL1Send = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), true, &cycleL1wb);
				cycleL2Recv = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), involvedCores*MP_access->getBytes(), false, &cycleL2wb);
				assert(!cycleL2wb);
				cycleL2Send = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), true, &cycleL2wb);
				cycleTotal = cycleL1Send + cycleL1wb*10 + cycleL2Send;
				cycleTotal += cycleL2Recv;
				cycleTotal += cycleL2wb*150;
				cycleTotal += (150)*((((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBytes())/64*(involvedCores+1));
				cerr << "  p" << i << " (" << cycleL1Send << "," << cycleL1Recv << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
				cout << "  p" << i << " (" << cycleL1Send << "," << cycleL1Recv << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
#endif
#ifdef CTOC
				cycleL1wb = cycleL2wb = 0;
				cycleL1Recv = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), involvedCores*MP_access->getBytes(), false, &cycleL1wb);
				cycleL2Recv = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), involvedCores*MP_access->getBytes(), false, &cycleL2wb);
				assert(!cycleL1wb && !cycleL2wb);
				cycleL1Send = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), true, &cycleL1wb);
				cycleL2Send = hw->getL2Cache(i)->do_send(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), MP_access->getBytes(), &cycleL2wb);
				cycleL2Send += cycleL2wb*150;
				cycleL2wb = 0;
				cycleL2Recv += hw->getL2Cache(i)->do_recv(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), involvedCores*MP_access->getBytes(), &cycleL2wb);
				cycleL2Recv += cycleL2wb*150;

				cycleTotal = cycleL1Send + cycleL1wb*10 + cycleL2Send + cycleL2Recv;

				cerr << "  p" << i << " (" << cycleL1Send << "," << cycleL1Recv << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
				cout << "  p" << i << " (" << cycleL1Send << "," << cycleL1Recv << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
#endif
			}
			commWith >>= 1;
			if (cycleMax<cycleTotal)
				cycleMax = cycleTotal;
		}
		cerr << "  MP Cycles: " << cycleMax << "\n";
		cout << "  MP Cycles: " << cycleMax << "\n";

		commWith = MP_access->getCommWith();
		for (i=0; i<16; i++) {
			if (commWith & 1) {
				hw->getMPIControl()->clear_busy(i);
				hw->getMPIControl()->set_end(i, cycleMax);
			}
			commWith >>= 1;
		}

		break;
	case kd_MPI_Alltoall:
		commWith = MP_access->getCommWith();
		involvedCores = 1;
		for (i=0; i<16; i++) {
			if (pid != i) {
				if (commWith & 1) {
					if (hw->getMPIControl()->getReqAddr(i)==NULL)
						return;
					if (MP_access->getMpiKind() != ((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getMpiKind()) {
						unsigned long long temp = 1;
						assert (((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getCommWith() & (temp <<= pid));
						return;
					}
					involvedCores++;
				}
			}
			commWith >>= 1;
		}

		cerr << ">>>>>>>>>> MPI Alltoall (" << involvedCores << "," << involvedCores*MP_access->getBytes() << ")<<<<<<<<<<\n";
		cout << ">>>>>>>>>> MPI Alltoall (" << involvedCores << "," << involvedCores*MP_access->getBytes() << ")<<<<<<<<<<\n";
		commWith = MP_access->getCommWith();
		cycleMax = 0;
		for (i=0; i<16; i++) {
			cycleTotal = 0;
			if (commWith & 1) {
#ifndef CTOC
				cycleL1wb = cycleL2wb = 0;
				cycleL1Recv = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), involvedCores*MP_access->getBytes(), false, &cycleL1wb);
				assert(!cycleL1wb);
				cycleL1Send = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), involvedCores*MP_access->getBytes(), true, &cycleL1wb);
				cycleL2Recv = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), involvedCores*MP_access->getBytes(), false, &cycleL2wb);
				assert(!cycleL2wb);
				cycleL2Send = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), involvedCores*MP_access->getBytes(), true, &cycleL2wb);
				cycleTotal = cycleL1Send + cycleL1wb*10 + cycleL2Send;
				cycleTotal += cycleL2Recv;
				cycleTotal += cycleL2wb*150;
				cycleTotal += (2*150)*((((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBytes())*involvedCores/64*(involvedCores-1)/involvedCores);
				cerr << "  p" << i << " (" << cycleL1Send << "," << cycleL1Recv << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
				cout << "  p" << i << " (" << cycleL1Send << "," << cycleL1Recv << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
#endif
#ifdef CTOC
				cycleL1wb = cycleL2wb = 0;
				cycleL1Recv = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), involvedCores*MP_access->getBytes(), false, &cycleL1wb);
				cycleL2Recv = hw->getL2Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), involvedCores*MP_access->getBytes(), false, &cycleL2wb);
				assert(!cycleL1wb && !cycleL2wb);
				cycleL1Send = hw->getL1Cache(i)->do_flush(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), involvedCores*MP_access->getBytes(), true, &cycleL1wb);
				cycleL2Send = hw->getL2Cache(i)->do_send(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff1(), involvedCores*MP_access->getBytes(), &cycleL2wb);
				cycleL2Send += cycleL2wb*150;
				cycleL2wb = 0;
				cycleL2Recv += hw->getL2Cache(i)->do_recv(((ReqMPAccess *) hw->getMPIControl()->getReqAddr(i))->getBuff2(), involvedCores*MP_access->getBytes(), &cycleL2wb);
				cycleL2Recv += cycleL2wb*150;

				cycleTotal = cycleL1Send + cycleL1wb*10 + cycleL2Send + cycleL2Recv;

				cerr << "  p" << i << " (" << cycleL1Send << "," << cycleL1Recv << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
				cout << "  p" << i << " (" << cycleL1Send << "," << cycleL1Recv << "," << cycleL1wb << "," << cycleL2Send << "," << cycleL2Recv << "," << cycleL2wb << "," << cycleTotal << ")\n";
#endif
			}
			commWith >>= 1;
			if (cycleMax<cycleTotal)
				cycleMax = cycleTotal;
		}
		cerr << "  MP Cycles: " << cycleMax << "\n";
		cout << "  MP Cycles: " << cycleMax << "\n";

		commWith = MP_access->getCommWith();
		for (i=0; i<16; i++) {
			if (commWith & 1) {
				hw->getMPIControl()->clear_busy(i);
				hw->getMPIControl()->set_end(i, cycleMax);
			}
			commWith >>= 1;
		}

		break;
	default:
		assert(0);
	}

	/*if (hw->getMemory(pid)->getBusyCycle((void *) &mem_read))
		return;

	hw->getMemory(pid)->drop_request((void *) &mem_read);
	mem_read = false;

	hw->getL2Cache(pid)->clear_P1busy();
	hw->getL1Cache(pid)->clear_P1busy();*/

	if (L2_access == NULL)
		return;
	if (hw->getL2Cache(pid)->isP1Busy() && hw->getL2Cache(pid)->getP1ReqAddr() != (void *) L2_access)
		return;

	if (!hw->getL2Cache(pid)->isP1Busy())
		hw->getL2Cache(pid)->set_P1busy((void *) L2_access);
	if (hw->getL2Cache(pid)->getP1BusyCycle())
		return;

	assert(addr == L2_access->getAddr());
	ReqNextCacheAccess *mem_access = NULL;
	ReqNextCacheAccess *mem_access_WB = NULL;
	hw->getL2Cache(pid)->do_access(L2_access->getAddr(), L2_access->getOp(), &mem_access_WB, &mem_access);
	delete L2_access;
	L2_access = NULL;
}

Sim_Memory_Raw_Request *Trace_Manager::readTrace(unsigned long argCore) {
	char line[100];
	char rw;
	char *temp;
	char *addr, *addr2;
	char *commWith, *tag, *size;
	char *id_proc;
	unsigned long pid;
	unsigned long hex_addr;
	unsigned long byte;

	assert(argCore < numCore);

	if (fgets(line, sizeof(line), listFilePointer[argCore]) == NULL)
		return NULL;

	if (!strncmp(line, "PHASE", 5)) {
		id_proc = strtok(line, " ");
		size = strtok(NULL, " \n");
		byte = atoi(size);
#ifdef DEBUG_FILE_READ
		cout << "PAHSE " << byte << "\n";
#endif
		return new Sim_Memory_Raw_Request(argCore, 0, 'p', byte);
	} else if (!strncmp(line, "MPI_", 4)) {
		Sim_Memory_Raw_Request *raw_req = new Sim_Memory_Raw_Request(argCore, 0, 'm', 0);

#ifdef DEBUG_FILE_READ
		cout << "LINE " << line << " =>  ";
#endif
		id_proc = strtok(line, " ");
		if (!strncmp(line, "MPI_SEND", 8)) {
			id_proc = strtok(NULL, " ");
			commWith = strtok(NULL, " ");
			tag = strtok(NULL, " ");
			size = strtok(NULL, " ");
			addr = strtok(NULL, " ");
			raw_req->setMpiArgs(kd_MPI_Send, argCore, atol(commWith), atol(tag), 0, 0, atol(size), string2hex(addr), 0);
		}
		else if (!strncmp(line, "MPI_RECV", 8)) {
			id_proc = strtok(NULL, " ");
			commWith = strtok(NULL, " ");
			tag = strtok(NULL, " ");
			size = strtok(NULL, " ");
			addr = strtok(NULL, " ");
			raw_req->setMpiArgs(kd_MPI_Recv, argCore, atol(commWith), atol(tag), 0, 0, atol(size), string2hex(addr), 0);
		}
		else if (!strncmp(line, "MPI_BARRIER", 11)) {
			id_proc = strtok(NULL, " ");
			commWith = strtok(NULL, " ");
			size = strtok(NULL, " ");
			raw_req->setMpiArgs(kd_MPI_Barrier, argCore, 0, 0, string2hex(commWith), 0, 0, 0, 0);
		}
		else if (!strncmp(line, "MPI_BCAST", 9)) {
			id_proc = strtok(NULL, " ");
			commWith = strtok(NULL, " ");
			tag = strtok(NULL, " ");
			size = strtok(NULL, " ");
			addr = strtok(NULL, " ");
			raw_req->setMpiArgs(kd_MPI_Bcast, argCore, 0, 0, string2hex(commWith), atol(tag), atol(size), string2hex(addr), 0);
		}
		else if (!strncmp(line, "MPI_REDUCE", 10)) {
			id_proc = strtok(NULL, " ");
			commWith = strtok(NULL, " ");
			tag = strtok(NULL, " ");
			size = strtok(NULL, " ");
			addr = strtok(NULL, " ");
			addr2 = strtok(NULL, " ");
			raw_req->setMpiArgs(kd_MPI_Reduce, argCore, 0, 0, string2hex(commWith), atol(tag), atol(size), string2hex(addr), string2hex(addr2));
		}
		else if (!strncmp(line, "MPI_ALLREDUCE", 13)) {
			id_proc = strtok(NULL, " ");
			commWith = strtok(NULL, " ");
			size = strtok(NULL, " ");
			addr = strtok(NULL, " ");
			addr2 = strtok(NULL, " ");
			raw_req->setMpiArgs(kd_MPI_Allreduce, argCore, 0, 0, string2hex(commWith), 0, atol(size), string2hex(addr), string2hex(addr2));
		}
		else if (!strncmp(line, "MPI_ALLGATHER", 13)) {
			id_proc = strtok(NULL, " ");
			commWith = strtok(NULL, " ");
			size = strtok(NULL, " ");
			addr = strtok(NULL, " ");
			addr2 = strtok(NULL, " ");
			raw_req->setMpiArgs(kd_MPI_Allgather, argCore, 0, 0, string2hex(commWith), 0, atol(size), string2hex(addr), string2hex(addr2));
		}
		else if (!strncmp(line, "MPI_ALLTOALL", 12)) {
			id_proc = strtok(NULL, " ");
			commWith = strtok(NULL, " ");
			size = strtok(NULL, " ");
			addr = strtok(NULL, " ");
			addr2 = strtok(NULL, " ");
			raw_req->setMpiArgs(kd_MPI_Alltoall, argCore, 0, 0, string2hex(commWith), 0, atol(size), string2hex(addr), string2hex(addr2));
		}
		else {
			assert(0);
		}
		return raw_req;
	} else {
#ifdef DEBUG_FILE_READ
		cout << "LINE " << line << " =>  ";
#endif
		id_proc = strtok(line, " ");
		pid = atoi(id_proc);
		temp = strtok(NULL, " ");
		rw = *temp;
		addr = strtok(NULL, " ");
		size = strtok(NULL, " \n");
		byte = atoi(size);
		hex_addr = string2hex(addr);
#ifdef DEBUG_FILE_READ
		cout << pid << " " << rw << " "<< hex << hex_addr << " " << dec << byte << "\n";
#endif
		return new Sim_Memory_Raw_Request(argCore, hex_addr, rw, byte);
	}
}

void Sim_16core_Shared::doSim(string argFileName) {
	unsigned long i, target_cycle, trace_buffer;
	unsigned long simulated_accesses[16];
	unsigned long simulated_phase[16];
	unsigned long current_phase;
	bool flag = false;
	string interestLine, interestLine_prev;
	Sim_Memory_Raw_Request *raw_req;
	Sim_16core_Shared_Request *req;

	trace->setFileName(argFileName);
	trace->openFile();

	current_phase = 0;
	for (i = 0; i < 16; i++)
		simulated_phase[i] = 0;

	while (1) {
		for (i = 0; i < 16; i++) {
			trace_buffer = TRACE_BUFFER;
			while (trace_buffer && current_phase >= simulated_phase[i]) {
				raw_req = trace->readTrace(i);
				if (raw_req == NULL)
					break;

				if (raw_req->getOp() == 'p') {
					simulated_phase[i] = raw_req->getSize();
					assert(raw_req->getPid() == i);
				} else {
					req = new Sim_16core_Shared_Request(hw, raw_req->getPid(), raw_req->getAddr(), raw_req->getOp());
					assert(req->getOp() == 'r' || req->getOp() == 'w');
					assert(raw_req->getPid() == i);
					req_queue[i].push(req);
					trace_buffer--;
				}

				delete raw_req;
			}
		}

		/*req = new Sim_16core_Shared_Request(hw, 0, 0x12345045, 'r');
		 req_queue[0].push(req);
		 req = new Sim_16core_Shared_Request(hw, 1, 0x12345040, 'r');
		 req_queue[1].push(req);
		 req = new Sim_16core_Shared_Request(hw, 2, 0x12345040, 'r');
		 req_queue[2].push(req);
		 req = new Sim_16core_Shared_Request(hw, 3, 0x12345042, 'r');
		 req_queue[3].push(req);
		 req = new Sim_16core_Shared_Request(hw, 4, 0x12345040, 'r');
		 req_queue[4].push(req);
		 req = new Sim_16core_Shared_Request(hw, 5, 0x12345040, 'r');
		 req_queue[5].push(req);
		 req = new Sim_16core_Shared_Request(hw, 1, 0x16345040, 'r');
		 req_queue[1].push(req);
		 req = new Sim_16core_Shared_Request(hw, 1, 0x17345040, 'r');
		 req_queue[1].push(req);
		 req = new Sim_16core_Shared_Request(hw, 1, 0x12345040, 'w');
		 req_queue[1].push(req);*/

		for (i = 0; i < 16; i++) {
			simulated_accesses[i] = 0;
		}
		i = 0;
		target_cycle = 0;
		while (i < 16) {
			if (!(get_cycle() % 100000)) {
				cerr << "Current Cycles: " << get_cycle() << "\n";
				for (i = 0; i < 16; i++)
					cerr << simulated_accesses[i] << " ";
				cerr << "\n";
			}
#ifdef DEBUG
			if (get_cycle()>42400000) {
				cout << "========== Current Cycle: " << global_cycle << " ==========\n";
				printStatus_MemReq();
				hw->printStatus_L1s();
				hw->printStatus_L2s();
				hw->printStatus_Dirs();
				hw->printStatus_Inflight();
				if (target_cycle<get_cycle()) {
					string str;
					cin >> str;
					target_cycle = (unsigned long) atol(str.c_str());
				}
			}
#endif
#ifdef DEBUG_DETAIL
			/*if (get_cycle()>152050) {
				cout << "========== Current Cycle: " << global_cycle << " ==========\n";
				if (target_cycle<get_cycle()) {
					string str;
					cin >> str;
					target_cycle = (unsigned long) atol(str.c_str());
				}
			}
			if (get_cycle()>60000000) {
				interestLine_prev = interestLine;
				interestLine = "L2 ";
				for (i=0; i<16; i++) {
					interestLine += hw->getL2Cache(i)->printLine(6298176);
					interestLine += " ";
				}
				interestLine += "DIR " + hw->getDirectoryManager()->printLine(6298176);
				if (interestLine!=interestLine_prev)
				cout << setw(10) << get_cycle() << ":" << interestLine << "\n";
			}*/
#endif
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_mem_read();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_L1_inv();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_L2_inv_WB();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_L2_inv();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_L2_read();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_L1_inv_self();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_dir_request_WB();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_dir_request();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_L2_access();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_L1_access();
			hw->simLatency();
			increase_cycle();

			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					if (req_queue[i].front()->isEnd()) {
						Sim_16core_Shared_Request *req;
						req = req_queue[i].front();
						delete req;
						req_queue[i].pop();
						simulated_accesses[i]++;

						if (current_phase >= simulated_phase[i]) {
							raw_req = trace->readTrace(i);
							if (raw_req != NULL) {
								if (raw_req->getOp() == 'p') {
									simulated_phase[i] = raw_req->getSize();
									assert(raw_req->getPid() == i);
								} else {
									req = new Sim_16core_Shared_Request(hw, raw_req->getPid(), raw_req->getAddr(), raw_req->getOp());
									assert(req->getOp() == 'r' || req->getOp() == 'w');
									assert(raw_req->getPid() == i);
									req_queue[i].push(req);
								}
								delete raw_req;
							}
						}
					}
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					break;
		}
		cerr << ">>>>>>>>>> Phase " << current_phase << " End <<<<<<<<<<\n";
		cerr << "  Current Cycles: " << get_cycle() << "\n";
		cout << ">>>>>>>>>> Phase " << current_phase << " End <<<<<<<<<<\n";
		cout << "  Current Cycles: " << get_cycle() << "\n";
		cout << "  Current Simulated: ";
		for (i = 0; i < 16; i++)
			cout << simulated_accesses[i] << " ";
		cout << "\n";

		for (i = 0; i < 16; i++)
			if (simulated_accesses[i])
				break;
		if (i < 16) {
			current_phase++;
			flag = false;
		} else {
			if (flag)
				break;
			else {
				current_phase++;
				flag = true;
			}
		}
	}

	cout << "Total Cycles: " << get_cycle() << "\n";
	hw->printStat();
}

void Sim_16core_Shared::printStatus_MemReq() {
	for (unsigned long i = 0; i < 16; i++)
		if (hw->getPrintStatus()[i]) {
			if (!req_queue[i].empty()) {
				cout << setw(2) << req_queue[i].front()->getPid() << req_queue[i].front()->getOp() << " 0x" << hex << setw(8) << req_queue[i].front()->getAddr() << dec << ":";
				if (req_queue[i].front()->getL1_access())
					cout << "L1";
				else
					cout << "  ";
				if (req_queue[i].front()->getL2_access())
					cout << "L2";
				else
					cout << "  ";
				if (req_queue[i].front()->getDir_access_WB())
					cout << setw(2) << hw->getDirectoryManager()->getId(req_queue[i].front()->getDir_access_WB()->getAddr()) << "W";
				else
					cout << "   ";
				if (req_queue[i].front()->getDir_aceess())
					cout << setw(2) << hw->getDirectoryManager()->getId(req_queue[i].front()->getDir_aceess()->getAddr()) << "D";
				else
					cout << "   ";
				if (req_queue[i].front()->getMem_read())
					cout << "M";
				else
					cout << " ";
				if (req_queue[i].front()->getL2_read())
					cout << "Rx" << hex << setw(2) << req_queue[i].front()->getL2_read()->getInvolveCores() << dec;
				else
					cout << "    ";
				if (req_queue[i].front()->getL2_inv())
					cout << "Ix" << hex << setw(2) << req_queue[i].front()->getL2_inv()->getInvolveCores() << dec;
				else
					cout << "    ";
				if (req_queue[i].front()->getL2_inv_WB())
					cout << "IWx" << hex << setw(2) << req_queue[i].front()->getL2_inv_WB()->getInvolveCores() << dec;
				else
					cout << "     ";
			} else
				cout << "                                       ";
		}
	cout << "\n";
}

void Sim_16core_Message::printStatus_MemReq() {
	for (unsigned long i = 0; i < 16; i++)
		if (hw->getPrintStatus()[i]) {
			if (!req_queue[i].empty()) {
				if (req_queue[i].front()->getOp()=='m') {
					cout << setw(2) << req_queue[i].front()->getPid() << req_queue[i].front()->getOp() << dec << ":";
					cout << "                           ";
				}
				else {
					cout << setw(2) << req_queue[i].front()->getPid() << req_queue[i].front()->getOp() << " 0x" << hex << setw(8) << req_queue[i].front()->getAddr() << dec << ":";
					if (req_queue[i].front()->getL1_access())
						cout << "L1";
					else
						cout << "  ";
					if (req_queue[i].front()->getL2_access())
						cout << "L2";
					else
						cout << "  ";
					if (req_queue[i].front()->getMem_read())
						cout << "M";
					else
						cout << " ";
					cout << "                   ";
				}
			} else
				cout << "                                       ";
		}
	cout << "\n";
}

void Sim_16core_Message_HW::simLatency() {
	mpi_control->simLatency();
	for (unsigned long i = 0; i < 16; i++) {
		L1_cache[i]->simLatency();
		L2_cache[i]->simLatency();
		mem[i]->simLatency();
	}
}

void Sim_16core_Message::doSim(string argFileName) {
	unsigned long i, target_cycle, trace_buffer;
	unsigned long simulated_accesses[16];
	unsigned long simulated_phase[16];
	unsigned long current_phase;
	bool flag = false;
	Sim_Memory_Raw_Request *raw_req;
	Sim_16core_Message_Request *req;

	trace->setFileName(argFileName);
	trace->openFile();

	current_phase = 0;
	for (i = 0; i < 16; i++)
		simulated_phase[i] = 0;

	while (1) {
		for (i = 0; i < 16; i++) {
			trace_buffer = TRACE_BUFFER;
			while (trace_buffer && current_phase >= simulated_phase[i]) {
				raw_req = trace->readTrace(i);
				if (raw_req == NULL)
					break;

				if (raw_req->getOp() == 'p') {
					simulated_phase[i] = raw_req->getSize();
					assert(raw_req->getPid() == i);
				} else if (raw_req->getOp() == 'm') {
					assert(raw_req->getPid() == i && raw_req->getRank() == i);
					req = new Sim_16core_Message_Request(hw, i, 0, 'm', raw_req->getMpiKind(), raw_req->getSrcDest(), raw_req->getTag(), raw_req->getCommWith(), raw_req->getRoot(), raw_req->getBytes(), raw_req->getBuff1(), raw_req->getBuff2());
					req_queue[i].push(req);
					trace_buffer--;
					/*switch (raw_req->getMpiKind()) {
					case kd_MPI_Send:
						break;
					default:
						assert(0);
						break;
					}*/
				} else {
					req = new Sim_16core_Message_Request(hw, raw_req->getPid(), raw_req->getAddr(), raw_req->getOp());
					assert(req->getOp() == 'r' || req->getOp() == 'w');
					assert(raw_req->getPid() == i);
					req_queue[i].push(req);
					trace_buffer--;
				}

				delete raw_req;
			}
		}

		for (i = 0; i < 16; i++) {
			simulated_accesses[i] = 0;
		}
		i = 0;
		target_cycle = 0;
		while (i < 16) {
			if (!(get_cycle() % 100000)) {
				cerr << "Current Cycles: " << get_cycle() << "\n";
				for (i = 0; i < 16; i++)
					cerr << simulated_accesses[i] << " ";
				cerr << "\n";
			}
#ifdef DEBUG
			cout << "========== Current Cycle: " << global_cycle << " ==========\n";
			printStatus_MemReq();
			hw->printStatus_L1s();
			hw->printStatus_L2s();
			if (target_cycle<get_cycle()) {
				string str;
				cin >> str;
				target_cycle = (unsigned long) atol(str.c_str());
			}
#endif
#ifdef DEBUG_DETAIL
			/*if (get_cycle()>152050) {
				cout << "========== Current Cycle: " << global_cycle << " ==========\n";
				if (target_cycle<get_cycle()) {
					string str;
					cin >> str;
					target_cycle = (unsigned long) atol(str.c_str());
				}
			}*/
			if (get_cycle()>16000000) {
				cout << "========== Current Cycle: " << global_cycle << " ==========\n";
				printStatus_MemReq();
				if (target_cycle<get_cycle()) {
					string str;
					cin >> str;
					target_cycle = (unsigned long) atol(str.c_str());
				}
			}
			/*if (get_cycle()>16000000) {
				interestLine_prev = interestLine;
				interestLine = "L2 ";
				for (i=0; i<16; i++) {
					interestLine += hw->getL2Cache(i)->printLine(6298176);
					interestLine += " ";
				}
				interestLine += "DIR " + hw->getDirectoryManager()->printLine(6298176);
				if (interestLine!=interestLine_prev)
				cout << setw(10) << get_cycle() << ":" << interestLine << "\n";
			}*/
#endif
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_mem_read();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_L2_access_WB();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_L2_access();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_L1_access();
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					req_queue[i].front()->do_MP_access();

			hw->simLatency();
			increase_cycle();

			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					if (req_queue[i].front()->isEnd()) {
						Sim_16core_Message_Request *req;
						req = req_queue[i].front();
						delete req;
						req_queue[i].pop();
						simulated_accesses[i]++;

						if (current_phase >= simulated_phase[i]) {
							raw_req = trace->readTrace(i);
							if (raw_req != NULL) {
								if (raw_req->getOp() == 'p') {
									simulated_phase[i] = raw_req->getSize();
									assert(raw_req->getPid() == i);
								} else if (raw_req->getOp() == 'm') {
									assert(raw_req->getPid() == i && raw_req->getRank() == i);
									req = new Sim_16core_Message_Request(hw, i, 0, 'm', raw_req->getMpiKind(), raw_req->getSrcDest(), raw_req->getTag(), raw_req->getCommWith(), raw_req->getRoot(), raw_req->getBytes(), raw_req->getBuff1(), raw_req->getBuff2());
									req_queue[i].push(req);
									trace_buffer--;
									/*switch (raw_req->getMpiKind()) {
									case kd_MPI_Send:
										break;
									default:
										assert(0);
										break;
									}*/
								} else {
									req = new Sim_16core_Message_Request(hw, raw_req->getPid(), raw_req->getAddr(), raw_req->getOp());
									assert(req->getOp() == 'r' || req->getOp() == 'w');
									assert(raw_req->getPid() == i);
									req_queue[i].push(req);
									trace_buffer--;
								}

								delete raw_req;
							}
						}
					}
			for (i = 0; i < 16; i++)
				if (!req_queue[i].empty())
					break;
		}
		cerr << ">>>>>>>>>> Phase " << current_phase << " End <<<<<<<<<<\n";
		cerr << "  Current Cycles: " << get_cycle() << "\n";
		cout << ">>>>>>>>>> Phase " << current_phase << " End <<<<<<<<<<\n";
		cout << "  Current Cycles: " << get_cycle() << "\n";
		cout << "  Current Simulated: ";
		for (i = 0; i < 16; i++)
			cout << simulated_accesses[i] << " ";
		cout << "\n";

		for (i = 0; i < 16; i++)
			if (simulated_accesses[i])
				break;
		if (i < 16) {
			current_phase++;
			flag = false;
		} else {
			if (flag)
				break;
			else {
				current_phase++;
				flag = true;
			}
		}
	}

	cout << "Total Cycles: " << get_cycle() << "\n";
	hw->printStat();
}
