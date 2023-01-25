CFLAGS += -O3 -Wall -W --std=c11 -lm
CXXFLAGS += -O3 -Wall -W --std=c++11 -lm -Wno-cast-function-type
OMP_CFLAGS = $(CFLAGS) -fopenmp
# MPI_CFLAGS = $(CXXFLAGS) -lmpi
MPI_CFLAGS = $(CFLAGS) -lmpi

all: CGOL-mpi

CGOL-mpi: CGOL-mpi.c
	mpiCC $(MPI_CFLAGS) -o CGOL-mpi CGOL-mpi.c

clean:
	rm -f *.out