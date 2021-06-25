#ifndef _ARENA_H_
#define _ARENA_H_

#include "common.h"
#include "cell.h"

#define  RAW_CELL(i)                arena[i % CORESIZE]   
#define  SET_CELL_B_VALUE(i,val)    arena[i % CORESIZE].B = val
#define  SET_CELL_B_MODE(i,mode)    arena[i % CORESIZE].bMode = mode

void arena_init(unsigned char doRandomize);

Cell* arena_getLocation(int x);
void arena_setLocation(int x, Cell *copy);
void arena_clearLocation(int position, unsigned char doRandomize);

#endif
