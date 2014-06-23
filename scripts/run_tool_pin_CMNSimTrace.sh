#!/bin/sh

#echo "Create OMP traces"
#export OMP_NUM_THREADS=16
#../../../tools/pin-2.13-62732-gcc.4.4.7-linux/pin -t ../../../tools/pin-2.13-62732-gcc.4.4.7-linux/source/tools/CMNSimTrace/obj-intel64/CMNSimTrace.so -tg omp -i is.A.x.obj.dump -o is.A.x.trace -rs 402604 -re 402712 -- ./is.A.x > res.txt


echo "Create MPI traces"
mpirun -np 16 -output-filename is.S.16 ../../../tools/pin-2.13-62732-gcc.4.4.7-linux/pin -t ../../../tools/pin-2.13-62732-gcc.4.4.7-linux/source/tools/CMNSimTrace/obj-intel64/CMNSimTrace.so -tg mpi -i is.S.16.obj.dump -rs 40424a -re 4042ae -- ./is.S.16
mpirun -np 16 -output-filename is.W.16 ../../../tools/pin-2.13-62732-gcc.4.4.7-linux/pin -t ../../../tools/pin-2.13-62732-gcc.4.4.7-linux/source/tools/CMNSimTrace/obj-intel64/CMNSimTrace.so -tg mpi -i is.W.16.obj.dump -rs 404257 -re 40433d -- ./is.W.16
mpirun -np 16 -output-filename is.A.16 ../../../tools/pin-2.13-62732-gcc.4.4.7-linux/pin -t ../../../tools/pin-2.13-62732-gcc.4.4.7-linux/source/tools/CMNSimTrace/obj-intel64/CMNSimTrace.so -tg mpi -i is.A.16.obj.dump -rs 404257 -re 40433d -- ./is.A.16
