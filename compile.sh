source /etc/profile.d/modules.sh
module load mpi/mpich-$(uname -m)
mpicxx -o run_rel main.cpp Fano.cpp
mv run_rel ../vm_shared/run_rel