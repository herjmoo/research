#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//#define DEBUG_FILE_READ

using namespace std;

unsigned long string2hex (char *);

int main(int argc, char *argv[]) {
    if(argv[1]==NULL || argv[2]==NULL){
    	cerr << "input format: \n";
    	cerr << "./trace_modify <total number of cores> <trace_file> ...\n";
    	return 1;
    }

    string *fileName;
    string *listFileName;
    int numCore = atoi(argv[1]);
    FILE *FilePointer;
    FILE **listFilePointer;
    unsigned long i, j;

    fileName = new string[argc-2];
    for (i=0; i<argc-2; i++)
        fileName[i].assign(argv[i+2]);
    assert(numCore);
    listFileName = new string[numCore];
    listFilePointer = new FILE*[numCore];

    for (i=0; i<numCore; i++) {
        ostringstream ss;
        ss << i;
        if (i<10) listFileName[i] = fileName[0] + ".0" + ss.str();
        else listFileName[i] = fileName[0] + "." + ss.str();
    }

    for (i=0; i<numCore; i++) {
        listFilePointer[i] = fopen(listFileName[i].c_str(), "w");
        if (listFilePointer[i]==NULL) {
            cerr << "File Open Error (" << i << ")\n";
            exit(0);
        }
    }

for (j=0; j<argc-2; j++) {
    FilePointer = fopen(fileName[j].c_str(), "r");
    if (FilePointer==NULL) {
        cerr << "File Open Error\n";
        exit(0);
    }

    char line[100], line_d[100];
    char rw;
    char *temp;
    char *addr;
    char *size;
    char *id_proc;
    unsigned long pid;
    unsigned long hex_addr;
    unsigned long byte;

    for (i=0; i<numCore; i++) {
        ostringstream ss;
        ss << j;
        string ln = "PHASE " + ss.str() + "\n";
        fputs(ln.c_str(), listFilePointer[i]);
    }

    while (fgets(line, sizeof(line), FilePointer)!=NULL) {
        strcpy(line_d, line);
#ifdef DEBUG_FILE_READ
    	cout << "LINE " << line << " =>  ";
#endif 
    	id_proc = strtok(line," ");
    	pid = atoi(id_proc);
#ifdef DEBUG_FILE_READ
    	temp = strtok(NULL," ");
    	rw = *temp;
    	addr = strtok(NULL," ");
    	size = strtok(NULL," \n");
    	byte = atoi(size);
    	hex_addr = string2hex(addr);
    	cout << pid << " "  << rw << " "<< hex << hex_addr << " " << dec << byte << "\n";
#endif 
        fputs(line_d, listFilePointer[pid]);
    }

    fclose(FilePointer);
}

    for (i=0; i<numCore; i++)
        fclose(listFilePointer[i]);

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
