/* File: glider.h */

//   # 
//    #
//  ###

#include "stdint.h"

#define GLIDER_HEIGHT (3)
#define GLIDER_WIDTH (3)

uint8_t glider[GLIDER_HEIGHT][GLIDER_WIDTH] = {
    {0, 1, 0},
    {0, 0, 1},
    {1, 1, 1},
};
