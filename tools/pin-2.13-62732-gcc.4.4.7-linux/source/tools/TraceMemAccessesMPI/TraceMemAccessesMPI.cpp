#include "pin.H"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iomanip>
#include <list>

#define NUM_CORES 16
#define MAX_TRACE 100000000

struct access {
	char rw;
	ADDRINT addr;
	UINT32 size;
};

class Phase {
private:
	struct access *traces;
	INT32 lenTraces;
	bool flag;

public:
	Phase () {
		traces = new struct access [MAX_TRACE];
		lenTraces = 0;
		flag = false;
	};
	~Phase () {};

	void clear () {
		lenTraces = 0;
	}
	void addAccess (char argRw, ADDRINT argAddr, UINT32 argSize) {
        traces[lenTraces].rw = argRw;
        traces[lenTraces].addr = argAddr;
        traces[lenTraces].size = argSize;
        lenTraces++;
        assert(lenTraces<MAX_TRACE);
	}
	void printFile (INT32 seq) {
    	//stringstream ss;
    	//string file;
    	//ss << "trace_ft_" << seq << ".out";
    	//file = ss.str();
        //ofstream out(file.c_str());
		cerr << "PHASE " << seq << "\n";
		cerr << ":<PHASE> " << seq << " : " << lenTraces << "\n";
		for (INT32 j=0; j<lenTraces; j++)
			cerr << setw(2) << 0 << setw(2) << traces[j].rw << hex << setw(13) << traces[j].addr << dec << " " << traces[j].size << "\n";
        //out.close();
	}
	void setFlag () { flag = true; }
	void clearFlag () { flag = false; }
	bool getFlag () { return flag; }
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
bool global_flag = false;
string invalid = "invalid_rtn";

/* ===================================================================== */

const string *Target2String(ADDRINT target) {
    string name = RTN_FindNameByAddress(target);
    if (name == "")
        return &invalid;
    else
        return new string(name);
}

/* ===================================================================== */

VOID  do_call_args(ADDRINT addr, const string *s, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3, ADDRINT arg4, ADDRINT arg5) {
	if (global_flag/**s=="mpi_alltoall_"*/) {
		if (addr==0x40717e || addr==0x407e2e || addr==0x40871e) {
			trace.clearFlag();
			trace.printFile(phase++);
			trace.clear();
			trace.printFile(phase++);
			trace.clear();
			cerr << ": alltoall start" << std::endl;
			cerr << "MPI_ALLTOALL " << hex << arg0 << " " << arg3 << dec << "\n";

		}
		else if (addr==0x408d70) {
			trace.clearFlag();
			trace.printFile(phase++);
			trace.clear();
			trace.printFile(phase++);
			trace.clear();
			cerr << ": reduce start" << std::endl;
			cerr << "MPI_REDUCE " << hex << arg0 << " " << arg1 << dec << "\n";
		}
		//else
		//	cerr << *s << "(" << arg0 << ",...)" << endl;
	}
	//else if (*s=="mpi_reduce_") {
	//	cerr << *s << "(" << arg0 << ",...)" << endl;
	//}
}

/* ===================================================================== */

VOID  do_call_args_indirect(ADDRINT addr, ADDRINT target, BOOL taken, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3, ADDRINT arg4, ADDRINT arg5) {
    if( !taken ) return;

    const string *s = Target2String(target);
    do_call_args(addr, s, arg0, arg1, arg2, arg3, arg4, arg5);

    if (s != &invalid)
        delete s;
}

/* ===================================================================== */

VOID InsLoad(ADDRINT addr, UINT32 size) {
	if (global_flag) if (trace.getFlag()) trace.addAccess('r', addr, size);
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

VOID InsStore(ADDRINT addr, UINT32 size) {
	if (global_flag) if (trace.getFlag()) trace.addAccess('w', addr, size);
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

VOID Region_start() {
	if (!global_flag && !phase) {
		cerr << ": Region_start" << std::endl;
		global_flag = true;
		trace.setFlag();
	}
}

VOID Region_end() {
	if (global_flag) {
		trace.clearFlag();
		trace.printFile(phase++);
		trace.clear();
		global_flag = false;
		cerr << ": Region_end" << std::endl;
	}
}

VOID MPI_alltoall_start() {
	if (global_flag) {
	}
}

VOID MPI_alltoall_end() {
	if (global_flag) {
		cerr << ": alltoall end" << std::endl;
		trace.setFlag();
	}
}

VOID MPI_reduce_start() {
	if (global_flag) {
	}
}

VOID MPI_reduce_end() {
	if (global_flag) {
		cerr << ": reduce end" << std::endl;
		trace.setFlag();
	}
}
/* ===================================================================== */

VOID Instruction(INS ins, void * v) {
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    switch (INS_Address(ins)) {
    case 0x402066: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) Region_start, IARG_END); break;
    case 0x402196: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) Region_end, IARG_END); break;
    case 0x40717e: case 0x407e2e: case 0x40871e: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) MPI_alltoall_start, IARG_END); break;
    case 0x407183: case 0x407e33: case 0x408723: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) MPI_alltoall_end, IARG_END); break;
    case 0x408d70: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) MPI_reduce_start, IARG_END); break;
    case 0x408d75: INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) MPI_reduce_end, IARG_END); break;
    default: break;
    }

    if (INS_IsCall(ins)) {
    	{
			if (INS_IsDirectBranchOrCall(ins)) {
				const ADDRINT target = INS_DirectBranchOrCallTargetAddress(ins);
				INS_InsertPredicatedCall(ins, IPOINT_BEFORE, AFUNPTR(do_call_args), IARG_ADDRINT, INS_Address(ins), IARG_PTR, Target2String(target), IARG_G_ARG0_CALLER, IARG_G_ARG1_CALLER, IARG_G_ARG2_CALLER, IARG_G_ARG3_CALLER, IARG_G_ARG4_CALLER, IARG_G_ARG5_CALLER, IARG_END);
			}
			else {
				INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(do_call_args_indirect), IARG_ADDRINT, INS_Address(ins), IARG_BRANCH_TARGET_ADDR, IARG_BRANCH_TAKEN, IARG_G_ARG0_CALLER, IARG_G_ARG1_CALLER, IARG_G_ARG2_CALLER, IARG_G_ARG3_CALLER, IARG_G_ARG4_CALLER, IARG_G_ARG5_CALLER, IARG_END);
			}
    	}
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

    trace.clearFlag();
    PIN_StartProgram();

    return 0;
}
