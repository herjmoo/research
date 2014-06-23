/*
 * cache.h
 *
 *  Created on: Mar 18, 2014
 *      Author: Joonmoo Huh
 */

#ifndef CACHE_H_
#define CACHE_H_

#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <assert.h>

#include "request.h"
#include "directory.h"

using namespace std;

class Directory_Manager_Interleave;

class LRU {
private:
	unsigned long **seq;
	unsigned long sets, assoc;

public:
	LRU(unsigned long argSets, unsigned long argAssoc) {
		sets = argSets;
		assoc = argAssoc;
		seq = new unsigned long *[sets];
		for (unsigned long i = 0; i < sets; i++) {
			seq[i] = new unsigned long[assoc];
			for (unsigned long j = 0; j < assoc; j++)
				seq[i][j] = j;
		}
	}
	~LRU() {
		delete seq;
	}

	unsigned long getLRU(unsigned long argIdx);
	void updateMRU(unsigned long argIdx, unsigned long argPos);
	void updateLRU(unsigned long argIdx, unsigned long argPos);
};

class CacheBlock {
private:
	unsigned long tag;
	enum coherence state;

public:
	CacheBlock() {
		tag = 0;
		state = INVALID;
	}
	~CacheBlock() {
	}

	void setTag(unsigned long argTag) {
		tag = argTag;
	}
	void setCoherence(enum coherence argState) {
		state = argState;
	}

	unsigned long getTag() {
		return tag;
	}
	enum coherence getCoherence() {
		return state;
	}
	bool isValid() {
		return (state != INVALID);
	}

	void invalidate() {
		tag = 0;
		state = INVALID;
	}
};

class Cache {
protected:
	CacheBlock **cache;
	unsigned long *accessesSets;
	unsigned long id;

	unsigned long size, blockSize, assoc;
	unsigned long sets, blocks, log2sets, log2blockSize, tagMask;

	unsigned long reads, readMisses, writes, writeMisses, writeBacks, invalidates;
	unsigned long invalidated, read, exclusived;

	unsigned long calcTag(unsigned long addr) {
		return (addr >> (log2blockSize + log2sets));
	}
	unsigned long calcIndex(unsigned long addr) {
		return ((addr >> log2blockSize) & tagMask);
	}
	unsigned long findBlock(unsigned long argTag, unsigned long argIndex);

public:
	Cache(unsigned long argId, unsigned long s, unsigned long b, unsigned long a) {
		id = argId;
		size = s, blockSize = b, assoc = a;
		sets = (s / b) / a;
		blocks = s / b;
		log2sets = log2(sets);
		log2blockSize = log2(blockSize);
		tagMask = 0;
		for (unsigned long i = 0; i < log2sets; i++) {
			tagMask <<= 1;
			tagMask |= 1;
		}
		reads = readMisses = writes = writeMisses = writeBacks = invalidates = 0;
		invalidated = read = exclusived = 0;

		cache = new CacheBlock*[sets];
		accessesSets = new unsigned long [sets];
		for (unsigned long i = 0; i < sets; i++) {
			cache[i] = new CacheBlock[assoc];
			accessesSets[i] = 0;
		}
	}
	~Cache() {
		delete cache;
		delete accessesSets;
	}

	unsigned long calcAddr(unsigned long tag, unsigned long idx) {
		return (((tag << log2sets) | idx) << log2blockSize);
	}
	unsigned long countNumDirtyBlocks() {
		unsigned long cnt = 0;
		for (unsigned long i = 0; i < sets; i++) {
			for (unsigned long j = 0; j < assoc; j++) {
				if (cache[i][j].getCoherence()==MODIFIED) cnt++;
			}
		}
		return cnt;
	}
};

class Cache_WT: Cache {
private:
	LRU *lru;
	void *p1_busy;
	unsigned long p1_busyCycles;
	unsigned long p1_latency;
	void *p2_busy;
	unsigned long p2_busyCycles;
	unsigned long p2_latency;
	unsigned long printStatusLine;

public:
	Cache_WT(unsigned long argId, unsigned long s, unsigned long b,	unsigned long a, unsigned long argLatency) :
			Cache(argId, s, b, a) {
		lru = new LRU(sets, assoc);
		p1_busy = 0;
		p1_busyCycles = 0;
		p1_latency = argLatency;
		p2_busy = 0;
		p2_busyCycles = 0;
		p2_latency = argLatency;

		printStatusLine = 0;
	}
	~Cache_WT() {
	}

	void set_P1busy(void *argReqAddr) {
		p1_busy = argReqAddr;
		p1_busyCycles = p1_latency;
	}
	void clear_P1busy() {
		p1_busy = 0;
	}
	void set_P2busy(void *argReqAddr) {
		p2_busy = argReqAddr;
		p2_busyCycles = p2_latency;
	}
	void clear_P2busy() {
		p2_busy = 0;
	}
	void simLatency() {
		if (p1_busyCycles)
			p1_busyCycles--;
		if (p2_busyCycles)
			p2_busyCycles--;
	}

	void do_access(unsigned long addr, unsigned char op, ReqNextCacheAccess **req);
	void do_invalidate(unsigned long addr, unsigned long tagMask);

	unsigned long getP1BusyCycle() {
		return p1_busyCycles;
	}
	void *getP1ReqAddr() {
		return p1_busy;
	}
	bool isP1Busy() {
		return (p1_busy != 0);
	}
	unsigned long getP2BusyCycle() {
		return p2_busyCycles;
	}
	void *getP2ReqAddr() {
		return p2_busy;
	}
	bool isP2Busy() {
		return (p2_busy != 0);
	}

	void printStatus_reset() {
		printStatusLine = 0;
	}
	bool printStatus_nextLine();
	void printStat();
	unsigned long printStatReads() {
		return reads;
	}
	unsigned long printStatReadMisses() {
		return readMisses;
	}
	unsigned long printStatWrites() {
		return writes;
	}
	unsigned long printStatWriteMisses() {
		return writeMisses;
	}
	unsigned long printStatWriteBacks() {
		return writeBacks;
	}
	unsigned long printStatInvalidates() {
		return invalidates;
	}
	unsigned long printStatInvalidated() {
		return invalidated;
	}
	unsigned long printStatRead() {
		return read;
	}
	unsigned long printStatExclusived() {
		return exclusived;
	}
};

class Cache_WB_SCL: Cache {
private:
	LRU *lru;
	void *p1_busy;
	unsigned long p1_busyCycles;
	unsigned long p1_latency;
	void *p2_busy;
	unsigned long p2_busyCycles;
	unsigned long p2_latency;
	unsigned long printStatusLine;

	unsigned long mp_flushed, mp_writebacks, mp_fromCache, mp_fromMem, mp_toCache, mp_toMem;

public:
	Cache_WB_SCL(unsigned long argId, unsigned long s, unsigned long b, unsigned long a, unsigned long argLatency) :
			Cache(argId, s, b, a) {
		lru = new LRU(sets, assoc);
		p1_busy = 0;
		p1_busyCycles = 0;
		p1_latency = argLatency;
		p2_busy = 0;
		p2_busyCycles = 0;
		p2_latency = argLatency;

		printStatusLine = 0;

		mp_flushed = mp_writebacks = mp_fromCache = mp_fromMem = mp_toCache = mp_toMem = 0;
	}
	~Cache_WB_SCL() {
	}

	void set_P1busy(void *argReqAddr) {
		p1_busy = argReqAddr;
		p1_busyCycles = p1_latency;
	}
	void clear_P1busy() {
		p1_busy = 0;
	}
	void set_P2busy(void *argReqAddr) {
		p2_busy = argReqAddr;
		p2_busyCycles = p2_latency;
	}
	void clear_P2busy() {
		p2_busy = 0;
	}
	void simLatency() {
		if (p1_busyCycles)
			p1_busyCycles--;
		if (p2_busyCycles)
			p2_busyCycles--;
	}

	void do_access(unsigned long addr, unsigned char op, ReqNextCacheAccess **reqWB, ReqNextCacheAccess **req);
	unsigned long do_flush(unsigned long addr, unsigned long size, bool isSend, unsigned long *wb);
	unsigned long do_send(unsigned long addr, unsigned long size, unsigned long *fromMem);
	unsigned long do_recv(unsigned long addr, unsigned long size, unsigned long *toMem);

	unsigned long getP1BusyCycle() {
		return p1_busyCycles;
	}
	void *getP1ReqAddr() {
		return p1_busy;
	}
	bool isP1Busy() {
		return (p1_busy != 0);
	}
	unsigned long getP2BusyCycle() {
		return p2_busyCycles;
	}
	void *getP2ReqAddr() {
		return p2_busy;
	}
	bool isP2Busy() {
		return (p2_busy != 0);
	}

	void printStatus_reset() {
		printStatusLine = 0;
	}
	bool printStatus_nextLine();
	unsigned long printStatReads() {
		return reads;
	}
	unsigned long printStatReadMisses() {
		return readMisses;
	}
	unsigned long printStatWrites() {
		return writes;
	}
	unsigned long printStatWriteMisses() {
		return writeMisses;
	}
	unsigned long printStatWriteBacks() {
		return writeBacks;
	}
	unsigned long printStatMPflushed() {
		return mp_flushed;
	}
	unsigned long printStatMPwritebacks() {
		return mp_writebacks;
	}
	unsigned long printStatMPfromCache() {
		return mp_fromCache;
	}
	unsigned long printStatMPfromMem() {
		return mp_fromMem;
	}
	unsigned long printStatMPtoCache() {
		return mp_toCache;
	}
	unsigned long printStatMPtoMem() {
		return mp_toMem;
	}
	unsigned long printNumDirtyBlocks() {
		return countNumDirtyBlocks();
	}
};

class Cache_MESI: Cache {
private:
	LRU *lru;
	void *p1_busy;
	unsigned long p1_busyCycles;
	unsigned long p1_latency;
	void *p2_busy;
	unsigned long p2_busyCycles;
	unsigned long p2_latency;
	unsigned long printStatusLine;

public:
	Cache_MESI(unsigned long argId, unsigned long s, unsigned long b, unsigned long a, unsigned long argLatency) :
			Cache(argId, s, b, a) {
		lru = new LRU(sets, assoc);
		p1_busy = 0;
		p1_busyCycles = 0;
		p1_latency = argLatency;
		p2_busy = 0;
		p2_busyCycles = 0;
		p2_latency = argLatency;

		printStatusLine = 0;
	}
	~Cache_MESI() {
	}

	void set_P1busy(void *argReqAddr) {
		p1_busy = argReqAddr;
		p1_busyCycles = p1_latency;
	}
	void clear_P1busy() {
		p1_busy = 0;
	}
	void set_P2busy(void *argReqAddr) {
		p2_busy = argReqAddr;
		p2_busyCycles = p2_latency;
	}
	void clear_P2busy() {
		p2_busy = 0;
	}
	void simLatency() {
		if (p1_busyCycles)
			p1_busyCycles--;
		if (p2_busyCycles)
			p2_busyCycles--;
	}

	void do_access(unsigned long addr, unsigned char op, Directory_Manager_Interleave *dir, ReqDirectoryAccess **req, ReqDirectoryAccess **reqWB, ReqPrevCacheInvalidate **inv);
	unsigned long check_WB(unsigned long addr, unsigned char op);
	void do_read(unsigned long addr);
	void do_invalidate(unsigned long addr, unsigned long tagMask, ReqPrevCacheInvalidate **req);
	void do_exclusive(unsigned long addr);
	enum coherence refer_state(unsigned long addr);

	unsigned long getID() {
		return id;
	}
	unsigned long getP1BusyCycle() {
		return p1_busyCycles;
	}
	void *getP1ReqAddr() {
		return p1_busy;
	}
	bool isP1Busy() {
		return (p1_busy != 0);
	}
	unsigned long getP2BusyCycle() {
		return p2_busyCycles;
	}
	void *getP2ReqAddr() {
		return p2_busy;
	}
	bool isP2Busy() {
		return (p2_busy != 0);
	}

	void printStatus_reset() {
		printStatusLine = 0;
	}
	bool printStatus_nextLine();
	string printLine(unsigned long addr);
	void printStat();
	unsigned long printStatReads() {
		return reads;
	}
	unsigned long printStatReadMisses() {
		return readMisses;
	}
	unsigned long printStatWrites() {
		return writes;
	}
	unsigned long printStatWriteMisses() {
		return writeMisses;
	}
	unsigned long printStatWriteBacks() {
		return writeBacks;
	}
	unsigned long printStatInvalidates() {
		return invalidates;
	}
	unsigned long printStatInvalidated() {
		return invalidated;
	}
	unsigned long printStatRead() {
		return read;
	}
	unsigned long printStatExclusived() {
		return exclusived;
	}
	unsigned long printNumDirtyBlocks() {
		return countNumDirtyBlocks();
	}
	void printStatAccessesSets() {
		for (unsigned long i = 0; i < sets; i++) {
			cout << i << " " << accessesSets[i] << "\n";
		}
	}
};

#endif /* CACHE_H_ */
