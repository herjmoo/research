/*
 * main.cc
 *
 *  Created on: Mar 18, 2014
 *      Author: Joonmoo Huh
 */

#include "sim.h"

#ifdef OMP16
#elif MPI16
#else
#error "Unknown target architecture"
#endif

using namespace std;

int main(int argc, char *argv[]) {
	if (argv[1] == NULL) {
		cerr << "input format: \n";
		cerr << "./cmn_sim <trace_file> \n";
		return 1;
	}

	string fileName(argv[1]);

#ifdef OMP16
	Sim_16core_Shared sim;
	sim.doSim(fileName);
#endif

#ifdef MPI16
	Sim_16core_Message sim;
	sim.doSim(fileName);
#endif

	return 0;
}
