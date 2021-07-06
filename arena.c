//#include <stdio.h>
//#include "x16.h"

#include <stdlib.h>

#include "common.h"
#include "arena.h"
#include "bank.h"
#include "cell.h"

#ifndef X16
/* 
    For Unix, we don't have to fool around with banked memory.
    So let's just grab a big hunk of static memory here as usual.
 */
Cell arena[CORESIZE];
#endif

unsigned char corewar_system_status;
Cell* tmp;

void arena_clearLocation(int position, unsigned char doRandomize)
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

   setCoreBank(position);
   tmp = &CORE_MEMORY(position); // &arena[ position % CORESIZE ];
   tmp->opcode = HCF;
   tmp->aMode  = 0;
   tmp->A      = doRandomize? rand()%256 : 0;
   tmp->bMode  = 0;
   tmp->B      = doRandomize? rand()%256 : 0;
}

void arena_init(unsigned char doRandomize)
{
   int pos;      
   for(pos=0; pos<CORESIZE; ++pos)
      arena_clearLocation(pos, doRandomize);
}

Cell* arena_getLocation(int position)
{
   setCoreBank(position);
   return &CORE_MEMORY(position); //&arena[ position % CORESIZE];
}

Cell arena_tmp_cell;

/*
     We need this due to banking.
 */
void arena_setData(int target, int source)
{
   Cell *src = arena_getLocation(source);
   arena_tmp_cell = *src; // copy
   arena_setLocation(target, &arena_tmp_cell);
}

void arena_setLocation(int target, Cell *copy)
{
   Cell *tgt = arena_getLocation(target);   
   tgt->opcode = copy->opcode;
   tgt->aMode  = copy->aMode;
   tgt->A      = copy->A;
   tgt->bMode  = copy->bMode;
   tgt->B      = copy->B;
}
