#ifndef _ARENA_H_
#define _ARENA_H_

#include "common.h"
#include "cell.h"
#include "bank.h"

#define     SET_BANK(pos)          POKE 0x9f61, 10 + int(pos/CELLS_PER_BANK)
#define     BANKED_CORE_MEMORY(n)  ((Cell *)0xA000)[n % CELLS_PER_BANK];
//
//  cc65 prefers we access this with array notation, like so:
//
//      BANKED_CORE_MEMORY[400];
//

void arena_init(unsigned char doRandomize);

Cell* arena_getLocation(int x);
void arena_setLocation(int x, Cell *copy);
void arena_clearLocation(int position, unsigned char doRandomize);

#endif
