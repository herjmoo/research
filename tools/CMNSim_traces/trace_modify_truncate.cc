#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string>

//#define DEBUG_FILE_READ

using namespace std;

int main(int argc, char *argv[]) {
    if(argv[1]==NULL || argv[2]==NULL){
    	cerr << "input format: \n";
    	cerr << "./trace_modify <total number of cores> <trace_file>\n";
    	return 1;
    }

    string fileName;
    string *listFileName;
    int numCore = atoi(argv[1]);
    FILE **listFilePointer;
    FILE **listFilePointer_out;
    unsigned long i, j;

    fileName.assign(argv[2]);
    assert(numCore);
    listFileName = new string[numCore];
    listFilePointer = new FILE*[numCore];
    listFilePointer_out = new FILE*[numCore];

    for (i=0; i<numCore; i++) {
        ostringstream ss;
        ss << i;
        if (i<10) listFileName[i] = fileName + ".0" + ss.str();
        else listFileName[i] = fileName + "." + ss.str();
    }

    for (i=0; i<numCore; i++) {
        listFilePointer[i] = fopen(listFileName[i].c_str(), "r");
        if (listFilePointer[i]==NULL) {
            cerr << "File Open Error r(" << i << ")" << listFileName[i] << "\n";
            exit(0);
        }
    }

    for (i=0; i<numCore; i++) {
        listFileName[i] = "tr." + listFileName[i];
        listFilePointer_out[i] = fopen(listFileName[i].c_str(), "w");
        if (listFilePointer_out[i]==NULL) {
            cerr << "File Open Error w(" << i << ")" << listFileName[i] << "\n";
            exit(0);
        }
    }

    char line[100];

for (i=0; i<numCore; i++) {

    while (fgets(line, sizeof(line), listFilePointer[i])!=NULL) {
        if (!strncmp(line, "PHASE 3", 7)) {
            break;
        }
        fputs(line, listFilePointer_out[i]);
    }
}

    for (i=0; i<numCore; i++) {
        fclose(listFilePointer[i]);
        fclose(listFilePointer_out[i]);
    }

    return 0;
}

