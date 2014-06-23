#!/bin/sh

for i in {0..15}
do
if [ $i -lt 10 ]; then
../../trace_modify_mpi 0$i $1'0'$i
else
../../trace_modify_mpi $i $1$i
fi
done
