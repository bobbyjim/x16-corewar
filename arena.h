#ifndef _ARENA_H_
#define _ARENA_H_

#include "common.h"
#include "cell.h"
#include "bank.h"

#ifdef X16
#define     CORE_MEMORY(n)  ((Cell *)0xA000)[n % CELLS_PER_BANK];
#else 
#define     CORE_MEMORY(n)   arena[ n % CORESIZE ];
#endif

void arena_init(unsigned char doRandomize);

Cell* arena_getLocation(int x);
void arena_setData(int target, int source);
void arena_setLocation(int target, Cell *copy);
void arena_clearLocation(int position, unsigned char doRandomize);

#endif
