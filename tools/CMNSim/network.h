/*
 * network.h
 *
 *  Created on: Mar 18, 2014
 *      Author: Joonmoo Huh
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <stdlib.h>

typedef enum mpiKind { kd_NA, kd_MPI_Send, kd_MPI_Recv, kd_MPI_Barrier, kd_MPI_Bcast, kd_MPI_Reduce, kd_MPI_Allreduce, kd_MPI_Allgather, kd_MPI_Alltoall } t_mpiKind;

class Noc_4X4_Mesh {
private:
	int **dist;
	int **shortest;

public:
	Noc_4X4_Mesh() {
		dist = new int *[16];
		for (int i = 0; i < 16; i++)
			dist[i] = new int[16];
		for (int i = 0; i < 16; i++)
			for (int j = 0; j < 16; j++)
				dist[i][j] = abs(i % 4 - j % 4) + abs(i / 4 - j / 4);
		shortest = new int *[16];
		for (int i = 0; i < 16; i++)
			shortest[i] = new int[15];
		shortest[ 0][0]= 1; shortest[ 0][1]= 4; shortest[ 0][2]= 5; shortest[ 0][3]= 2; shortest[ 0][4]= 8; shortest[ 0][5]= 3; shortest[ 0][6]= 6; shortest[ 0][7]= 9; shortest[ 0][8]=12; shortest[ 0][9]= 7; shortest[ 0][10]=13; shortest[ 0][11]=10; shortest[ 0][12]=11; shortest[ 0][13]=14; shortest[ 0][14]=15;
		shortest[ 1][0]= 0; shortest[ 1][1]= 5; shortest[ 1][2]= 2; shortest[ 1][3]= 4; shortest[ 1][4]= 3; shortest[ 1][5]= 6; shortest[ 1][6]= 9; shortest[ 1][7]= 7; shortest[ 1][8]= 8; shortest[ 1][9]=13; shortest[ 1][10]=10; shortest[ 1][11]=12; shortest[ 1][12]=11; shortest[ 1][13]=14; shortest[ 1][14]=15;
		shortest[ 2][0]= 3; shortest[ 2][1]= 6; shortest[ 2][2]= 1; shortest[ 2][3]= 7; shortest[ 2][4]= 0; shortest[ 2][5]= 5; shortest[ 2][6]=10; shortest[ 2][7]= 4; shortest[ 2][8]=11; shortest[ 2][9]=14; shortest[ 2][10]= 9; shortest[ 2][11]=15; shortest[ 2][12]= 8; shortest[ 2][13]=13; shortest[ 2][14]=12;
		shortest[ 3][0]= 2; shortest[ 3][1]= 7; shortest[ 3][2]= 6; shortest[ 3][3]= 1; shortest[ 3][4]=11; shortest[ 3][5]= 0; shortest[ 3][6]= 5; shortest[ 3][7]=10; shortest[ 3][8]=15; shortest[ 3][9]= 4; shortest[ 3][10]=14; shortest[ 3][11]= 9; shortest[ 3][12]= 8; shortest[ 3][13]=13; shortest[ 3][14]=12;
		shortest[ 4][0]= 5; shortest[ 4][1]= 0; shortest[ 4][2]= 8; shortest[ 4][3]= 1; shortest[ 4][4]= 6; shortest[ 4][5]= 9; shortest[ 4][6]=12; shortest[ 4][7]= 7; shortest[ 4][8]= 2; shortest[ 4][9]=13; shortest[ 4][10]=10; shortest[ 4][11]= 3; shortest[ 4][12]=11; shortest[ 4][13]=14; shortest[ 4][14]=15;
		shortest[ 5][0]= 4; shortest[ 5][1]= 1; shortest[ 5][2]= 6; shortest[ 5][3]= 9; shortest[ 5][4]= 0; shortest[ 5][5]= 7; shortest[ 5][6]= 2; shortest[ 5][7]= 8; shortest[ 5][8]=13; shortest[ 5][9]=10; shortest[ 5][10]= 3; shortest[ 5][11]=12; shortest[ 5][12]=11; shortest[ 5][13]=14; shortest[ 5][14]=15;
		shortest[ 6][0]= 7; shortest[ 6][1]= 2; shortest[ 6][2]= 5; shortest[ 6][3]=10; shortest[ 6][4]= 3; shortest[ 6][5]= 1; shortest[ 6][6]= 4; shortest[ 6][7]=11; shortest[ 6][8]=14; shortest[ 6][9]= 9; shortest[ 6][10]= 0; shortest[ 6][11]=15; shortest[ 6][12]= 8; shortest[ 6][13]=13; shortest[ 6][14]=12;
		shortest[ 7][0]= 6; shortest[ 7][1]= 3; shortest[ 7][2]=11; shortest[ 7][3]= 2; shortest[ 7][4]= 5; shortest[ 7][5]=10; shortest[ 7][6]=15; shortest[ 7][7]= 4; shortest[ 7][8]= 1; shortest[ 7][9]=14; shortest[ 7][10]= 9; shortest[ 7][11]= 0; shortest[ 7][12]= 8; shortest[ 7][13]=13; shortest[ 7][14]=12;
		shortest[ 8][0]= 9; shortest[ 8][1]=12; shortest[ 8][2]= 4; shortest[ 8][3]=13; shortest[ 8][4]=10; shortest[ 8][5]= 5; shortest[ 8][6]= 0; shortest[ 8][7]= 1; shortest[ 8][8]=11; shortest[ 8][9]=14; shortest[ 8][10]= 6; shortest[ 8][11]=15; shortest[ 8][12]= 7; shortest[ 8][13]= 2; shortest[ 8][14]= 3;
		shortest[ 9][0]= 8; shortest[ 9][1]=13; shortest[ 9][2]=10; shortest[ 9][3]= 5; shortest[ 9][4]=12; shortest[ 9][5]=14; shortest[ 9][6]=11; shortest[ 9][7]= 4; shortest[ 9][8]= 1; shortest[ 9][9]= 6; shortest[ 9][10]=15; shortest[ 9][11]= 0; shortest[ 9][12]= 7; shortest[ 9][13]= 2; shortest[ 9][14]= 3;
		shortest[10][0]=11; shortest[10][1]=14; shortest[10][2]= 9; shortest[10][3]= 6; shortest[10][4]=15; shortest[10][5]=13; shortest[10][6]= 8; shortest[10][7]= 7; shortest[10][8]= 2; shortest[10][9]= 5; shortest[10][10]=12; shortest[10][11]= 3; shortest[10][12]= 4; shortest[10][13]= 1; shortest[10][14]= 0;
		shortest[11][0]=10; shortest[11][1]=15; shortest[11][2]= 7; shortest[11][3]=14; shortest[11][4]= 9; shortest[11][5]= 6; shortest[11][6]= 3; shortest[11][7]=13; shortest[11][8]= 8; shortest[11][9]= 2; shortest[11][10]= 5; shortest[11][11]=12; shortest[11][12]= 4; shortest[11][13]= 1; shortest[11][14]= 0;
		shortest[12][0]=13; shortest[12][1]= 8; shortest[12][2]= 9; shortest[12][3]=14; shortest[12][4]= 4; shortest[12][5]=15; shortest[12][6]=10; shortest[12][7]= 5; shortest[12][8]= 0; shortest[12][9]=11; shortest[12][10]= 1; shortest[12][11]= 6; shortest[12][12]= 7; shortest[12][13]= 2; shortest[12][14]= 3;
		shortest[13][0]=12; shortest[13][1]= 9; shortest[13][2]=14; shortest[13][3]= 8; shortest[13][4]=15; shortest[13][5]=10; shortest[13][6]= 5; shortest[13][7]=11; shortest[13][8]= 4; shortest[13][9]= 1; shortest[13][10]= 6; shortest[13][11]= 0; shortest[13][12]= 7; shortest[13][13]= 2; shortest[13][14]= 3;
		shortest[14][0]=15; shortest[14][1]=10; shortest[14][2]=13; shortest[14][3]=11; shortest[14][4]=12; shortest[14][5]= 9; shortest[14][6]= 6; shortest[14][7]= 8; shortest[14][8]= 7; shortest[14][9]= 2; shortest[14][10]= 5; shortest[14][11]= 3; shortest[14][12]= 4; shortest[14][13]= 1; shortest[14][14]= 0;
		shortest[15][0]=14; shortest[15][1]=11; shortest[15][2]=10; shortest[15][3]=13; shortest[15][4]= 7; shortest[15][5]=12; shortest[15][6]= 9; shortest[15][7]= 6; shortest[15][8]= 3; shortest[15][9]= 8; shortest[15][10]= 2; shortest[15][11]= 5; shortest[15][12]= 4; shortest[15][13]= 1; shortest[15][14]= 0;
	}
	~Noc_4X4_Mesh() {
		for (int i = 0; i < 16; i++)
			delete dist[i];
		delete dist;
	}

	unsigned long getDistance(unsigned long from, unsigned long to) {
		return (unsigned long) dist[from][to];
	}
	unsigned long getShortest(unsigned long from, unsigned long involveCores) {
		unsigned long i;
		for (i = 0; i < 15; i++)
			if (involveCores & ((unsigned long) 0x8000 >> shortest[from][i]))
				break;
		if (i == 15)
			return from;
		else
			return shortest[from][i];
	}
};

class MPI_Control {
private:
	unsigned int procs;
	t_mpiKind *readyMessage;

	void **busy;
	unsigned long *busyCycles;
	bool *end;
	unsigned long *endCycles;
	unsigned long latency;

public:
	MPI_Control(unsigned int argProcs) {
		procs = argProcs;
		readyMessage = new t_mpiKind[procs];
		busy = new void *[procs];
		busyCycles = new unsigned long[procs];
		end = new bool[procs];
		endCycles = new unsigned long[procs];
		for (unsigned int i=0; i<procs; i++) {
			readyMessage[i] = kd_NA;
			busy[i] = NULL;
			busyCycles[i] = 0;
			end[i] = false;
			endCycles[i] = 0;
		}
		latency = 3;
	}
	~MPI_Control() {
	}

	void set_busy(unsigned int argProc, void *argReqAddr) {
		busy[argProc] = argReqAddr;
		busyCycles[argProc] = latency;
		end[argProc] = false;
	}
	void clear_busy(unsigned int argProc) {
		busy[argProc] = NULL;
		readyMessage[argProc] = kd_NA;
	}
	void set_end(unsigned int argProc, unsigned long argCycle) {
		end[argProc] = true;
		endCycles[argProc] = argCycle;
	}
	void clear_end(unsigned int argProc) {
		end[argProc] = false;
	}
	void simLatency() {
		for (unsigned int i=0; i<procs; i++) {
			if (busyCycles[i])
				busyCycles[i]--;
			if (endCycles[i])
				endCycles[i]--;
		}
	}

	void setReadyMessage (unsigned int argProc, t_mpiKind argMpiKind) {
		readyMessage[argProc] = argMpiKind;
	}

	unsigned long getBusyCycle(unsigned int argProc) {
		return busyCycles[argProc];
	}
	void *getReqAddr(unsigned int argProc) {
		return busy[argProc];
	}
	bool isBusy(unsigned int argProc) {
		return (busy[argProc] != 0);
	}
	unsigned long getEndCycle(unsigned int argProc) {
		return endCycles[argProc];
	}
	bool isEnd(unsigned int argProc) {
		return end[argProc];
	}

	t_mpiKind getReadyMessage (unsigned int argProc) {
		return readyMessage[argProc];
	}
};

#endif /* NETWORK_H_ */
