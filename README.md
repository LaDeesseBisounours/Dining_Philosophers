# AUTHOR: LeDieuBisounours
# Dining_Philosophers
Dining philosophers Dijkstra using MPICH2 and C++
MPICH2 is required.
To run, in a terminal:
-PATH=$PATH:.
-make
-mpirun -n 5 program

For number given in parameter to mpirun, you can use any integer bigger than 2.
Take in consideration that this number represent the number of processes 
executed.
