#!/bin/bash -e
#SBATCH -t 30:00 -n 16 --mem=100M

make
echo "MPI parallelism"
echo

for ncores in {1..24}

do
  export OMP_NUM_THREADS=$ncores

  echo "CPUS: " $OMP_NUM_THREADS
  srun --mpi=pmi2 ./CGOL-mpi
  echo "DONE "
done