/* File: beehive.h */

//   ## 
//  #  #
//   ## 

#include "stdint.h"

#define BEEHIVE_HEIGHT (3)
#define BEEHIVE_WIDTH (4)

uint8_t beehive[BEEHIVE_HEIGHT][BEEHIVE_WIDTH] = {
    {0, 1, 1, 0},
    {1, 0, 0, 1},
    {0, 1, 1, 0},
};
