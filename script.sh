#!/bin/bash
#
FILENAME=test_randbmp1000.bmp
#launch 1 thread
mpirun --host mya:2 ./run_rel $FILENAME
echo "One thread active"

echo "Two threads active"
mpirun --host mya:1,myb:1,myc:1 ./run_rel $FILENAME
#
#echo "Three threads active"
#mpirun --host mya:2,myb:1,myc:1 ./run_rel $FILENAME


echo "Four threads active"
mpirun --host mya:1,myb:2,myc:2 ./run_rel $FILENAME

for THREADS in 2 4 6 8
do
	echo "Threads: $((THREADS*3))"
	mpirun --host mya:$((THREADS+1)),myb:$THREADS,myc:$THREADS ./run_rel $FILENAME
done
