/*
 * sim.h
 *
 *  Created on: Mar 18, 2014
 *      Author: Joonmoo Huh
 */

#ifndef SIM_H_
#define SIM_H_

#include <fstream>
#include <sstream>
#include <queue>
#include <stdlib.h>
#include <string.h>
#include "cache.h"
#include "directory.h"
#include "network.h"

#define TRACE_BUFFER 100

using namespace std;

class InflightAccess {
private:
	unsigned long blockSize;
	unsigned long log2blockSize;
	map<unsigned long, unsigned long> accesses;

public:
	InflightAccess(unsigned long b) {
		blockSize = b;
		log2blockSize = log2(blockSize);
	}
	~InflightAccess() {
	}

	void add_access(unsigned long addr) {
		addr >>= log2blockSize;
		assert(accesses.find(addr) == accesses.end());
		accesses[addr] = 0;
	}
	void drop_access(unsigned long addr) {
		addr >>= log2blockSize;
		assert(accesses.find(addr) != accesses.end());
		accesses.erase(addr);
	}
	bool is_inflight(unsigned long addr) {
		addr >>= log2blockSize;
		if (accesses.find(addr) == accesses.end())
			return false;
		else
			return true;
	}
	map<unsigned long, unsigned long>::iterator getBegin() {
		return accesses.begin();
	}
	map<unsigned long, unsigned long>::iterator getEnd() {
		return accesses.end();
	}
};

class Sim_Memory_Raw_Request {
protected:
	unsigned long pid;
	unsigned long addr;
	unsigned char op;
	unsigned long size;
	t_mpiKind mpiKind;
	unsigned long rank;
	unsigned long srcDest;
	unsigned long tag;
	unsigned long long commWith;
	unsigned long root;
	unsigned long bytes;
	unsigned long buff1;
	unsigned long buff2;

public:
	Sim_Memory_Raw_Request(unsigned long argPid, unsigned long argAddr, unsigned char argOp, unsigned long argSize) {
		pid = argPid;
		addr = argAddr;
		op = argOp;
		size = argSize;
		mpiKind = kd_NA;
		rank = 0;
		srcDest = 0;
		tag = 0;
		commWith = 0;
		root = 0;
		bytes = 0;
		buff1 = 0;
		buff2 = 0;
	}
	~Sim_Memory_Raw_Request() {
	}

	void setMpiArgs(t_mpiKind argMpiKind, unsigned long argRank, unsigned long argSrcDest, unsigned long argTag, unsigned long long argCommWith, unsigned long argRoot, unsigned long argBytes, unsigned long argBuff1, unsigned long argBuff2) {
		mpiKind = argMpiKind;
		rank = argRank;
		srcDest = argSrcDest;
		tag = argTag;
		commWith = argCommWith;
		root = argRoot;
		bytes = argBytes;
		buff1 = argBuff1;
		buff2 = argBuff2;
	}

	unsigned long getPid() {
		return pid;
	}
	unsigned long getAddr() {
		return addr;
	}
	unsigned char getOp() {
		return op;
	}
	unsigned long getSize() {
		return size;
	}
	t_mpiKind getMpiKind() {
		return mpiKind;
	}
	unsigned long getRank() {
		return rank;
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

class Sim_16core_Shared_HW {
private:
	Cache_WT *L1_cache[16];
	Cache_MESI *L2_cache[16];
	Directory_Manager_Interleave *directory;
	Memory *mem;
	Noc_4X4_Mesh *network;
	InflightAccess *inflight;
	bool printStatus[16];

public:
	Sim_16core_Shared_HW() {
		for (unsigned long i = 0; i < 16; i++) {
			L1_cache[i] = new Cache_WT(i, 32768, 64, 2, 3);
			L2_cache[i] = new
Cache_MESI(i, 8388608, 64, 4, 10);
		}
		directory = new Directory_Manager_Interleave(16, 16, 64, 3);
		mem = new Memory(150);
		network = new Noc_4X4_Mesh();
		inflight = new InflightAccess(64);

		for (unsigned long i = 0; i < 16; i++) {
			printStatus[i] = true;
		}
		for (unsigned long i = 0; i < 4; i++) {
			printStatus[i] = true;
		}
	}
	~Sim_16core_Shared_HW() {
		for (unsigned long i = 0; i < 16; i++) {
			delete L1_cache[i];
			delete L2_cache[i];
		}
		delete directory;
	}

	Cache_WT *getL1Cache(unsigned long argID) {
		return L1_cache[argID];
	}
	Cache_MESI *getL2Cache(unsigned long argID) {
		return L2_cache[argID];
	}
	Cache_MESI **getL2CacheList() {
		return L2_cache;
	}
	Directory_Manager_Interleave *getDirectoryManager() {
		return directory;
	}
	Noc_4X4_Mesh *getNetwork() {
		return network;
	}
	Memory *getMemory() {
		return mem;
	}
	InflightAccess *getInflight() {
		return inflight;
	}
	bool *getPrintStatus() {
		return printStatus;
	}

	void simLatency();

	void printStatus_L1s();
	void printStatus_L2s();
	void printStatus_Dirs();
	void printStatus_Inflight();
	void printStat();
};

class Sim_16core_Message_HW {
private:
	Cache_WB_SCL *L1_cache[16];
	Cache_WB_SCL *L2_cache[16];
	Memory *mem[16];
	Noc_4X4_Mesh *network;
	MPI_Control *mpi_control;
	bool printStatus[16];

public:
	Sim_16core_Message_HW() {
		for (unsigned long i = 0; i < 16; i++) {
			L1_cache[i] = new Cache_WB_SCL(i, 32768, 64, 2, 3);
			L2_cache[i] = new
Cache_WB_SCL(i, 8388608, 64, 4, 10);
			mem[i] = new Memory(150);
		}
		network = new Noc_4X4_Mesh();
		mpi_control = new MPI_Control(16);

		for (unsigned long i = 0; i < 16; i++) {
			printStatus[i] = false;
		}
		for (unsigned long i = 0; i < 4; i++) {
			printStatus[i] = true;
		}
	}
	~Sim_16core_Message_HW() {
		for (unsigned long i = 0; i < 16; i++) {
			delete L1_cache[i];
			delete L2_cache[i];
		}
	}

	Cache_WB_SCL *getL1Cache(unsigned long argID) {
		return L1_cache[argID];
	}
	Cache_WB_SCL *getL2Cache(unsigned long argID) {
		return L2_cache[argID];
	}
	Cache_WB_SCL **getL2CacheList() {
		return L2_cache;
	}
	Noc_4X4_Mesh *getNetwork() {
		return network;
	}
	MPI_Control *getMPIControl() {
		return mpi_control;
	}
	Memory *getMemory(unsigned long argID) {
		return mem[argID];
	}
	bool *getPrintStatus() {
		return printStatus;
	}

	void simLatency();

	void printStatus_L1s();
	void printStatus_L2s();
	void printStat();
};

class Trace_Manager {
private:
	unsigned long numCore;
	string fileName;
	string *listFileName;
	FILE **listFilePointer;

	FILE *file_open(string argFileName) {
		return fopen(argFileName.c_str(), "r");
	}
	unsigned long string2hex(char *addr);

public:
	Trace_Manager(unsigned long argNumCore) {
		numCore = argNumCore;
		for (unsigned long i = 0; i < numCore; i++) {
			listFileName = new string[numCore];
			listFilePointer = new FILE*[numCore];
		}
	}
	~Trace_Manager() {
		for (unsigned long i = 0; i < numCore; i++) {
			fclose(listFilePointer[i]);
		}
		delete listFileName;
		delete listFilePointer;
	}

	void setFileName(string argFileName) {
		fileName = argFileName;
		for (unsigned long i = 0; i < numCore; i++) {
			ostringstream ss;
			ss << i;
			if (i < 10)
				listFileName[i] = fileName + ".0" + ss.str();
			else
				listFileName[i] = fileName + "." + ss.str();
		}
	}

	void openFile() {
		for (unsigned long i = 0; i < numCore; i++) {
			listFilePointer[i] = file_open(listFileName[i]);
			if (listFilePointer[i] == NULL)
				cerr << "ERROR: file open problem: " << listFileName[i] << "\n";
		}
	}

	Sim_Memory_Raw_Request *readTrace(unsigned long argCore);
};

class Sim_16core_Shared_Request {
protected:
	Sim_16core_Shared_HW *hw;
	unsigned long pid;
	unsigned long addr;
	unsigned long addr_wb;
	unsigned char op;
	ReqNextCacheAccess *L1_access;
	ReqNextCacheAccess *L2_access;
	ReqDirectoryAccess *dir_access_WB;
	ReqDirectoryAccess *dir_access;
	ReqPrevCacheInvalidate *L1_inv_self;
	bool mem_read;
	ReqPrevCacheReadManager *L2_read;
	ReqPrevCacheInvalidateManager *L2_inv;
	list<pair<bool, ReqPrevCacheInvalidate *> *> L1_inv;
	ReqPrevCacheInvalidateManager *L2_inv_WB;

public:
	Sim_16core_Shared_Request(Sim_16core_Shared_HW *argHw, unsigned long argPid, unsigned long argAddr, unsigned char argOp) {
		hw = argHw;
		pid = argPid;
		addr = argAddr;
		addr_wb = 0;
		op = argOp;
		L1_access = new ReqNextCacheAccess(argAddr, argOp);
		L2_access = NULL;
		dir_access_WB = NULL;
		dir_access = NULL;
		L1_inv_self = NULL;
		mem_read = false;
		L2_read = NULL;
		L2_inv = NULL;
		L2_inv_WB = NULL;
	}
	~Sim_16core_Shared_Request() {
		hw = NULL;
	}

	unsigned long getPid() {
		return pid;
	}
	unsigned long getAddr() {
		return addr;
	}
	unsigned char getOp() {
		return op;
	}
	ReqNextCacheAccess *getL1_access() {
		return L1_access;
	}
	ReqNextCacheAccess *getL2_access() {
		return L2_access;
	}
	ReqDirectoryAccess *getDir_access_WB() {
		return dir_access_WB;
	}
	ReqDirectoryAccess *getDir_aceess() {
		return dir_access;
	}
	bool getMem_read() {
		return mem_read;
	}
	ReqPrevCacheReadManager *getL2_read() {
		return L2_read;
	}
	ReqPrevCacheInvalidateManager *getL2_inv() {
		return L2_inv;
	}
	ReqPrevCacheInvalidateManager *getL2_inv_WB() {
		return L2_inv_WB;
	}
	void do_L1_access();
	void do_L2_access();
	void do_dir_request();
	void do_dir_request_WB();
	void do_L1_inv_self();
	void do_L2_read();
	void do_L2_inv();
	void do_L2_inv_WB();
	void do_L1_inv();
	void do_mem_read();

	bool isEnd() {
		if (L1_access == NULL && L2_access == NULL && dir_access_WB == NULL	&& dir_access == NULL && L1_inv_self == NULL && mem_read == false && L2_read == NULL && L2_inv == NULL && L2_inv_WB == NULL)
			return true;
		else
			return false;
	}
};

class Sim_16core_Message_Request {
protected:
	Sim_16core_Message_HW *hw;
	unsigned long pid;
	unsigned long addr;
	unsigned long addr_wb;
	unsigned char op;
	ReqNextCacheAccess *L1_access;
	ReqNextCacheAccess *L2_access;
	ReqNextCacheAccess *L2_access_WB;
	ReqMPAccess *MP_access;
	bool mem_read;

	t_mpiKind mpiKind;
	unsigned long srcDest;
	unsigned long tag;
	unsigned long long commWith;
	unsigned long root;
	unsigned long bytes;
	unsigned long buff1;
	unsigned long buff2;

public:
	Sim_16core_Message_Request(Sim_16core_Message_HW *argHw, unsigned long argPid, unsigned long argAddr, unsigned char argOp) {
		hw = argHw;
		pid = argPid;
		addr = argAddr;
		addr_wb = 0;
		op = argOp;
		L1_access = new ReqNextCacheAccess(argAddr, argOp);
		L2_access = NULL;
		L2_access_WB = NULL;
		MP_access = NULL;
		mem_read = false;

		mpiKind = kd_NA;
		srcDest = 0;
		tag = 0;
		commWith = 0;
		root = 0;
		bytes = 0;
		buff1 = 0;
		buff2 = 0;
	}
	Sim_16core_Message_Request(Sim_16core_Message_HW *argHw, unsigned long argPid, unsigned long argAddr, unsigned char argOp, t_mpiKind argMpiKind, unsigned long argSrcDest, unsigned long argTag, unsigned long long argCommWith, unsigned long argRoot, unsigned long argBytes, unsigned long argBuff1, unsigned long argBuff2) {
		hw = argHw;
		pid = argPid;
		addr = argAddr;
		addr_wb = 0;
		op = argOp;
		L1_access = NULL;
		L2_access = NULL;
		L2_access_WB = NULL;
		MP_access = new ReqMPAccess(argMpiKind, argSrcDest, argTag, argCommWith, argRoot, argBytes, argBuff1, argBuff2);
		mem_read = false;

		mpiKind = argMpiKind;
		srcDest = argSrcDest;
		tag = argTag;
		commWith = argCommWith;
		root = argRoot;
		bytes = argBytes;
		buff1 = argBuff1;
		buff2 = argBuff2;
	}
	~Sim_16core_Message_Request() {
		hw = NULL;
	}

	unsigned long getPid() {
		return pid;
	}
	unsigned long getAddr() {
		return addr;
	}
	unsigned char getOp() {
		return op;
	}
	ReqNextCacheAccess *getL1_access() {
		return L1_access;
	}
	ReqNextCacheAccess *getL2_access() {
		return L2_access;
	}
	ReqNextCacheAccess *getL2_access_WB() {
		return L2_access_WB;
	}
	bool getMem_read() {
		return mem_read;
	}
	void do_L1_access();
	void do_L2_access();
	void do_L2_access_WB();
	void do_MP_access();
	void do_mem_read();

	bool isEnd() {
		if (L1_access == NULL && L2_access == NULL && L2_access_WB == NULL && MP_access == NULL && mem_read == false)
			return true;
		else
			return false;
	}
};

class Sim_16core_Shared {
private:
	Sim_16core_Shared_HW *hw;
	Trace_Manager *trace;
	queue<Sim_16core_Shared_Request *> req_queue[16];
	unsigned long global_cycle;
	unsigned long MP_cycle;

	void increase_cycle() {
		global_cycle++;
	}
	unsigned long get_cycle() {
		return global_cycle;
	}

public:
	Sim_16core_Shared() {
		hw = new Sim_16core_Shared_HW();
		trace = new Trace_Manager(16);
		global_cycle = 0;
		MP_cycle = 0;
	}
	~Sim_16core_Shared() {
	}

	void doSim(string argFileName);

	void printStatus_MemReq();
};

class Sim_16core_Message {
private:
	Sim_16core_Message_HW *hw;
	Trace_Manager *trace;
	queue<Sim_16core_Message_Request *> req_queue[16];
	unsigned long global_cycle;

	void increase_cycle() {
		global_cycle++;
	}
	unsigned long get_cycle() {
		return global_cycle;
	}

public:
	Sim_16core_Message() {
		hw = new Sim_16core_Message_HW();
		trace = new Trace_Manager(16);
		global_cycle = 0;
	}
	~Sim_16core_Message() {
	}

	void doSim(string argFileName);

	void printStatus_MemReq();
};

#endif /* SIM_H_ *//*
 * sim.h
 *
 *  Created on: Mar 18, 2014
 *      Author: Joonmoo Huh
 */

#ifndef SIM_H_
#define SIM_H_

#include <fstream>
#include <sstream>
#include <queue>
#include <stdlib.h>
#include <string.h>
#include "cache.h"
#include "directory.h"
#include "network.h"

#define TRACE_BUFFER 100

using namespace std;

class InflightAccess {
private:
	unsigned long blockSize;
	unsigned long log2blockSize;
	map<unsigned long, unsigned long> accesses;

public:
	InflightAccess(unsigned long b) {
		blockSize = b;
		log2blockSize = log2(blockSize);
	}
	~InflightAccess() {
	}

	void add_access(unsigned long addr) {
		addr >>= log2blockSize;
		assert(accesses.find(addr) == accesses.end());
		accesses[addr] = 0;
	}
	void drop_access(unsigned long addr) {
		addr >>= log2blockSize;
		assert(accesses.find(addr) != accesses.end());
		accesses.erase(addr);
	}
	bool is_inflight(unsigned long addr) {
		addr >>= log2blockSize;
		if (accesses.find(addr) == accesses.end())
			return false;
		else
			return true;
	}
	map<unsigned long, unsigned long>::iterator getBegin() {
		return accesses.begin();
	}
	map<unsigned long, unsigned long>::iterator getEnd() {
		return accesses.end();
	}
};

class Sim_Memory_Raw_Request {
protected:
	unsigned long pid;
	unsigned long addr;
	unsigned char op;
	unsigned long size;
	t_mpiKind mpiKind;
	unsigned long rank;
	unsigned long srcDest;
	unsigned long tag;
	unsigned long long commWith;
	unsigned long root;
	unsigned long bytes;
	unsigned long buff1;
	unsigned long buff2;

public:
	Sim_Memory_Raw_Request(unsigned long argPid, unsigned long argAddr, unsigned char argOp, unsigned long argSize) {
		pid = argPid;
		addr = argAddr;
		op = argOp;
		size = argSize;
		mpiKind = kd_NA;
		rank = 0;
		srcDest = 0;
		tag = 0;
		commWith = 0;
		root = 0;
		bytes = 0;
		buff1 = 0;
		buff2 = 0;
	}
	~Sim_Memory_Raw_Request() {
	}

	void setMpiArgs(t_mpiKind argMpiKind, unsigned long argRank, unsigned long argSrcDest, unsigned long argTag, unsigned long long argCommWith, unsigned long argRoot, unsigned long argBytes, unsigned long argBuff1, unsigned long argBuff2) {
		mpiKind = argMpiKind;
		rank = argRank;
		srcDest = argSrcDest;
		tag = argTag;
		commWith = argCommWith;
		root = argRoot;
		bytes = argBytes;
		buff1 = argBuff1;
		buff2 = argBuff2;
	}

	unsigned long getPid() {
		return pid;
	}
	unsigned long getAddr() {
		return addr;
	}
	unsigned char getOp() {
		return op;
	}
	unsigned long getSize() {
		return size;
	}
	t_mpiKind getMpiKind() {
		return mpiKind;
	}
	unsigned long getRank() {
		return rank;
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

class Sim_16core_Shared_HW {
private:
	Cache_WT *L1_cache[16];
	Cache_MESI *L2_cache[16];
	Directory_Manager_Interleave *directory;
	Memory *mem;
	Noc_4X4_Mesh *network;
	InflightAccess *inflight;
	bool printStatus[16];

public:
	Sim_16core_Shared_HW() {
		for (unsigned long i = 0; i < 16; i++) {
			L1_cache[i] = new Cache_WT(i, 32768, 64, 2, 3);
L2_cache[i]= new Cache_MESI(i, 262144, 64, 4, 10);
		}
		directory = new Directory_Manager_Interleave(16, 16, 64, 3);
		mem = new Memory(150);
		network = new Noc_4X4_Mesh();
		inflight = new InflightAccess(64);

		for (unsigned long i = 0; i < 16; i++) {
			printStatus[i] = true;
		}
		for (unsigned long i = 0; i < 4; i++) {
			printStatus[i] = true;
		}
	}
	~Sim_16core_Shared_HW() {
		for (unsigned long i = 0; i < 16; i++) {
			delete L1_cache[i];
			delete L2_cache[i];
		}
		delete directory;
	}

	Cache_WT *getL1Cache(unsigned long argID) {
		return L1_cache[argID];
	}
	Cache_MESI *getL2Cache(unsigned long argID) {
		return L2_cache[argID];
	}
	Cache_MESI **getL2CacheList() {
		return L2_cache;
	}
	Directory_Manager_Interleave *getDirectoryManager() {
		return directory;
	}
	Noc_4X4_Mesh *getNetwork() {
		return network;
	}
	Memory *getMemory() {
		return mem;
	}
	InflightAccess *getInflight() {
		return inflight;
	}
	bool *getPrintStatus() {
		return printStatus;
	}

	void simLatency();

	void printStatus_L1s();
	void printStatus_L2s();
	void printStatus_Dirs();
	void printStatus_Inflight();
	void printStat();
};

class Sim_16core_Message_HW {
private:
	Cache_WB_SCL *L1_cache[16];
	Cache_WB_SCL *L2_cache[16];
	Memory *mem[16];
	Noc_4X4_Mesh *network;
	MPI_Control *mpi_control;
	bool printStatus[16];

public:
	Sim_16core_Message_HW() {
		for (unsigned long i = 0; i < 16; i++) {
			L1_cache[i] = new Cache_WB_SCL(i, 32768, 64, 2, 3);
L2_cache[i]= new Cache_WB_SCL(i, 262144, 64, 4, 10);
			mem[i] = new Memory(150);
		}
		network = new Noc_4X4_Mesh();
		mpi_control = new MPI_Control(16);

		for (unsigned long i = 0; i < 16; i++) {
			printStatus[i] = false;
		}
		for (unsigned long i = 0; i < 4; i++) {
			printStatus[i] = true;
		}
	}
	~Sim_16core_Message_HW() {
		for (unsigned long i = 0; i < 16; i++) {
			delete L1_cache[i];
			delete L2_cache[i];
		}
	}

	Cache_WB_SCL *getL1Cache(unsigned long argID) {
		return L1_cache[argID];
	}
	Cache_WB_SCL *getL2Cache(unsigned long argID) {
		return L2_cache[argID];
	}
	Cache_WB_SCL **getL2CacheList() {
		return L2_cache;
	}
	Noc_4X4_Mesh *getNetwork() {
		return network;
	}
	MPI_Control *getMPIControl() {
		return mpi_control;
	}
	Memory *getMemory(unsigned long argID) {
		return mem[argID];
	}
	bool *getPrintStatus() {
		return printStatus;
	}

	void simLatency();

	void printStatus_L1s();
	void printStatus_L2s();
	void printStat();
};

class Trace_Manager {
private:
	unsigned long numCore;
	string fileName;
	string *listFileName;
	FILE **listFilePointer;

	FILE *file_open(string argFileName) {
		return fopen(argFileName.c_str(), "r");
	}
	unsigned long string2hex(char *addr);

public:
	Trace_Manager(unsigned long argNumCore) {
		numCore = argNumCore;
		for (unsigned long i = 0; i < numCore; i++) {
			listFileName = new string[numCore];
			listFilePointer = new FILE*[numCore];
		}
	}
	~Trace_Manager() {
		for (unsigned long i = 0; i < numCore; i++) {
			fclose(listFilePointer[i]);
		}
		delete listFileName;
		delete listFilePointer;
	}

	void setFileName(string argFileName) {
		fileName = argFileName;
		for (unsigned long i = 0; i < numCore; i++) {
			ostringstream ss;
			ss << i;
			if (i < 10)
				listFileName[i] = fileName + ".0" + ss.str();
			else
				listFileName[i] = fileName + "." + ss.str();
		}
	}

	void openFile() {
		for (unsigned long i = 0; i < numCore; i++) {
			listFilePointer[i] = file_open(listFileName[i]);
			if (listFilePointer[i] == NULL)
				cerr << "ERROR: file open problem: " << listFileName[i] << "\n";
		}
	}

	Sim_Memory_Raw_Request *readTrace(unsigned long argCore);
};

class Sim_16core_Shared_Request {
protected:
	Sim_16core_Shared_HW *hw;
	unsigned long pid;
	unsigned long addr;
	unsigned long addr_wb;
	unsigned char op;
	ReqNextCacheAccess *L1_access;
	ReqNextCacheAccess *L2_access;
	ReqDirectoryAccess *dir_access_WB;
	ReqDirectoryAccess *dir_access;
	ReqPrevCacheInvalidate *L1_inv_self;
	bool mem_read;
	ReqPrevCacheReadManager *L2_read;
	ReqPrevCacheInvalidateManager *L2_inv;
	list<pair<bool, ReqPrevCacheInvalidate *> *> L1_inv;
	ReqPrevCacheInvalidateManager *L2_inv_WB;

public:
	Sim_16core_Shared_Request(Sim_16core_Shared_HW *argHw, unsigned long argPid, unsigned long argAddr, unsigned char argOp) {
		hw = argHw;
		pid = argPid;
		addr = argAddr;
		addr_wb = 0;
		op = argOp;
		L1_access = new ReqNextCacheAccess(argAddr, argOp);
		L2_access = NULL;
		dir_access_WB = NULL;
		dir_access = NULL;
		L1_inv_self = NULL;
		mem_read = false;
		L2_read = NULL;
		L2_inv = NULL;
		L2_inv_WB = NULL;
	}
	~Sim_16core_Shared_Request() {
		hw = NULL;
	}

	unsigned long getPid() {
		return pid;
	}
	unsigned long getAddr() {
		return addr;
	}
	unsigned char getOp() {
		return op;
	}
	ReqNextCacheAccess *getL1_access() {
		return L1_access;
	}
	ReqNextCacheAccess *getL2_access() {
		return L2_access;
	}
	ReqDirectoryAccess *getDir_access_WB() {
		return dir_access_WB;
	}
	ReqDirectoryAccess *getDir_aceess() {
		return dir_access;
	}
	bool getMem_read() {
		return mem_read;
	}
	ReqPrevCacheReadManager *getL2_read() {
		return L2_read;
	}
	ReqPrevCacheInvalidateManager *getL2_inv() {
		return L2_inv;
	}
	ReqPrevCacheInvalidateManager *getL2_inv_WB() {
		return L2_inv_WB;
	}
	void do_L1_access();
	void do_L2_access();
	void do_dir_request();
	void do_dir_request_WB();
	void do_L1_inv_self();
	void do_L2_read();
	void do_L2_inv();
	void do_L2_inv_WB();
	void do_L1_inv();
	void do_mem_read();

	bool isEnd() {
		if (L1_access == NULL && L2_access == NULL && dir_access_WB == NULL	&& dir_access == NULL && L1_inv_self == NULL && mem_read == false && L2_read == NULL && L2_inv == NULL && L2_inv_WB == NULL)
			return true;
		else
			return false;
	}
};

class Sim_16core_Message_Request {
protected:
	Sim_16core_Message_HW *hw;
	unsigned long pid;
	unsigned long addr;
	unsigned long addr_wb;
	unsigned char op;
	ReqNextCacheAccess *L1_access;
	ReqNextCacheAccess *L2_access;
	ReqNextCacheAccess *L2_access_WB;
	ReqMPAccess *MP_access;
	bool mem_read;

	t_mpiKind mpiKind;
	unsigned long srcDest;
	unsigned long tag;
	unsigned long long commWith;
	unsigned long root;
	unsigned long bytes;
	unsigned long buff1;
	unsigned long buff2;

public:
	Sim_16core_Message_Request(Sim_16core_Message_HW *argHw, unsigned long argPid, unsigned long argAddr, unsigned char argOp) {
		hw = argHw;
		pid = argPid;
		addr = argAddr;
		addr_wb = 0;
		op = argOp;
		L1_access = new ReqNextCacheAccess(argAddr, argOp);
		L2_access = NULL;
		L2_access_WB = NULL;
		MP_access = NULL;
		mem_read = false;

		mpiKind = kd_NA;
		srcDest = 0;
		tag = 0;
		commWith = 0;
		root = 0;
		bytes = 0;
		buff1 = 0;
		buff2 = 0;
	}
	Sim_16core_Message_Request(Sim_16core_Message_HW *argHw, unsigned long argPid, unsigned long argAddr, unsigned char argOp, t_mpiKind argMpiKind, unsigned long argSrcDest, unsigned long argTag, unsigned long long argCommWith, unsigned long argRoot, unsigned long argBytes, unsigned long argBuff1, unsigned long argBuff2) {
		hw = argHw;
		pid = argPid;
		addr = argAddr;
		addr_wb = 0;
		op = argOp;
		L1_access = NULL;
		L2_access = NULL;
		L2_access_WB = NULL;
		MP_access = new ReqMPAccess(argMpiKind, argSrcDest, argTag, argCommWith, argRoot, argBytes, argBuff1, argBuff2);
		mem_read = false;

		mpiKind = argMpiKind;
		srcDest = argSrcDest;
		tag = argTag;
		commWith = argCommWith;
		root = argRoot;
		bytes = argBytes;
		buff1 = argBuff1;
		buff2 = argBuff2;
	}
	~Sim_16core_Message_Request() {
		hw = NULL;
	}

	unsigned long getPid() {
		return pid;
	}
	unsigned long getAddr() {
		return addr;
	}
	unsigned char getOp() {
		return op;
	}
	ReqNextCacheAccess *getL1_access() {
		return L1_access;
	}
	ReqNextCacheAccess *getL2_access() {
		return L2_access;
	}
	ReqNextCacheAccess *getL2_access_WB() {
		return L2_access_WB;
	}
	bool getMem_read() {
		return mem_read;
	}
	void do_L1_access();
	void do_L2_access();
	void do_L2_access_WB();
	void do_MP_access();
	void do_mem_read();

	bool isEnd() {
		if (L1_access == NULL && L2_access == NULL && L2_access_WB == NULL && MP_access == NULL && mem_read == false)
			return true;
		else
			return false;
	}
};

class Sim_16core_Shared {
private:
	Sim_16core_Shared_HW *hw;
	Trace_Manager *trace;
	queue<Sim_16core_Shared_Request *> req_queue[16];
	unsigned long global_cycle;
	unsigned long MP_cycle;

	void increase_cycle() {
		global_cycle++;
	}
	unsigned long get_cycle() {
		return global_cycle;
	}

public:
	Sim_16core_Shared() {
		hw = new Sim_16core_Shared_HW();
		trace = new Trace_Manager(16);
		global_cycle = 0;
		MP_cycle = 0;
	}
	~Sim_16core_Shared() {
	}

	void doSim(string argFileName);

	void printStatus_MemReq();
};

class Sim_16core_Message {
private:
	Sim_16core_Message_HW *hw;
	Trace_Manager *trace;
	queue<Sim_16core_Message_Request *> req_queue[16];
	unsigned long global_cycle;

	void increase_cycle() {
		global_cycle++;
	}
	unsigned long get_cycle() {
		return global_cycle;
	}

public:
	Sim_16core_Message() {
		hw = new Sim_16core_Message_HW();
		trace = new Trace_Manager(16);
		global_cycle = 0;
	}
	~Sim_16core_Message() {
	}

	void doSim(string argFileName);

	void printStatus_MemReq();
};

#endif /* SIM_H_ */
