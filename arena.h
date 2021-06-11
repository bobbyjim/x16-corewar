#ifndef _ARENA_H_
#define _ARENA_H_

#include "common.h"
#include "cell.h"

#define  CORESIZE    4056

Cell* getLocation(int x);
void setLocation(int x, Cell *copy);
void clearArena();
void drawArena();
void drawCell(int location);
void dumpArena(int start, int end);

#endif

