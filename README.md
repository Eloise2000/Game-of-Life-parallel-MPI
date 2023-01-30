# Game-of-Life-parallel-MPI
An MPI implementation of Game of Life, with a speedup of 11.46 for 16 threads on the benchmark.
## Quick Start
- Compile the code: `make`
- Run the code with mpiexec (example with 3 nodes): `mpiexec -n 3 ./CGOL-mpi`
- `CGOL-mpi-sbatch.sh` can be used to submit jobs in supercomputers and computer clusters.

## Game of Life
Game of life is a classical example of the cellular automaton. Ba-
sically, the game starts with numerous dead nodes with the value
0, and each node has eight neighbours (horizontally, vertically, or
diagonally adjacent). The rules are:
1. Any live node with fewer than two live neighbours dies.
2. Any live node with more than three live neighbours dies.
3. Any live node with two or three live neighbours lives un-
changed to the next generation.
4. Any dead node with exactly three live neighbours will come
to life.

## Setup
The pattern of cell would be placed on a larger board, where cell (0,0) of the pattern would be at cell (1500,1500) of the board.

For all benchmark executions of the code the board is of size 3000x3000 and the 'grower' pattern in folder `game-of-life-test` is used. 

To verify the correctness, the 'grower' pattern should has a population of 49 at generation 10, and a population of 138 at generation 100.

Also the website https://golly.sourceforge.net can be used to visualize the result. The `.rle` files in `game-of-life-test` folder can be uploaded for visualization.