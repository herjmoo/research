#include "pin.H"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iomanip>
#include <list>

#define NUM_CORES 16
#define MAX_TRACE 10000000

struct access {
	char rw;
	ADDRINT addr;
	UINT32 size;
};

class Phase {
private:
	INT32 numCores;
	struct access **traces;
	INT32 *lenTraces;
	bool *flag;
    PIN_LOCK _lock;

public:
	Phase () {
		numCores = NUM_CORES;
		traces = new struct access * [numCores];
		lenTraces = new INT32 [numCores];
		flag = new bool [numCores];
		for (INT32 i=0; i<numCores; i++) {
			traces[i] = new struct access [MAX_TRACE];
			lenTraces[i] = 0;
			flag[i] = false;
		}
	};
	~Phase () {};

	void clear (THREADID tid) {
        PIN_GetLock(&_lock, tid);
		for (INT32 i=0; i<numCores; i++)
			lenTraces[i] = 0;
        PIN_ReleaseLock(&_lock);
	}
	void addAccess (THREADID tid, char argRw, ADDRINT argAddr, UINT32 argSize) {
        traces[tid][lenTraces[tid]].rw = argRw;
        traces[tid][lenTraces[tid]].addr = argAddr;
        traces[tid][lenTraces[tid]].size = argSize;
        PIN_GetLock(&_lock, 1);
        lenTraces[tid]++;
        assert(lenTraces[tid]<MAX_TRACE);
        PIN_ReleaseLock(&_lock);
	}
	void printStat (THREADID tid) {
        PIN_GetLock(&_lock, tid);
		for (INT32 i=0; i<numCores; i++)
			cout << setw(2) << i << ": " << setw(8) << lenTraces[i] << " ";
		cout << "\n";
        PIN_ReleaseLock(&_lock);
	}
	void printFile (INT32 seq) {
		if (seq<4) {
    	stringstream ss;
    	string file;
    	ss << "trace_ft_" << seq << ".out";
    	file = ss.str();
        ofstream out(file.c_str());
		for (INT32 i=0; i<numCores; i++)
			for (INT32 j=0; j<lenTraces[i]; j++)
				out << setw(2) << i << setw(2) << traces[i][j].rw << hex << setw(13) << traces[i][j].addr << dec << setw(2) << traces[i][j].size << "\n";

        out.close();
		}
	}
	void setFlag (THREADID tid) { flag[tid] = true; }
	void clearFlag (THREADID tid) { flag[tid] = false; }
	void setAllFlag () { for (INT32 i=0; i<numCores; i++) flag[i] = true; }
	void clearAllFlag () { for (INT32 i=0; i<numCores; i++) flag[i] = false; }
	bool getFlag (THREADID tid) { return flag[tid]; }
};

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,    "pintool",
    "o", "trace.out", "specify dcache file name");
KNOB<BOOL>   KnobTrackLoads(KNOB_MODE_WRITEONCE,    "pintool",
    "tl", "0", "track individual loads -- increases profiling time");
KNOB<BOOL>   KnobTrackStores(KNOB_MODE_WRITEONCE,   "pintool",
   "ts", "0", "track individual stores -- increases profiling time");
KNOB<UINT32> KnobThresholdHit(KNOB_MODE_WRITEONCE , "pintool",
   "rh", "100", "only report memops with hit count above threshold");
KNOB<UINT32> KnobThresholdMiss(KNOB_MODE_WRITEONCE, "pintool",
   "rm","100", "only report memops with miss count above threshold");
KNOB<UINT32> KnobCacheSize(KNOB_MODE_WRITEONCE, "pintool",
    "c","32", "cache size in kilobytes");
KNOB<UINT32> KnobLineSize(KNOB_MODE_WRITEONCE, "pintool",
    "b","32", "cache block size in bytes");
KNOB<UINT32> KnobAssociativity(KNOB_MODE_WRITEONCE, "pintool",
    "a","4", "cache associativity (1 for direct mapped)");

/* ===================================================================== */

INT32 Usage() {
    cerr << "This tool generate memory traces.\n";
    cerr << KNOB_BASE::StringKnobSummary();
    cerr << endl;

    return -1;
}

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

Phase trace;
INT32 phase = 0;

/* ===================================================================== */

VOID InsLoad(ADDRINT addr, UINT32 size, THREADID tid) {
	if (trace.getFlag(tid)) trace.addAccess(tid, 'r', addr, size);
	/*struct access *element;

	element = new struct access;
	element->rw = 'r';
	element->addr = addr;
	element->size = size;
	assert(tid<16);*/
	//if (traces[tid].size()<1000000)
	//	traces[tid].push_back(element);
    //cout << setw(2) << tid << " r" << hex << setw(10) << addr << dec << setw(2) << size << "\n";
	//countRead[tid]++;
}

/* ===================================================================== */

VOID InsStore(ADDRINT addr, UINT32 size, THREADID tid) {
	if (trace.getFlag(tid)) trace.addAccess(tid, 'w', addr, size);
	/*struct access *element;

	element = new struct access;
	element->rw = 'w';
	element->addr = addr;
	element->size = size;
	assert(tid<16);*/
	//if (traces[tid].size()<1000000)
	//	traces[tid].push_back(element);
    //cout << setw(2) << tid << " w" << hex << setw(10) << addr << dec << setw(2) << size << "\n";
	//countWrite[tid]++;
}

/* ===================================================================== */

VOID GOMP_parallel_start(THREADID tid) {
	//trace.printStat(tid);
	trace.printFile(phase++);
	trace.clear(tid);
	trace.clearAllFlag();
	cout << tid << ": GOMP_parallel_start" << std::endl;
}

VOID GOMP_parallel_end(THREADID tid) {
	//trace.printStat(tid);
	trace.printFile(phase++);
	trace.clear(tid);
	trace.setAllFlag();
	cout << tid << ": GOMP_parallel_end" << std::endl;
}

VOID omp_fn7start(THREADID tid) {
	trace.setFlag(tid);
	cout << tid << ": init_ui_.omp_fn.7 start" << std::endl;
}

VOID omp_fn7end(THREADID tid) {
	cout << tid << ": init_ui_.omp_fn.7 end" << std::endl;
	trace.clearFlag(tid);
}

VOID omp_fn6start(THREADID tid) {
	trace.setFlag(tid);
	cout << tid << ": evolve_.omp_fn.6 start" << std::endl;
}

VOID omp_fn6end(THREADID tid) {
	cout << tid << ": evolve_.omp_fn.6 end" << std::endl;
	trace.clearFlag(tid);
}

VOID omp_fn5start(THREADID tid) {
	trace.setFlag(tid);
	cout << tid << ": compute_initial_conditions_.omp_fn.5 start" << std::endl;
}

VOID omp_fn5end(THREADID tid) {
	cout << tid << ": compute_initial_conditions_.omp_fn.5 end" << std::endl;
	trace.clearFlag(tid);
}

VOID omp_fn4start(THREADID tid) {
	trace.setFlag(tid);
	cout << tid << ": compute_indexmap_.omp_fn.4 start" << std::endl;
}

VOID omp_fn4end(THREADID tid) {
	cout << tid << ": compute_indexmap_.omp_fn.4 end" << std::endl;
	trace.clearFlag(tid);
}

VOID omp_fn3start(THREADID tid) {
	trace.setFlag(tid);
	cout << tid << ": cffts1_.omp_fn.3 start" << std::endl;
}

VOID omp_fn3end(THREADID tid) {
	cout << tid << ": cffts1_.omp_fn.3 end" << std::endl;
	trace.clearFlag(tid);
}

VOID omp_fn2start(THREADID tid) {
	trace.setFlag(tid);
	cout << tid << ": cffts2_.omp_fn.2 start" << std::endl;
}

VOID omp_fn2end(THREADID tid) {
	cout << tid << ": cffts2_.omp_fn.2 end" << std::endl;
	trace.clearFlag(tid);
}

VOID omp_fn1start(THREADID tid) {
	trace.setFlag(tid);
	cout << tid << ": cffts3_.omp_fn.1 start" << std::endl;
}

VOID omp_fn1end(THREADID tid) {
	cout << tid << ": cffts3_.omp_fn.1 end" << std::endl;
	trace.clearFlag(tid);
}

VOID omp_fn0start(THREADID tid) {
	trace.setFlag(tid);
	cout << tid << ": checksum_.omp_fn.0 start" << std::endl;
}

VOID omp_fn0end(THREADID tid) {
	cout << tid << ": checksum_.omp_fn.0 end" << std::endl;
	trace.clearFlag(tid);
}

VOID omp_fn00start(THREADID tid) {
	trace.setFlag(tid);
	cout << tid << ": print_results_.omp_fn.0 start" << std::endl;
}

VOID omp_fn00end(THREADID tid) {
	cout << tid << ": print_results_.omp_fn.0 end" << std::endl;
	trace.clearFlag(tid);
}
/* ===================================================================== */

VOID Instruction(INS ins, void * v) {
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    switch (INS_Address(ins)) {
    case 0x400ed0: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) GOMP_parallel_start, IARG_THREAD_ID, IARG_END); break;
    case 0x40176a: case 0x4019b2: case 0x401b8f: case 0x4020f1: case 0x402783: case 0x402ab0: case 0x402ddd: case 0x40382f: case 0x4064e3: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) GOMP_parallel_end, IARG_THREAD_ID, IARG_END); break;
    case 0x40611f: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn7start, IARG_THREAD_ID, IARG_END); break;
    case 0x406338: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn7end, IARG_THREAD_ID, IARG_END); break;
    case 0x405e17: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn6start, IARG_THREAD_ID, IARG_END); break;
    case 0x40611e: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn6end, IARG_THREAD_ID, IARG_END); break;
    case 0x405cee: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn5start, IARG_THREAD_ID, IARG_END); break;
    case 0x405e16: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn5end, IARG_THREAD_ID, IARG_END); break;
    case 0x405b24: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn4start, IARG_THREAD_ID, IARG_END); break;
    case 0x405ced: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn4end, IARG_THREAD_ID, IARG_END); break;
    case 0x405770: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn3start, IARG_THREAD_ID, IARG_END); break;
    case 0x405b23: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn3end, IARG_THREAD_ID, IARG_END); break;
    case 0x4053b7: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn2start, IARG_THREAD_ID, IARG_END); break;
    case 0x40576f: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn2end, IARG_THREAD_ID, IARG_END); break;
    case 0x404ffe: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn1start, IARG_THREAD_ID, IARG_END); break;
    case 0x4053b6: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn1end, IARG_THREAD_ID, IARG_END); break;
    case 0x404e35: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn0start, IARG_THREAD_ID, IARG_END); break;
    case 0x404ffd: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn0end, IARG_THREAD_ID, IARG_END); break;
    case 0x407201: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn00start, IARG_THREAD_ID, IARG_END); break;
    case 0x40722a: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn00end, IARG_THREAD_ID, IARG_END); break;
    default: break;
    }

    for (UINT32 memOp=0; memOp<memOperands; memOp++) {
        const UINT32 size = INS_MemoryOperandSize(ins, memOp);

        if (INS_MemoryOperandIsRead(ins, memOp))
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) InsLoad, IARG_MEMORYOP_EA, memOp, IARG_UINT32, size, IARG_THREAD_ID, IARG_END);

        if (INS_MemoryOperandIsWritten(ins, memOp))
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) InsStore, IARG_MEMORYOP_EA, memOp, IARG_UINT32, size, IARG_THREAD_ID, IARG_END);
    }
}

/*VOID ThreadStart(THREADID tid, CONTEXT *ctxt, INT32 flags, VOID *v) {
	struct access *element;

	element = new struct access;
	element->rw = 't';
	element->addr = 0;
	element->size = 0;
	assert(tid<16);
	traces[tid].push_back(element);
}

VOID ThreadFini(THREADID tid, const CONTEXT *ctxt, INT32 code, VOID *v) {
	struct access *element;

	element = new struct access;
	element->rw = 't';
	element->addr = 0;
	element->size = 1;
	assert(tid<16);
	traces[tid].push_back(element);
}*/

/*VOID Fini(int code, VOID * v) {
    

    for (int i=0; i<16; i++) {
    	if (traces[i].size()) {
    	stringstream ss;
    	string file;
    	ss << "trace_" << i << ".out";
    	file = ss.str();
        ofstream out(file.c_str());
        for (list<struct access *>::iterator I=traces[i].begin(), E=traces[i].end(); I!=E; I++)
            out << setw(2) << i << setw(2) << (*I)->rw << hex << setw(13) << (*I)->addr << dec << setw(2) << (*I)->size << "\n";
        out.close();

        cout << "<Tread " << i << ">\n";
    	cout << "R: " << countRead[i] << "\n";
    	cout << "W: " << countWrite[i] << "\n";
    	}
    }
}*/


int main(int argc, char *argv[]) {
    PIN_InitSymbols();
    if(PIN_Init(argc,argv)) {
        return Usage();
    }

    INS_AddInstrumentFunction(Instruction, 0);
    //PIN_AddThreadStartFunction(ThreadStart, 0);
    //PIN_AddThreadFiniFunction(ThreadFini, 0);
    //PIN_AddFiniFunction(Fini, 0);

    trace.setAllFlag();
    PIN_StartProgram();

    return 0;
}
