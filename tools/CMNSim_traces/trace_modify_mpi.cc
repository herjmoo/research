#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//#define DEBUG_FILE_READ

using namespace std;

unsigned long string2hex (char *);

int main(int argc, char *argv[]) {
    if(argv[1]==NULL){
    	cerr << "input format: \n";
    	cerr << "./trace_modify_mpi <pid> <trace_file> ...\n";
    	return 1;
    }

    string fileName, fileName_out, fileName_res;
    FILE *FilePointer, *FilePointer_out, *FilePointer_res;
    unsigned long i, j;
    int pID = atoi(argv[1]);
    fileName.assign(argv[2]);
    fileName_out = "m_" + fileName;
    fileName_res = fileName + ".res";
    
    FilePointer_out = fopen(fileName_out.c_str(), "w");
    if (FilePointer_out==NULL) {
        cerr << "File Open Error ()\n";
        exit(0);
    }

    FilePointer_res = fopen(fileName_res.c_str(), "w");
    if (FilePointer_res==NULL) {
        cerr << "File Open Error ()\n";
        exit(0);
    }

    FilePointer = fopen(fileName.c_str(), "r");
    if (FilePointer==NULL) {
        cerr << "File Open Error\n";
        exit(0);
    }

    string line_res;
    char line[100], line_d[100];
    char *line_twit;
    char rw;
    char *temp;
    char *addr;
    char *size;
    char *id_proc;
    unsigned long pid;
    unsigned long hex_addr;
    unsigned long byte;

    ostringstream ss;
    ss << pID;

    i = 0;
    while (fgets(line, sizeof(line), FilePointer)!=NULL) {
        if (line[0]=='0') {
#ifdef DEBUG_FILE_READ
    	cout << "LINE " << line << " => ";
    	id_proc = strtok(line," ");
    	pid = atoi(id_proc);
    	temp = strtok(NULL," ");
    	rw = *temp;
    	addr = strtok(NULL," ");
    	size = strtok(NULL," \n");
    	byte = atoi(size);
    	hex_addr = string2hex(addr);
    	cout << pid << " "  << rw << " "<< hex << hex_addr << " " << dec << byte << "\n";
#endif
        line_twit = &line_d[1];
            switch (pID) {
            case 0:              fputs(line, FilePointer_out); break;
            case 1: line[0]='1'; fputs(line, FilePointer_out); break;
            case 2: line[0]='2'; fputs(line, FilePointer_out); break;
            case 3: line[0]='3'; fputs(line, FilePointer_out); break;
            case 4: line[0]='4'; fputs(line, FilePointer_out); break;
            case 5: line[0]='5'; fputs(line, FilePointer_out); break;
            case 6: line[0]='6'; fputs(line, FilePointer_out); break;
            case 7: line[0]='7'; fputs(line, FilePointer_out); break;
            case 8: line[0]='8'; fputs(line, FilePointer_out); break;
            case 9: line[0]='9'; fputs(line, FilePointer_out); break;
            case 10: strcpy (line_twit, line); line_d[0]='1'; line_d[1]='0'; fputs(line_d, FilePointer_out); break;
            case 11: strcpy (line_twit, line); line_d[0]='1'; line_d[1]='1'; fputs(line_d, FilePointer_out); break;
            case 12: strcpy (line_twit, line); line_d[0]='1'; line_d[1]='2'; fputs(line_d, FilePointer_out); break;
            case 13: strcpy (line_twit, line); line_d[0]='1'; line_d[1]='3'; fputs(line_d, FilePointer_out); break;
            case 14: strcpy (line_twit, line); line_d[0]='1'; line_d[1]='4'; fputs(line_d, FilePointer_out); break;
            case 15: strcpy (line_twit, line); line_d[0]='1'; line_d[1]='5'; fputs(line_d, FilePointer_out); break;
            default: cout << line; break;
	    }
        }
        else if (!strncmp(line, "MPI_SEND", 8) || !strncmp(line, "MPI_RECV", 8) || !strncmp(line, "MPI_BARRIER", 11) || !strncmp(line, "MPI_BCAST", 9) || !strncmp(line, "MPI_REDUCE", 10) || !strncmp(line, "MPI_ALLREDUCE", 13) || !strncmp(line, "MPI_ALLGATHER", 13) || !strncmp(line, "MPI_ALLTOALL", 12) || !strncmp(line, "PHASE", 5)) {
    	    //cout << "DLine " << setw(8) << i << " => " << line;
            fputs(line, FilePointer_out);
            fputs(line, FilePointer_res);
        }
        else {
    	    //cout << "ULine " << setw(8) << i << " => " << line;
            fputs(line, FilePointer_res);
        }
        i++;
    }

    fclose(FilePointer);
    fclose(FilePointer_out);
    fclose(FilePointer_res);

    return 0;
}

unsigned long string2hex (char *addr) {
	int i;
	unsigned long inc = 1;
	int len = strlen(addr)-1;
	unsigned long dec_sum = 0;

	for(i=0; i<len+1; i++){
		switch(*(addr+len-i)){
		case '1':           dec_sum += 1*inc;  break;
		case '2':           dec_sum += 2*inc;  break;
		case '3':           dec_sum += 3*inc;  break;
		case '4':           dec_sum += 4*inc;  break;
		case '5':           dec_sum += 5*inc;  break;
		case '6':           dec_sum += 6*inc;  break;
		case '7':           dec_sum += 7*inc;  break;
		case '8':           dec_sum += 8*inc;  break;
		case '9':           dec_sum += 9*inc;  break;
		case 'a': case 'A': dec_sum += 10*inc; break;
		case 'b': case 'B': dec_sum += 11*inc; break;
		case 'c': case 'C': dec_sum += 12*inc; break;
		case 'd': case 'D': dec_sum += 13*inc; break;
		case 'e': case 'E': dec_sum += 14*inc; break;
		case 'f': case 'F': dec_sum += 15*inc; break;
		default: break;
		}
		inc = inc * 16;
	}
	return dec_sum;
}
