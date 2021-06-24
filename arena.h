#ifndef _ARENA_H_
#define _ARENA_H_

#include "common.h"
#include "cell.h"

/*
    Thoughts on some core sizes.

    78 cols x 52 rows = 4056.

 */

#define  CORESIZE   4056

#define  RAW_CELL(i)                arena[i % CORESIZE]   
#define  SET_CELL_B(i,val)          arena[i % CORESIZE].B = val
#define  SET_CELL_B_MODE(i,mode)    arena[i % CORESIZE].bMode = mode

typedef struct {
    unsigned int status : 8;
} System;

void arena_init(unsigned char doRandomize);

char arena_getCellChar(int ip);
Cell* getLocation(int x);
void setLocation(int x, Cell *copy);
void clearLocation(int position, unsigned char doRandomize);
void drawCell(int location);

#endif
