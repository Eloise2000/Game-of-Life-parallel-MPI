#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include "game-of-life-test/beehive.h"
#include "game-of-life-test/glider.h"
#include "game-of-life-test/grower.h"

#define GEN_LIMIT 5000
#define BOARD_ROWS 3000
#define BOARD_COLS 3000
#define BOARD_ROW_START 1500
#define BOARD_COL_START 1500

void perror_exit(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void evolve_inner(uint8_t **local, uint8_t **new, int width_local, int height_local, int *sum_cell)
{
    // Access the cells of the actual grid, leaving out the auxiliary cells
    // around the grid, yet, taking them into account for the calculations
    // Can first compute the inner matrix, while waiting for the outer to send and receive
    *sum_cell = 0;
    for (int x = 2; x <= height_local-1; x++)
    {
        for (int y = 2; y <= width_local-1; y++)
        {
            int neighbors = 0;

            // Add the value of each cell to neighbor's variable
            neighbors = local[x - 1][y - 1] + local[x - 1][y] +
                        local[x - 1][y + 1] + local[x][y - 1] +
                        local[x][y + 1] + local[x + 1][y - 1] +
                        local[x + 1][y] + local[x + 1][y + 1];

            // Determine if the current cell is going to be alive or not
            // 3 means that it has 3 neighbors 
            // 2 means that it has 2 neighbors 
            if (neighbors == 3 || (neighbors == 2 && (local[x][y] == 1))) {
                new[x][y] = 1;
                (*sum_cell) ++;
            }
            else
                new[x][y] = 0;
        }
    }
}

void evolve_outer(uint8_t **local, uint8_t **new, int width_local, int height_local, int *sum_cell)
{
    // Compute the outer layer after communication
    for (int x = 1; x <= height_local; x++)
    {
        int y = 1;
        int neighbors = 0;
        neighbors = local[x - 1][y - 1] + local[x - 1][y] +
                    local[x - 1][y + 1] + local[x][y - 1] +
                    local[x][y + 1] + local[x + 1][y - 1] +
                    local[x + 1][y] + local[x + 1][y + 1];

        if (neighbors == 3 || (neighbors == 2 && (local[x][y] == 1))) {
            new[x][y] = 1;
            (*sum_cell) ++;
        }
        else
            new[x][y] = 0;
    }
    if (width_local != 1) {
        for (int x = 1; x <= height_local; x++)
        {
            int y = width_local;
            int neighbors = 0;
            neighbors = local[x - 1][y - 1] + local[x - 1][y] +
                        local[x - 1][y + 1] + local[x][y - 1] +
                        local[x][y + 1] + local[x + 1][y - 1] +
                        local[x + 1][y] + local[x + 1][y + 1];

            if (neighbors == 3 || (neighbors == 2 && (local[x][y] == 1))) {
                new[x][y] = 1;
                (*sum_cell) ++;
            }
            else
                new[x][y] = 0;
        }
    }
    
    for (int y = 2; y <= width_local-1; y++)
    {
        int x = 1;
        int neighbors = 0;
        neighbors = local[x - 1][y - 1] + local[x - 1][y] +
                    local[x - 1][y + 1] + local[x][y - 1] +
                    local[x][y + 1] + local[x + 1][y - 1] +
                    local[x + 1][y] + local[x + 1][y + 1];

        if (neighbors == 3 || (neighbors == 2 && (local[x][y] == 1))) {
            new[x][y] = 1;
            (*sum_cell) ++;
        }
        else
            new[x][y] = 0;
    }
    if (height_local != 1) {
        for (int y = 2; y <= width_local-1; y++)
        {
            int x = height_local;
            int neighbors = 0;
            neighbors = local[x - 1][y - 1] + local[x - 1][y] +
                        local[x - 1][y + 1] + local[x][y - 1] +
                        local[x][y + 1] + local[x + 1][y - 1] +
                        local[x + 1][y] + local[x + 1][y + 1];

            if (neighbors == 3 || (neighbors == 2 && (local[x][y] == 1))) {
                new[x][y] = 1;
                (*sum_cell) ++;
            }
            else
                new[x][y] = 0;
        }
    }
}

void evolve(uint8_t **local, uint8_t **new, int width_local, int height_local, int *sum_cell)
{
    // Access the cells of the actual grid, leaving out the auxiliary cells
    // around the grid, yet, taking them into account for the calculations
    *sum_cell = 0;
    for (int x = 1; x <= height_local; x++)
    {
        for (int y = 1; y <= width_local; y++)
        {
            int neighbors = 0;

            // Add the value of each cell to neighbor's variable
            neighbors = local[x - 1][y - 1] + local[x - 1][y] +
                        local[x - 1][y + 1] + local[x][y - 1] +
                        local[x][y + 1] + local[x + 1][y - 1] +
                        local[x + 1][y] + local[x + 1][y + 1];

            // Determine if the current cell is going to be alive or not
            // 3 means that it has 3 neighbors 
            // 2 means that it has 2 neighbors 
            if (neighbors == 3 || (neighbors == 2 && (local[x][y] == 1))) {
                new[x][y] = 1;
                (*sum_cell) ++;
            }
            else
                new[x][y] = 0;
        }
    }
}

int main(int argc, char *argv[]) {
    int comm_sz;

    // Initialize the MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    MPI_Comm old_comm, new_comm;
    int ndims, reorder, periods[2], dim_size[2];

    old_comm = MPI_COMM_WORLD;
    ndims = 2; // 2D matrix/grid
    int cols = (int)sqrt(comm_sz);
    int rows = comm_sz / cols;
    dim_size[0] = rows; // number of rows
    dim_size[1] = cols; // number of columns
    periods[0] = 1;             // rows periodic (each column forms a ring)
    periods[1] = 1;             // columns periodic (each row forms a ring)
    reorder = 1;                // allows processes reordered for efficiency

    // Create a fully periodic, 2D Cartesian topology
    MPI_Cart_create(old_comm, ndims, dim_size, periods, reorder, &new_comm);
    int my_rank, coords[2];

    MPI_Comm_rank(new_comm, &my_rank);
    MPI_Cart_coords(new_comm, my_rank, ndims, coords);

    int width = BOARD_COLS;
    int height = BOARD_ROWS;
    // Allocate space for the board
    uint8_t **board = malloc((height + 2) * sizeof(uint8_t *));
    uint8_t *c = malloc((width + 2) * (height + 2) * sizeof(uint8_t));
    if (board == NULL || c == NULL)
        perror_exit("malloc: ");
    for (int i = 0; i < (height + 2); i++)
        board[i] = &c[i * (width + 2)];
    
    // Initialize the pattern according to the target specified
    // target could be: beehive, glider, grower
    int pattern_row = GROWER_HEIGHT;
    int pattern_col = GROWER_WIDTH;
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            if (i >= 1500 && i < BOARD_ROW_START+pattern_row && j >= 1500 && j < BOARD_COL_START+pattern_col)
                board[i][j] = grower[i-BOARD_ROW_START][j-BOARD_COL_START];
            else
                board[i][j] = 0;
        }
    }

    // Calculate the local dimensions for the local subarrays
    int height_local, width_local;
    height_local = height / rows;
    width_local = width / cols;

    // Calculate start indices of the local array
    int start_indices[2];
    start_indices[0] = coords[0] * height_local; 
    start_indices[1] = coords[1] * width_local;

    // printf("my rank is %d, my coords is (%d, %d).\n", my_rank, coords[0], coords[1]);

    // Allocate space in each instance for the local array(s)
    uint8_t **local = malloc((height_local + 2) * sizeof(uint8_t *));
    uint8_t *b = malloc((width_local + 2) * (height_local + 2) * sizeof(uint8_t));
    if (local == NULL || b == NULL)
        perror_exit("malloc: ");
    for (int i = 0; i < (height_local + 2); i++)
        local[i] = &b[i * (width_local + 2)];

    for (int x = 0; x < height_local + 2; x++) {
        for (int y = 0; y < width_local + 2; y++) {
            if (x == 0 || x == height_local+1 || y == 0 || y == width_local+1)
                local[x][y] = 0;
            else
                local[x][y] = board[start_indices[0]+x-1][start_indices[1]+y-1];
        }
    }

    // Allocate space for the new array which holds the next generation of the local grid
    uint8_t **new = malloc((height_local + 2) * sizeof(uint8_t *));
    uint8_t *a = malloc((width_local + 2) * (height_local + 2) * sizeof(uint8_t));
    if (new == NULL || a == NULL)
        perror_exit("malloc: ");
    for (int i = 0; i < (height_local + 2); i++)
        new[i] = &a[i * (width_local + 2)];

    for (int x = 0; x < height_local + 2; x++) {
        for (int y = 0; y < width_local + 2; y++) {
            new[x][y] = 0;
        }
    }

    // Calculating the coordinates of the neighbours, relative to the current process ones
    int north, south, east, west, north_west, north_east, south_west, south_east;

    int north_coords[2];
    int south_coords[2];
    int west_coords[2];
    int east_coords[2];

    north_coords[0] = coords[0] - 1;
    south_coords[0] = coords[0] + 1;
    west_coords[0] = coords[0];
    east_coords[0] = coords[0];

    north_coords[1] = coords[1];
    south_coords[1] = coords[1];
    west_coords[1] = coords[1] - 1;
    east_coords[1] = coords[1] + 1;

    int north_west_coords[2];
    int north_east_coords[2];
    int south_west_coords[2];
    int south_east_coords[2];

    north_west_coords[0] = coords[0] - 1;
    north_east_coords[0] = coords[0] - 1;
    south_west_coords[0] = coords[0] + 1;
    south_east_coords[0] = coords[0] + 1;

    north_west_coords[1] = coords[1] - 1;
    north_east_coords[1] = coords[1] + 1;
    south_west_coords[1] = coords[1] - 1;
    south_east_coords[1] = coords[1] + 1;

    // Get the rank of each direction
    MPI_Cart_rank(new_comm, north_coords, &north);
    MPI_Cart_rank(new_comm, south_coords, &south);
    MPI_Cart_rank(new_comm, west_coords, &west);
    MPI_Cart_rank(new_comm, east_coords, &east);

    MPI_Cart_rank(new_comm, north_west_coords, &north_west);
    MPI_Cart_rank(new_comm, north_east_coords, &north_east);
    MPI_Cart_rank(new_comm, south_west_coords, &south_west);
    MPI_Cart_rank(new_comm, south_east_coords, &south_east);

    // Vector datatype representing columns in an 2D array
    MPI_Datatype vertical_type;

    MPI_Type_vector(height_local, 1, width_local + 2, MPI_UINT8_T, &vertical_type);
    MPI_Type_commit(&vertical_type);

    MPI_Request requests_odd[16];
    MPI_Request requests_even[16];

    // Communication requests to exchange data from local array in non-blocking way
    MPI_Recv_init(&local[0][1], width_local, MPI_CHAR, north, 1, new_comm, &requests_odd[0]);
    MPI_Send_init(&local[1][1], width_local, MPI_CHAR, north, 2, new_comm, &requests_odd[1]);
    MPI_Recv_init(&local[height_local + 1][1], width_local, MPI_CHAR, south, 2, new_comm, &requests_odd[2]);
    MPI_Send_init(&local[height_local][1], width_local, MPI_CHAR, south, 1, new_comm, &requests_odd[3]);

    MPI_Recv_init(&local[1][width_local + 1], 1, vertical_type, east, 3, new_comm, &requests_odd[4]);
    MPI_Send_init(&local[1][width_local], 1, vertical_type, east, 4, new_comm, &requests_odd[5]);
    MPI_Recv_init(&local[1][0], 1, vertical_type, west, 4, new_comm, &requests_odd[6]);
    MPI_Send_init(&local[1][1], 1, vertical_type, west, 3, new_comm, &requests_odd[7]);

    MPI_Recv_init(&local[0][0], 1, MPI_CHAR, north_west, 5, new_comm, &requests_odd[8]);
    MPI_Send_init(&local[1][1], 1, MPI_CHAR, north_west, 6, new_comm, &requests_odd[9]);
    MPI_Recv_init(&local[0][width_local + 1], 1, MPI_CHAR, north_east, 7, new_comm, &requests_odd[10]);
    MPI_Send_init(&local[1][width_local], 1, MPI_CHAR, north_east, 8, new_comm, &requests_odd[11]);

    MPI_Recv_init(&local[height_local + 1][0], 1, MPI_CHAR, south_west, 8, new_comm, &requests_odd[12]);
    MPI_Send_init(&local[height_local][1], 1, MPI_CHAR, south_west, 7, new_comm, &requests_odd[13]);
    MPI_Recv_init(&local[height_local + 1][width_local + 1], 1, MPI_CHAR, south_east, 6, new_comm, &requests_odd[14]);
    MPI_Send_init(&local[height_local][width_local], 1, MPI_CHAR, south_east, 5, new_comm, &requests_odd[15]);
    
    MPI_Recv_init(&new[0][1], width_local, MPI_CHAR, north, 1, new_comm, &requests_even[0]);
    MPI_Send_init(&new[1][1], width_local, MPI_CHAR, north, 2, new_comm, &requests_even[1]);
    MPI_Recv_init(&new[height_local + 1][1], width_local, MPI_CHAR, south, 2, new_comm, &requests_even[2]);
    MPI_Send_init(&new[height_local][1], width_local, MPI_CHAR, south, 1, new_comm, &requests_even[3]);

    MPI_Recv_init(&new[1][width_local + 1], 1, vertical_type, east, 3, new_comm, &requests_even[4]);
    MPI_Send_init(&new[1][width_local], 1, vertical_type, east, 4, new_comm, &requests_even[5]);
    MPI_Recv_init(&new[1][0], 1, vertical_type, west, 4, new_comm, &requests_even[6]);
    MPI_Send_init(&new[1][1], 1, vertical_type, west, 3, new_comm, &requests_even[7]);

    MPI_Recv_init(&new[0][0], 1, MPI_CHAR, north_west, 5, new_comm, &requests_even[8]);
    MPI_Send_init(&new[1][1], 1, MPI_CHAR, north_west, 6, new_comm, &requests_even[9]);
    MPI_Recv_init(&new[0][width_local + 1], 1, MPI_CHAR, north_east, 7, new_comm, &requests_even[10]);
    MPI_Send_init(&new[1][width_local], 1, MPI_CHAR, north_east, 8, new_comm, &requests_even[11]);

    MPI_Recv_init(&new[height_local + 1][0], 1, MPI_CHAR, south_west, 8, new_comm, &requests_even[12]);
    MPI_Send_init(&new[height_local][1], 1, MPI_CHAR, south_west, 7, new_comm, &requests_even[13]);
    MPI_Recv_init(&new[height_local + 1][width_local + 1], 1, MPI_CHAR, south_east, 6, new_comm, &requests_even[14]);
    MPI_Send_init(&new[height_local][width_local], 1, MPI_CHAR, south_east, 5, new_comm, &requests_even[15]);

    int generation = 1;
    double t_start = MPI_Wtime();

    // The actual loop of Game of Life
    while (generation <= GEN_LIMIT) {
        // Different requests for odd and even generations in order to compensate the pointer swap of local and new arrays
        int local_sum, global_sum;

        if ((generation % 2) == 1)
        {
            MPI_Startall(16, requests_odd);
            evolve_inner(local, new, width_local, height_local, &local_sum);
            MPI_Waitall(16, requests_odd, MPI_STATUSES_IGNORE);
        }
        else
        {
            MPI_Startall(16, requests_even);
            evolve_inner(local, new, width_local, height_local, &local_sum);
            MPI_Waitall(16, requests_even, MPI_STATUSES_IGNORE);
        }

        for (int j=0; j<width_local+2; j++) {
            if (coords[0] == 0) {
                local[0][j] = 0;
            }
            if (coords[0] == rows-1) {
                local[height_local+1][j] = 0;
            }
        }
        for (int i=0; i<height_local+2; i++) {
            if (coords[1] == 0) {
                local[i][0] = 0;
            }
            if (coords[1] == cols-1) {
                local[i][width_local+1] = 0;
            }
        }

        // evolve(local, new, width_local, height_local, &local_sum);
        evolve_outer(local, new, width_local, height_local, &local_sum);
        MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if (my_rank == 0) {
            printf("Generation %d: sum of cell is %d.\n", generation, global_sum);
        }

        // The pointer swap
        uint8_t **temp_array = local;
        local = new;
        new = temp_array;

        generation++;
    } // end of while loop

    double msecs = (MPI_Wtime() - t_start) * 1000;

    if (my_rank == 0) // If I am the master instance
        printf("Generations:\t%d\nExecution time:\t%.2lf msecs\n", GEN_LIMIT, msecs);

    // Deallocate space no longer needed
    free(a);
    free(new);
    a = NULL;
    new = NULL;

    free(b);
    free(local);
    b = NULL;
    local = NULL;

    free(c);
    free(board);
    c = NULL;
    board = NULL;

    MPI_Finalize();
}