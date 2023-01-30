#!/bin/bash -e
#SBATCH --job-name="cpu16"
#SBATCH -t 00:10:00 -n 16

module load 2022
module load OpenMPI/4.1.4-GCC-11.3.0

make
echo "MPI parallelism"
echo

srun --mpi=pmi2 ./CGOL-mpi