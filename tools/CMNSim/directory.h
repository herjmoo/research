/*
 * directory.h
 *
 *  Created on: Mar 20, 2014
 *      Author: Joonmoo Huh
 */

#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#include <map>
#include <assert.h>

#include "request.h"
#include "cache.h"

using namespace std;

class Cache_MESI;

class Directory_MESI {
private:
	unsigned long numCore;
	unsigned long id;
	map<unsigned long, pair<unsigned char *, enum coherence> *> dir;

	unsigned long memReads, memWrites;
	unsigned long *requests, *writebacks, *invalidates;
	unsigned long *invalidated, *read, *exclusived;

	void *busy;
	unsigned long busyCycles;
	unsigned long latency;
	map<unsigned long, pair<unsigned char *, enum coherence> *>::iterator printStatusLine;

public:
	Directory_MESI() {
		numCore = 0;
		id = 0;
		memReads = 0;
		memWrites = 0;
		requests = NULL;
		writebacks = NULL;
		invalidates = NULL;
		invalidated = NULL;
		read = NULL;
		exclusived = NULL;
		busy = 0;
		busyCycles = 0;
		latency = 0;
	}
	~Directory_MESI() {
	}

	void set_numCore(unsigned long argNumCore) {
		numCore = argNumCore;
		requests = new unsigned long[numCore];
		writebacks = new unsigned long[numCore];
		invalidates = new unsigned long[numCore];
		invalidated = new unsigned long[numCore];
		read = new unsigned long[numCore];
		exclusived = new unsigned long[numCore];
		for (unsigned long i = 0; i < numCore; i++) {
			requests[i] = 0;
			writebacks[i] = 0;
			invalidates[i] = 0;
			invalidated[i] = 0;
			read[i] = 0;
			exclusived[i] = 0;
		}
	}
	void set_id(unsigned long argId) {
		id = argId;
	}
	void set_latency(unsigned long arglatency) {
		latency = arglatency;
	}

	void set_busy(void *argReqAddr) {
		busy = argReqAddr;
		busyCycles = latency;
	}
	void clear_busy() {
		busy = 0;
	}
	void simLatency() {
		if (busyCycles)
			busyCycles--;
	}

	void do_request(unsigned long cache_id, unsigned long addr, unsigned char op, Cache_MESI **L2s, bool *memRead, ReqPrevCacheReadManager **reqRead, ReqPrevCacheInvalidateManager **reqInv);
	void do_request_WB(unsigned long cache_id, unsigned long addr, unsigned char op, ReqPrevCacheInvalidateManager **reqInv);
	enum coherence refer_state(unsigned long addr);

	unsigned long getId() {
		return id;
	}
	unsigned long getBusyCycle() {
		return busyCycles;
	}
	void *getReqAddr() {
		return busy;
	}
	bool isBusy() {
		return (busy != 0);
	}

	void incInvalidated(unsigned long argId) {
		invalidated[argId]++;
	}
	void incRead(unsigned long argId) {
		read[argId]++;
	}
	void incExclusived(unsigned long argId) {
		exclusived[argId]++;
	}
	void printStatus_reset() {
		printStatusLine = dir.begin();
	}
	bool printStatus_nextLine();
	string printLine(unsigned long addr);
	void printStat();
	unsigned long printStatMemReads() {
		return memReads;
	}
	unsigned long printStatMemWrites() {
		return memWrites;
	}
};

class Directory_Manager_Interleave {
private:
	unsigned long numDir;
	unsigned long blockSize;
	unsigned long log2blockSize;
	unsigned long blkMask;
	unsigned long idxMask;
	Directory_MESI *dir;

public:
	Directory_Manager_Interleave(unsigned long argNumCore,
			unsigned long argNumDir, unsigned long b,
			unsigned long argLatency) {
		numDir = argNumDir;
		blockSize = b;
		log2blockSize = log2(blockSize);
		blkMask = 0;
		for (unsigned long i = 0; i < log2blockSize; i++) {
			blkMask <<= 1;
			blkMask |= 1;
		}
		idxMask = 0;
		for (unsigned long i = 0; i < log2(numDir); i++) {
			idxMask <<= 1;
			idxMask |= 1;
		}
		dir = new Directory_MESI[numDir];
		for (unsigned long i = 0; i < numDir; i++) {
			dir[i].set_numCore(argNumCore);
			dir[i].set_latency(argLatency);
			dir[i].set_id(i);
		}
	}
	~Directory_Manager_Interleave() {
	}

	void simLatency();

	void set_busy(unsigned long addr, void *argReqAddr);
	void clear_busy(unsigned long addr);

	void do_request(unsigned long cache_id, unsigned long addr, unsigned char op, Cache_MESI **L2s, bool *memRead,ReqPrevCacheReadManager **reqRead, ReqPrevCacheInvalidateManager **reqInv);
	void do_request_WB(unsigned long cache_id, unsigned long addr,
			unsigned char op, ReqPrevCacheInvalidateManager **reqInv);
	enum coherence refer_state(unsigned long addr);
	unsigned long getLog2blockSize() {
		return log2blockSize;
	}
	Directory_MESI *getDirectoryIdx(unsigned long idx) {
		return &dir[idx];
	}

	unsigned long getId(unsigned long addr);
	unsigned long getBusyCycle(unsigned long addr);
	void *getReqAddr(unsigned long addr);
	bool isBusy(unsigned long addr);
	string printLine(unsigned long addr);
};

class Memory {
private:
	map<void *, unsigned long> busyCycles;
	unsigned long latency;

public:
	Memory(unsigned long argLatency) {
		latency = argLatency;
	}
	~Memory() {
	}

	void add_request(void *key) {
		assert(busyCycles.find(key) == busyCycles.end());
		busyCycles[key] = latency;
	}
	void drop_request(void *key) {
		busyCycles.erase(key);
	}
	void simLatency() {
		for (map<void *, unsigned long>::iterator I = busyCycles.begin(), E = busyCycles.end(); I != E; I++) {
			if (I->second)
				I->second--;
		}
	}
	unsigned long getBusyCycle(void *key) {
		if (busyCycles.find(key) == busyCycles.end())
			return 0;
		else
			return busyCycles[key];
	}
};

#endif /* DIRECTORY_H_ */
