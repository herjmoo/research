/*
 * request.h
 *
 *  Created on: Mar 21, 2014
 *      Author: joonmoo
 */

#ifndef REQUEST_H_
#define REQUEST_H_

#include <list>
#include "network.h"

using namespace std;

enum coherence {
	INVALID = 0, VALID, SHARED, MODIFIED, EXCLUSIVE,
};

class ReqNextCacheAccess {
private:
	unsigned long addr;
	unsigned char op;

public:
	ReqNextCacheAccess(unsigned long argAddr, unsigned char argOp) {
		addr = argAddr;
		op = argOp;
	}
	~ReqNextCacheAccess() {
	}

	unsigned long getAddr() {
		return addr;
	}
	unsigned char getOp() {
		return op;
	}
};

class ReqPrevCacheInvalidate {
private:
	unsigned long cache_id;
	unsigned long addr;
	unsigned long tagMask;

public:
	ReqPrevCacheInvalidate(unsigned long argCacheId, unsigned long argAddr, unsigned long argTagMask) {
		cache_id = argCacheId;
		addr = argAddr;
		tagMask = argTagMask;
	}
	~ReqPrevCacheInvalidate() {
	}

	unsigned char getCacheId() {
		return cache_id;
	}
	unsigned long getAddr() {
		return addr;
	}
	unsigned long getTagMask() {
		return tagMask;
	}
};

class ReqDirectoryAccess {
private:
	unsigned long cache_id;
	unsigned long addr;
	unsigned char op;

public:
	ReqDirectoryAccess(unsigned long argCacheId, unsigned long argAddr, unsigned char argOp) {
		cache_id = argCacheId;
		addr = argAddr;
		op = argOp;
	}
	~ReqDirectoryAccess() {
	}

	unsigned long getCacheId() {
		return cache_id;
	}
	unsigned long getAddr() {
		return addr;
	}
	unsigned char getOp() {
		return op;
	}
};

class ReqPrevCacheInvalidateManager {
private:
	unsigned long dir_id;
	unsigned long numCore;
	unsigned long addr;
	unsigned long tagMask;
	list<pair<bool, unsigned long> *> involveCores;

public:
	ReqPrevCacheInvalidateManager(unsigned long argDirId, unsigned long argNumCore, unsigned long argAddr, unsigned long argTagMask, unsigned char *argBitVector) {
		dir_id = argDirId;
		numCore = argNumCore;
		addr = argAddr;
		tagMask = argTagMask;
		for (unsigned long i = 0; i < numCore; i++)
			if (argBitVector[i] == 'V') {
				pair<bool, unsigned long> *core;
				core = new pair<bool, unsigned long>(true, i);
				involveCores.push_back(core);
			}
	}
	~ReqPrevCacheInvalidateManager() {
		for (list<pair<bool, unsigned long> *>::iterator I = involveCores.begin(), E = involveCores.end(); I != E; I++)
			delete *I;
	}

	unsigned long getDirId() {
		return dir_id;
	}
	unsigned long getAddr() {
		return addr;
	}
	unsigned long getTagMask() {
		return tagMask;
	}
	unsigned long getInvolveCores() {
		unsigned long ret = 0;
		for (list<pair<bool, unsigned long> *>::iterator I = involveCores.begin(), E = involveCores.end(); I != E; I++)
			ret |= (unsigned long) 0x8000 >> (*I)->second; // TODO: 0x8000 <= 16 cores
		return ret;
	}
	unsigned long getInvolveCoresNum() {
		return involveCores.size();
	}
	list<pair<bool, unsigned long> *>::iterator getInvolveCoresBegin() {
		return involveCores.begin();
	}
	list<pair<bool, unsigned long> *>::iterator getInvolveCoresEnd() {
		return involveCores.end();
	}
};

class ReqPrevCacheReadManager {
private:
	unsigned long dir_id;
	unsigned long numCore;
	unsigned long addr;
	list<pair<bool, unsigned long> *> involveCores;
	list<pair<bool, unsigned long> *>::iterator shortest_I;

public:
	ReqPrevCacheReadManager(unsigned long argDirId, unsigned long argNumCore, unsigned long argAddr, unsigned char *argBitVector) {
		dir_id = argDirId;
		numCore = argNumCore;
		addr = argAddr;
		for (unsigned long i = 0; i < numCore; i++)
			if (argBitVector[i] == 'V') {
				pair<bool, unsigned long> *core;
				core = new pair<bool, unsigned long>(true, i);
				involveCores.push_back(core);
			}
		shortest_I = involveCores.end();
	}
	~ReqPrevCacheReadManager() {
		for (list<pair<bool, unsigned long> *>::iterator I = involveCores.begin(), E = involveCores.end(); I != E; I++)
			delete *I;
	}

	unsigned long getDirId() {
		return dir_id;
	}
	unsigned long getAddr() {
		return addr;
	}
	unsigned long getInvolveCores() {
		unsigned long ret = 0;
		for (list<pair<bool, unsigned long> *>::iterator I = involveCores.begin(), E = involveCores.end(); I != E; I++)
			ret |= (unsigned long) 0x8000 >> (*I)->second; // TODO: 0x8000 <= 16 cores
		return ret;
	}
	list<pair<bool, unsigned long> *>::iterator getInvolveCoresBegin() {
		return involveCores.begin();
	}
	list<pair<bool, unsigned long> *>::iterator getInvolveCoresEnd() {
		return involveCores.end();
	}
	void setShortestI(list<pair<bool, unsigned long> *>::iterator argI) {
		shortest_I = argI;
	}
	list<pair<bool, unsigned long> *>::iterator getShortestI() {
		return shortest_I;
	}
};

class ReqMPAccess {
private:
	t_mpiKind mpiKind;
	unsigned long srcDest;
	unsigned long tag;
	unsigned long long commWith;
	unsigned long root;
	unsigned long bytes;
	unsigned long buff1;
	unsigned long buff2;

public:
	ReqMPAccess(t_mpiKind argMpiKind, unsigned long argSrcDest, unsigned long argTag, unsigned long long argCommWith, unsigned long argRoot, unsigned long argBytes, unsigned long argBuff1, unsigned long argBuff2) {
		mpiKind = argMpiKind;
		srcDest = argSrcDest;
		tag = argTag;
		commWith = argCommWith;
		root = argRoot;
		bytes = argBytes;
		buff1 = argBuff1;
		buff2 = argBuff2;
	}
	~ReqMPAccess() {
	}

	t_mpiKind getMpiKind() {
		return mpiKind;
	}
	unsigned long getSrcDest() {
		return srcDest;
	}
	unsigned long getTag() {
		return tag;
	}
	unsigned long long getCommWith() {
		return commWith;
	}
	unsigned long getRoot() {
		return root;
	}
	unsigned long getBytes() {
		return bytes;
	}
	unsigned long getBuff1() {
		return buff1;
	}
	unsigned long getBuff2() {
		return buff2;
	}
};

#endif /* REQUEST_H_ */
