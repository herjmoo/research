#!/bin/sh

echo "Simulation OMP"
perl -pi -e 's/^MACRO.*/MACRO = -O3 -DOMP16/g' ./Makefile
perl -pi -e 's/^Cache_MESI.*/Cache_MESI(i, 262144, 64, 4, 10);/g' ../CMNSim/sim.h
make clean; make
./cmn_sim ../CMNSim_traces/NPB3.3/$1.omp.16/$1.x.trace > result.$1.omp.16.O3.32_256.txt
perl -pi -e 's/^Cache_MESI.*/Cache_MESI(i, 1048576, 64, 4, 10);/g' ../CMNSim/sim.h
make clean; make
./cmn_sim ../CMNSim_traces/NPB3.3/$1.omp.16/$1.x.trace > result.$1.omp.16.O3.32_1024.txt
perl -pi -e 's/^Cache_MESI.*/Cache_MESI(i, 8388608, 64, 4, 10);/g' ../CMNSim/sim.h
make clean; make
./cmn_sim ../CMNSim_traces/NPB3.3/$1.omp.16/$1.x.trace > result.$1.omp.16.O3.32_8192.txt

echo "Simulation MPI"
perl -pi -e 's/^MACRO.*/MACRO = -O3 -DMPI16/g' ./Makefile
perl -pi -e 's/^Cache_WB_SCL.*/Cache_WB_SCL(i, 262144, 64, 4, 10);/g' ../CMNSim/sim.h
make clean; make
./cmn_sim ../CMNSim_traces/NPB3.3/$1.mpi.16/m_$1.16.1 > result.$1.mpi.16.O3.32_256.txt
perl -pi -e 's/^Cache_WB_SCL.*/Cache_WB_SCL(i, 1048576, 64, 4, 10);/g' ../CMNSim/sim.h
make clean; make
./cmn_sim ../CMNSim_traces/NPB3.3/$1.mpi.16/m_$1.16.1 > result.$1.mpi.16.O3.32_1024.txt
perl -pi -e 's/^Cache_WB_SCL.*/Cache_WB_SCL(i, 8388608, 64, 4, 10);/g' ../CMNSim/sim.h
make clean; make
./cmn_sim ../CMNSim_traces/NPB3.3/$1.mpi.16/m_$1.16.1 > result.$1.mpi.16.O3.32_8192.txt

echo "Simulation MPI c2c"
perl -pi -e 's/^MACRO.*/MACRO = -O3 -DMPI16 -DCTOC/g' ./Makefile
perl -pi -e 's/^Cache_WB_SCL.*/Cache_WB_SCL(i, 262144, 64, 4, 10);/g' ../CMNSim/sim.h
make clean; make
./cmn_sim ../CMNSim_traces/NPB3.3/$1.mpi.16/m_$1.16.1 > result.$1.mpi_c2c.16.O3.32_256.txt
perl -pi -e 's/^Cache_WB_SCL.*/Cache_WB_SCL(i, 1048576, 64, 4, 10);/g' ../CMNSim/sim.h
make clean; make
./cmn_sim ../CMNSim_traces/NPB3.3/$1.mpi.16/m_$1.16.1 > result.$1.mpi_c2c.16.O3.32_1024.txt
perl -pi -e 's/^Cache_WB_SCL.*/Cache_WB_SCL(i, 8388608, 64, 4, 10);/g' ../CMNSim/sim.h
make clean; make
./cmn_sim ../CMNSim_traces/NPB3.3/$1.mpi.16/m_$1.16.1 > result.$1.mpi_c2c.16.O3.32_8192.txt



