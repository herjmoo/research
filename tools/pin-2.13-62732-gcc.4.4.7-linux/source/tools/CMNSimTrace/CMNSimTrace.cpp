#include "pin.H"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iomanip>
#include <list>
#include <map>
#include <string.h>
#include <stdlib.h>

#define NUM_CORES 16

class Phase {
private:
	UINT32 numCores;
	ofstream *outFiles;
	UINT32 *lenTraces;
    map <ADDRINT, int> memFootPrint;
    map <ADDRINT, int> *memFootPrintPerThreads;
	UINT32 phase;
	bool *flag;
	bool output_direction;
    PIN_LOCK _lock;

public:
	Phase () {
		numCores = NUM_CORES;
		outFiles = new ofstream [numCores];
		lenTraces = new UINT32 [numCores];
		memFootPrintPerThreads = new map <ADDRINT, int> [numCores];
		phase = 0;
		flag = new bool [numCores];
		output_direction = false;
		for (UINT32 i=0; i<numCores; i++) {
			lenTraces[i] = 0;
			flag[i] = false;
		}
	};
	~Phase () {};

	void clear (THREADID tid) {
        PIN_GetLock(&_lock, tid);
		for (UINT32 i=0; i<numCores; i++)
			lenTraces[i] = 0;
        PIN_ReleaseLock(&_lock);
	}
	void addAccess (THREADID tid, char argRw, ADDRINT argAddr, UINT32 argSize) {
        PIN_GetLock(&_lock, tid);
	if (output_direction) cout << tid << " " << argRw << " " << hex << argAddr << dec << " " << argSize << "\n";
	else outFiles[tid] << tid << " " << argRw << " " << hex << argAddr << dec << " " << argSize << "\n";
        lenTraces[tid]++;
        for (UINT32 i=0; i<argSize; i++) {
			if (memFootPrint.find(argAddr+i)==memFootPrint.end()) {
				memFootPrint[argAddr+i] = 1;
			}
			else {
				memFootPrint[argAddr+i]++;
			}
			if (!output_direction) {
				if (memFootPrintPerThreads[tid].find(argAddr+i)==memFootPrintPerThreads[tid].end()) {
					memFootPrintPerThreads[tid][argAddr+i] = 1;
				}
				else {
					memFootPrintPerThreads[tid][argAddr+i]++;
				}
			}
        }
        PIN_ReleaseLock(&_lock);
	}
	void addPhase (THREADID tid) {
		ostringstream ss;
		ss << phase;
		string ln = "PHASE " + ss.str() + "\n";

        PIN_GetLock(&_lock, tid);
        if (output_direction) {
        	cout << ln;
        }
        else {
			for (UINT32 i=0; i<numCores; i++)
				outFiles[i] << ln;
        }
        phase++;
        PIN_ReleaseLock(&_lock);
        assert(!tid);
	}
	void printStat (THREADID tid) {
        PIN_GetLock(&_lock, tid);
		for (UINT32 i=0; i<numCores; i++)
			cout << setw(2) << i << ": " << setw(8) << lenTraces[i] << " ";
		cout << "\n";
        PIN_ReleaseLock(&_lock);
        assert(!tid);
	}
	void setOutputFile (string name) {
		for (UINT32 i=0; i<numCores; i++) {
			ostringstream ss;
			string str;
			ss << i;
			str = name + ".0" + ss.str();
			if (i<10) str = name + ".0" + ss.str();
			else str = name + "." + ss.str();
			outFiles[i].open(str.c_str());
	        if (!outFiles[i].is_open()) {
	            cerr << "fail to open output file(" << i << ")\n";
	        }
		}
	}
	void releaseOutputFile () {
		for (UINT32 i=0; i<numCores; i++) {
	        if (outFiles[i].is_open()) {
	        	outFiles[i].close();
	        }
		}
	}
	void setFlag (THREADID tid) { flag[tid] = true; }
	void clearFlag (THREADID tid) { flag[tid] = false; }
	void setAllFlag () { for (UINT32 i=0; i<numCores; i++) flag[i] = true; }
	void clearAllFlag () { for (UINT32 i=0; i<numCores; i++) flag[i] = false; }
	void setOutput_direction (bool argDirection) { output_direction = argDirection; }
	bool getOutput_direction () { return output_direction; }
	UINT32 getPhase () { return phase; }
	bool getFlag (THREADID tid) { return flag[tid]; }
	UINT32 getMemFootPrintSize () { return memFootPrint.size(); }
	map <ADDRINT, int>::iterator getMemFootPrintPerThreadBegin (unsigned int tid) { return memFootPrintPerThreads[tid].begin(); }
	map <ADDRINT, int>::iterator getMemFootPrintPerThreadEnd (unsigned int tid) { return memFootPrintPerThreads[tid].end(); }

};

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobTarget(KNOB_MODE_WRITEONCE, "pintool", "tg", "", "specify architecture");
KNOB<string> KnobInputFile(KNOB_MODE_WRITEONCE, "pintool", "i", "", "specify objdump file name");
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "", "specify trace file name");
KNOB<string> KnobAddrRegionStart(KNOB_MODE_WRITEONCE, "pintool", "rs", "", "address of start point to trace");
KNOB<string> KnobAddrRegionEnd(KNOB_MODE_WRITEONCE, "pintool", "re", "", "address of end point to trace");

/* ===================================================================== */

INT32 Usage() {
    cerr << "This tool generate memory traces for CMNSim.\n";
    cerr << KNOB_BASE::StringKnobSummary();
    cerr << endl;

    return -1;
}

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

Phase trace;
ifstream obj_dump;
typedef enum instPoint { pt_Region_start, pt_Region_end, pt_GOMP_parallel_start, pt_GOMP_parallel_end, pt_omp_fn_start, pt_omp_fn_end, pt_PinMPI_Send, pt_PinMPI_Recv, pt_PinMPI_Barrier, pt_PinMPI_Bcast, pt_PinMPI_Reduce, pt_PinMPI_Allreduce, pt_PinMPI_Allgather, pt_PinMPI_Alltoall, pt_PinMPI_Alltoallv, pt_MPI_Send, pt_MPI_Recv, pt_MPI_Isend, pt_MPI_Irecv, pt_MPI_Wait, pt_MPI_Barrier, pt_MPI_Bcast, pt_MPI_Reduce, pt_MPI_Allreduce, pt_MPI_Allgather, pt_MPI_Alltoall, pt_MPI_Alltoallv, pt_MPI_End } t_instPoint;
map <ADDRINT, t_instPoint> addr_instPoint;
bool global_flag = false;
string invalid = "invalid_rtn";

/* ===================================================================== */

unsigned long string2hex (char *addr) {
	int i;
	unsigned long inc = 1;
	int len = strlen(addr)-1;
	unsigned long dec_sum = 0;

	for(i=0; i<len+1; i++){
		switch(*(addr+len-i)){
		case '1': dec_sum += 1*inc;  break;
		case '2': dec_sum += 2*inc;  break;
		case '3': dec_sum += 3*inc;  break;
		case '4': dec_sum += 4*inc;  break;
		case '5': dec_sum += 5*inc;  break;
		case '6': dec_sum += 6*inc;  break;
		case '7': dec_sum += 7*inc;  break;
		case '8': dec_sum += 8*inc;  break;
		case '9': dec_sum += 9*inc;  break;
		case 'a': dec_sum += 10*inc; break;
		case 'b': dec_sum += 11*inc; break;
		case 'c': dec_sum += 12*inc; break;
		case 'd': dec_sum += 13*inc; break;
		case 'e': dec_sum += 14*inc; break;
		case 'f': dec_sum += 15*inc; break;
		case '0': case ' ': break;
		default: return 0;
		}
		inc = inc * 16;
	}
	return dec_sum;
}

/* ===================================================================== */

const string *Target2String(ADDRINT target) {
    string name = RTN_FindNameByAddress(target);
    if (name == "")
        return &invalid;
    else
        return new string(name);
}

/* ===================================================================== */

VOID do_call_args(ADDRINT addr, const string *s, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3, ADDRINT arg4, ADDRINT arg5) {
	if (global_flag) {
	    if (addr_instPoint.find(addr)!=addr_instPoint.end()) {
			switch (addr_instPoint[addr]) {
			case pt_PinMPI_Send:
				cout << "MPI_SEND " << arg0 << " " << arg1 << " " << arg2 << " " << arg3 << " " << hex << arg4 << dec << "\n";
				break;
			case pt_PinMPI_Recv:
				cout << "MPI_RECV " << arg0 << " " << arg1 << " " << arg2 << " " << arg3 << " " << hex << arg4 << dec << "\n";
				break;
			case pt_PinMPI_Barrier:
				cout << "MPI_BARRIER " << arg0 << " " << hex << arg1 << dec << " " << arg2 << "\n";
				break;
			case pt_PinMPI_Bcast:
				cout << "MPI_BCAST " << arg0 << " " << hex << arg1 << dec << " " << arg2 << " " << arg3 << " " << hex << arg4 << "\n";
				break;
			case pt_PinMPI_Reduce:
				cout << "MPI_REDUCE " << arg0 << " " << hex << arg1 << dec << " " << arg2 << " " << arg3 << " " << hex << arg4 << " " << arg5 << dec << "\n";
				break;
			case pt_PinMPI_Allreduce:
				cout << "MPI_ALLREDUCE " << arg0 << " " << hex << arg1 << dec << " " << arg2 << " " << hex << arg3 << " " << arg4 << dec << "\n";
				break;
			case pt_PinMPI_Allgather:
				cout << "MPI_ALLGATHER " << arg0 << " " << hex << arg1 << dec << " " << arg2 << " " << hex << arg3 << " " << arg4 << dec << "\n";
				break;
			case pt_PinMPI_Alltoall:
				cout << "MPI_ALLTOALL " << arg0 << " " << hex << arg1 << dec << " " << arg2 << " " << hex << arg3 << " " << arg4 << dec << "\n";
				break;
			case pt_PinMPI_Alltoallv:
				cout << "MPI_ALLTOALLV " << arg0 << " " << hex << arg1 << dec << " " << arg2 << " " << hex << arg3 << " " << arg4 << dec << "\n";
				break;
			case pt_MPI_Send:
				trace.clearAllFlag();
				cout << "[0]: mpi_send start" << "\n";
				break;
			case pt_MPI_Recv:
				trace.clearAllFlag();
				cout << "[0]: mpi_recv start" << "\n";
				break;
			case pt_MPI_Isend:
				trace.clearAllFlag();
				cout << "[0]: mpi_isend start" << "\n";
				break;
			case pt_MPI_Irecv:
				trace.clearAllFlag();
				cout << "[0]: mpi_irecv start" << "\n";
				break;
			case pt_MPI_Wait:
				trace.clearAllFlag();
				cout << "[0]: mpi_wait start" << "\n";
				break;
			case pt_MPI_Barrier:
				trace.clearAllFlag();
				cout << "[0]: mpi_barrier start" << "\n";
				break;
			case pt_MPI_Bcast:
				trace.clearAllFlag();
				cout << "[0]: mpi_bcast start" << "\n";
				break;
			case pt_MPI_Reduce:
				trace.clearAllFlag();
				cout << "[0]: mpi_reduce start" << "\n";
				break;
			case pt_MPI_Allreduce:
				trace.clearAllFlag();
				cout << "[0]: mpi_allreduce start" << "\n";
				break;
			case pt_MPI_Allgather:
				trace.clearAllFlag();
				cout << "[0]: mpi_allgather start" << "\n";
				break;
			case pt_MPI_Alltoall:
				trace.clearAllFlag();
				cout << "[0]: mpi_alltoall start" << "\n";
				break;
			case pt_MPI_Alltoallv:
				trace.clearAllFlag();
				cout << "[0]: mpi_alltoallv start" << "\n";
				break;
			default:
				cout << "ASSERT::ERROR:: " <<  addr_instPoint[addr] << "\n";
				assert(0);
				break;
			}
		}
	}
}

VOID do_call_args_indirect(ADDRINT addr, ADDRINT target, BOOL taken, ADDRINT arg0, ADDRINT arg1, ADDRINT arg2, ADDRINT arg3, ADDRINT arg4, ADDRINT arg5) {
    if( !taken ) return;

    const string *s = Target2String(target);
    do_call_args(addr, s, arg0, arg1, arg2, arg3, arg4, arg5);

    if (s != &invalid)
        delete s;
}

/* ===================================================================== */

VOID InsLoad(ADDRINT addr, UINT32 size, THREADID tid) {
	if (global_flag && trace.getFlag(tid)) trace.addAccess(tid, 'r', addr, size);
}

VOID InsStore(ADDRINT addr, UINT32 size, THREADID tid) {
	if (global_flag && trace.getFlag(tid)) trace.addAccess(tid, 'w', addr, size);
}

/* ===================================================================== */

VOID Region_start(THREADID tid) {
	if (!global_flag) {
		if (!trace.getPhase()) {
			cout << "[" << tid << "]: Region_start" << std::endl;
			global_flag = true;
			trace.addPhase(tid);
			trace.setAllFlag();
		}
		else
			cout << "[" << tid << "]: Region_start Multiple Passes" << std::endl;
	}
	else
		assert(0);
}

VOID Region_end(THREADID tid) {
	if (global_flag) {
		trace.clearAllFlag();
		trace.clear(tid);
		global_flag = false;
		cout << "[" << tid << "]: Region_end" << std::endl;
	}
	else {
		cout << "[" << tid << "]: Region_end Multiple Passes" << std::endl;
	}
}

/* ===================================================================== */

VOID GOMP_parallel_start(THREADID tid) {
	if (global_flag) {
		trace.printStat(tid);
		trace.addPhase(tid);
		trace.clear(tid);
		trace.clearAllFlag();
		cout << "[" << tid << "]: GOMP_parallel_start" << std::endl;
	}
}

VOID GOMP_parallel_end(THREADID tid) {
	if (global_flag) {
		trace.printStat(tid);
		trace.addPhase(tid);
		trace.clear(tid);
		trace.setAllFlag();
		cout << "[" << tid << "]: GOMP_parallel_end" << std::endl;
	}
}

/* ===================================================================== */

VOID omp_fn_start(THREADID tid) {
	if (global_flag) {
		trace.setFlag(tid);
		cout << "[" << tid << "]: .omp_fn.x start" << std::endl;
	}
}

VOID omp_fn_end(THREADID tid) {
	if (global_flag) {
		cout << "[" << tid << "]: .omp_fn.x end" << std::endl;
		trace.clearFlag(tid);
	}
}

/* ===================================================================== */

VOID mpi_end(THREADID tid) {
	if (global_flag) {
		//trace.printStat(tid);
		//trace.addPhase(tid);
		//trace.clear(tid);
		trace.setAllFlag();
		cout << "[" << tid << "]: mpi_xxx end" << std::endl;
	}
}

/* ===================================================================== */

VOID Instruction(INS ins, void * v) {
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    if (addr_instPoint.find(INS_Address(ins))!=addr_instPoint.end()) {
		switch (addr_instPoint[INS_Address(ins)]) {
    	case pt_Region_start:
    		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) Region_start, IARG_THREAD_ID, IARG_END);
    		break;
    	case pt_Region_end:
    		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) Region_end, IARG_THREAD_ID, IARG_END);
    		break;
    	case pt_GOMP_parallel_start:
    		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) GOMP_parallel_start, IARG_THREAD_ID, IARG_END);
    		break;
    	case pt_GOMP_parallel_end:
    		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) GOMP_parallel_end, IARG_THREAD_ID, IARG_END);
    		break;
    	case pt_omp_fn_start:
    		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn_start, IARG_THREAD_ID, IARG_END);
    		break;
    	case pt_omp_fn_end:
    		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) omp_fn_end, IARG_THREAD_ID, IARG_END);
    		break;
    	case pt_PinMPI_Send:
    	case pt_PinMPI_Recv:
    	case pt_PinMPI_Barrier:
    	case pt_PinMPI_Bcast:
    	case pt_PinMPI_Reduce:
    	case pt_PinMPI_Allreduce:
    	case pt_PinMPI_Allgather:
    	case pt_PinMPI_Alltoall:
    	case pt_PinMPI_Alltoallv:
    	case pt_MPI_Send:
    	case pt_MPI_Recv:
    	case pt_MPI_Isend:
    	case pt_MPI_Irecv:
    	case pt_MPI_Wait:
    	case pt_MPI_Barrier:
    	case pt_MPI_Bcast:
    	case pt_MPI_Reduce:
    	case pt_MPI_Allreduce:
    	case pt_MPI_Allgather:
    	case pt_MPI_Alltoall:
    	case pt_MPI_Alltoallv:
    		break;
    	case pt_MPI_End:
    		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) mpi_end, IARG_THREAD_ID, IARG_END);
    		break;
    	default:
    		assert(0);
		}
    }

	if (KnobTarget.Value()=="mpi") {
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
	}

    for (UINT32 memOp=0; memOp<memOperands; memOp++) {
        const UINT32 size = INS_MemoryOperandSize(ins, memOp);

        if (INS_MemoryOperandIsRead(ins, memOp))
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) InsLoad, IARG_MEMORYOP_EA, memOp, IARG_UINT32, size, IARG_THREAD_ID, IARG_END);

        if (INS_MemoryOperandIsWritten(ins, memOp))
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) InsStore, IARG_MEMORYOP_EA, memOp, IARG_UINT32, size, IARG_THREAD_ID, IARG_END);
    }
}

VOID Fini(int code, VOID * v) {
    trace.releaseOutputFile();
	cout << "<Memory Foot Print: " << trace.getMemFootPrintSize() << ">\n";
	/*if (!trace.getOutput_direction()) {
		ofstream extraOutFile;
		extraOutFile.open("MemoryFootPrintPerThread.txt");
		if (!extraOutFile.is_open()) {
            cerr << "fail to open output file(MemoryFootPrintPerThread.txt)\n";
            return;
        }
		extraOutFile << "<Memory Foot Print Per Thread:>\n";
		for (unsigned int i=0; i<16; i++) {
			for (map <ADDRINT, int>::iterator I=trace.getMemFootPrintPerThreadBegin(i), E=trace.getMemFootPrintPerThreadEnd(i); I!=E; I++) {
				extraOutFile << i << " " << I->first << " " << I->second << "\n";
			}
		}
	}*/
}

int main(int argc, char *argv[]) {
    PIN_InitSymbols();
    if(PIN_Init(argc,argv)) {
        return Usage();
    }

    if (KnobTarget.Value()!="mpi" && KnobTarget.Value()!="omp") {
    	cerr << "target architecture is not setup.\n";
    	return 0;
    }
    if (KnobInputFile.Value()=="") {
    	cerr << "obj.dump file is not specified.\n";
    	return 0;
    }
    if (KnobTarget.Value()=="omp" && KnobOutputFile.Value()=="") {
    	cerr << "trace file is not specified.\n";
    	return 0;
    }
    if (KnobAddrRegionStart.Value()=="") {
    	cerr << "address of start point is not specified.\n";
    	return 0;
    }
    if (KnobAddrRegionEnd.Value()=="") {
    	cerr << "address of end point is not specified.\n";
    	return 0;
    }

    obj_dump.open(KnobInputFile.Value().c_str());
    if (!obj_dump.is_open()) {
    	cerr << "obj.dump file is not found.\n";
    	return 0;
    }

	addr_instPoint[string2hex((char *)KnobAddrRegionStart.Value().c_str())] = pt_Region_start;
	addr_instPoint[string2hex((char *)KnobAddrRegionEnd.Value().c_str())] = pt_Region_end;

	if (KnobTarget.Value()=="omp") {
		char line[256], line_bak[256];
		char *line_tok;
		int addr, addr_prev = 0;
		bool flag_GOMP_end = false, flag_omp_fn_start = false, flag_omp_fn_end = false;
		while (!obj_dump.eof()) {
			obj_dump.getline(line, 255);
			strcpy(line_bak, line);
			line_tok = strtok(line, ":");
			if (line_tok) {
				addr = string2hex(line_tok);
				if (addr) {
					if (flag_GOMP_end) {
						//cerr << hex << addr << dec << "\n";
						flag_GOMP_end = false;
						addr_instPoint[addr] = pt_GOMP_parallel_end;
					}
					if (flag_omp_fn_start) {
						//cerr << "omp_fn_start: " << hex << addr << dec << "\n";
						flag_omp_fn_start = false;
						addr_instPoint[addr] = pt_omp_fn_start;
					}
					line_tok = strtok(NULL, "\n");
					if (strstr(line_tok, "GOMP_parallel_start")) {
						//cerr << "GOMP_parallel_start: " << hex << addr << dec << "\n";
						addr_instPoint[addr] = pt_GOMP_parallel_start;
					}
					else if (strstr(line_tok, "GOMP_parallel_end")) {
						//cerr << "GOMP_parallel_end: ";
						flag_GOMP_end = true;
					}
					else if (strstr(line_tok, "ret") && flag_omp_fn_end) {
						//cerr << "omp_fn_end: " << hex << addr << dec << "\n";
						flag_omp_fn_end = false;
						addr_instPoint[addr] = pt_omp_fn_end;
					}
					addr_prev = addr;
				}
				else if (strstr(line_bak, ".omp_fn.")) {
					//cerr << line_bak << "\n";
					flag_omp_fn_start = true;
					flag_omp_fn_end = true;
				}
			}
			else {
				if (flag_omp_fn_end) {
					cerr << "WARNING: cannot find a return function\n";
					flag_omp_fn_end = false;
					//addr_instPoint[addr_prev] = pt_omp_fn_end;
				}
			}
		}
	    trace.setOutputFile(KnobOutputFile.Value().c_str());
	}
	else if (KnobTarget.Value()=="mpi") {
		char line[256], line_bak[256];
		char *line_tok;
		int addr, addr_prev = 0;
		bool flag_MPI_end = false;
		while (!obj_dump.eof()) {
			obj_dump.getline(line, 255);
			strcpy(line_bak, line);
			line_tok = strtok(line, ":");
			if (line_tok) {
				addr = string2hex(line_tok);
				if (addr) {
					if (flag_MPI_end) {
						flag_MPI_end = false;
						addr_instPoint[addr] = pt_MPI_End;
					}
					line_tok = strtok(NULL, "\n");
					if (strstr(line_tok, "call")) {
					if (strstr(line_tok, "<PinMPI_Send")) {
						addr_instPoint[addr] = pt_PinMPI_Send;
					}
					else if (strstr(line_tok, "<PinMPI_Recv")) {
						addr_instPoint[addr] = pt_PinMPI_Recv;
					}
					else if (strstr(line_tok, "<PinMPI_Barrier")) {
						addr_instPoint[addr] = pt_PinMPI_Barrier;
					}
					else if (strstr(line_tok, "<PinMPI_Bcast")) {
						addr_instPoint[addr] = pt_PinMPI_Bcast;
					}
					else if (strstr(line_tok, "<PinMPI_Reduce")) {
						addr_instPoint[addr] = pt_PinMPI_Reduce;
					}
					else if (strstr(line_tok, "<PinMPI_Allreduce")) {
						addr_instPoint[addr] = pt_PinMPI_Allreduce;
					}
					else if (strstr(line_tok, "<PinMPI_Allgather")) {
						addr_instPoint[addr] = pt_PinMPI_Allgather;
					}
					else if (strstr(line_tok, "<PinMPI_Alltoall")) {
						addr_instPoint[addr] = pt_PinMPI_Alltoall;
					}
					else if (strstr(line_tok, "<PinMPI_Alltoallv")) {
						addr_instPoint[addr] = pt_PinMPI_Alltoallv;
					}
					else if (strstr(line_tok, "<MPI_Isend") || strstr(line_tok, "<mpi_isend_")) {
						addr_instPoint[addr] = pt_MPI_Isend;
						flag_MPI_end = true;
					}
					else if (strstr(line_tok, "<MPI_Irecv") || strstr(line_tok, "<mpi_irecv_")) {
						addr_instPoint[addr] = pt_MPI_Irecv;
						flag_MPI_end = true;
					}
					else if (strstr(line_tok, "<MPI_Send") || strstr(line_tok, "<mpi_send_")) {
						addr_instPoint[addr] = pt_MPI_Send;
						flag_MPI_end = true;
					}
					else if (strstr(line_tok, "<MPI_Recv") || strstr(line_tok, "<mpi_recv_")) {
						addr_instPoint[addr] = pt_MPI_Recv;
						flag_MPI_end = true;
					}
					else if (strstr(line_tok, "<MPI_Wait") || strstr(line_tok, "<mpi_wait_") || strstr(line_tok, "<MPI_Waitall") || strstr(line_tok, "<mpi_waitall_")) {
						addr_instPoint[addr] = pt_MPI_Wait;
						flag_MPI_end = true;
					}
					else if (strstr(line_tok, "<MPI_Barrier") || strstr(line_tok, "<mpi_barrier_")) {
						addr_instPoint[addr] = pt_MPI_Barrier;
						flag_MPI_end = true;
					}
					else if (strstr(line_tok, "<MPI_Bcast") || strstr(line_tok, "<mpi_bcast_")) {
						addr_instPoint[addr] = pt_MPI_Bcast;
						flag_MPI_end = true;
					}
					else if (strstr(line_tok, "<MPI_Reduce") || strstr(line_tok, "<mpi_reduce_")) {
						addr_instPoint[addr] = pt_MPI_Reduce;
						flag_MPI_end = true;
					}
					else if (strstr(line_tok, "<MPI_Allreduce") || strstr(line_tok, "<mpi_allreduce_")) {
						addr_instPoint[addr] = pt_MPI_Allreduce;
						flag_MPI_end = true;
					}
					else if (strstr(line_tok, "<MPI_Allgather") || strstr(line_tok, "<mpi_allgather_")) {
						addr_instPoint[addr] = pt_MPI_Allgather;
						flag_MPI_end = true;
					}
					else if (strstr(line_tok, "<MPI_Alltoall") || strstr(line_tok, "<mpi_alltoall_")) {
						addr_instPoint[addr] = pt_MPI_Alltoall;
						flag_MPI_end = true;
					}
					else if (strstr(line_tok, "<MPI_Alltoallv") || strstr(line_tok, "<mpi_alltoallv_")) {
						addr_instPoint[addr] = pt_MPI_Alltoallv;
						flag_MPI_end = true;
					}
					}

					addr_prev = addr;
				}
			}
			else {
				if (flag_MPI_end) {
					cerr << "WARNING: cannot find the next address of MPI function\n";
					assert(0);
				}
			}
		}
		trace.setOutput_direction(true);
	}
	else
		assert(0);

    for (map<ADDRINT,t_instPoint>::iterator I=addr_instPoint.begin(), E=addr_instPoint.end(); I!=E; I++) {
    	switch (I->second) {
    	case pt_Region_start:
    		cerr << "Region_start: ";
    		break;
    	case pt_Region_end:
    		cerr << "Region_end: ";
    		break;
    	case pt_GOMP_parallel_start:
    		cerr << "GOMP_parallel_start: ";
    		break;
    	case pt_GOMP_parallel_end:
    		cerr << "GOMP_parallel_end: ";
    		break;
    	case pt_omp_fn_start:
    		cerr << "omp_fn_start: ";
    		break;
    	case pt_omp_fn_end:
    		cerr << "omp_fn_end: ";
    		break;
    	case pt_PinMPI_Send:
    		cerr << "mpi_send_point: ";
    		break;
    	case pt_PinMPI_Recv:
    		cerr << "mpi_recv_point: ";
    		break;
    	case pt_PinMPI_Barrier:
    		cerr << "mpi_barrier_point: ";
    		break;
    	case pt_PinMPI_Bcast:
    		cerr << "mpi_bcast_point: ";
    		break;
    	case pt_PinMPI_Reduce:
    		cerr << "mpi_reduce_point: ";
    		break;
    	case pt_PinMPI_Allreduce:
    		cerr << "mpi_allreduce_point: ";
    		break;
    	case pt_PinMPI_Allgather:
    		cerr << "mpi_allgather_point: ";
    		break;
    	case pt_PinMPI_Alltoall:
    		cerr << "mpi_alltoall_point: ";
    		break;
    	case pt_PinMPI_Alltoallv:
    		cerr << "mpi_alltoallv_point: ";
    		break;
    	case pt_MPI_Isend:
    		cerr << "mpi_isend_start: ";
    		break;
    	case pt_MPI_Irecv:
    		cerr << "mpi_irecv_start: ";
    		break;
    	case pt_MPI_Send:
    		cerr << "mpi_send_start: ";
    		break;
    	case pt_MPI_Recv:
    		cerr << "mpi_recv_start: ";
    		break;
    	case pt_MPI_Wait:
    		cerr << "mpi_wait_start: ";
    		break;
    	case pt_MPI_Barrier:
    		cerr << "mpi_barrier_start: ";
    		break;
    	case pt_MPI_Bcast:
    		cerr << "mpi_bcast_start: ";
    		break;
    	case pt_MPI_Reduce:
    		cerr << "mpi_reduce_start: ";
    		break;
    	case pt_MPI_Allreduce:
    		cerr << "mpi_allreduce_start: ";
    		break;
    	case pt_MPI_Allgather:
    		cerr << "mpi_allgather_start: ";
    		break;
    	case pt_MPI_Alltoall:
    		cerr << "mpi_alltoall_start: ";
    		break;
    	case pt_MPI_Alltoallv:
    		cerr << "mpi_alltoallv_start: ";
    		break;
    	case pt_MPI_End:
    		cerr << "mpi_xxx_end: ";
    		break;
    	default:
    		assert(0);
    	}
		cerr << hex << I->first << dec << "\n";
    }

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    PIN_StartProgram();

    return 0;
}
