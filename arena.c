#include <stdlib.h>

#include "common.h"
#include "arena.h"
#include "cell.h"

#define  OPCODE(pos) (arena[pos % CORESIZE].opcode)

Cell arena[CORESIZE];
unsigned char corewar_system_status;

char cellChar[16] = {
   '.',     // hcf
   95,      // mov
   '+',     // add
   '-',     // sub
   94,      // jmp
   172,     // jmn
   187,     // jmz
   188,     // seq
   190,     // slt
   195,     // sne
   195,     // flp
   216,     // djn
   '?',     // 
   '?',     // 
   215,     // xch
   172      // spl
};

void clearLocation(int position, unsigned char doRandomize)
{
   // doRandomize:
   //
   // Clear with some random numbers.
   // This gives us RND without wasting
   // an opcode -- if we want it.
   //
   // NOTE.  If you want to give hints
   // about the coresize, just use rand().
   // Otherwise, use something like 
   //     rand() % 256, or
   //     65 + rand() % 26

    Cell *tgt = &arena[ position % CORESIZE ];
   tgt->opcode = HCF;
   tgt->aMode  = 0;
   tgt->A      = doRandomize? rand() : 0;
   tgt->bMode  = 0;
   tgt->B      = doRandomize? rand() : 0;
}

void arena_init(unsigned char doRandomize)
{
   int pos;
   for(pos=0; pos<CORESIZE; ++pos)
      clearLocation(pos, doRandomize);
}

Cell* getLocation(int position)
{
   return &arena[ position % CORESIZE];
}

void setLocation(int position, Cell *copy)
{
   Cell *tgt = &arena[ position % CORESIZE ];
   tgt->opcode = copy->opcode;
   tgt->aMode  = copy->aMode;
   tgt->A      = copy->A;
   tgt->bMode  = copy->bMode;
   tgt->B      = copy->B;
}
