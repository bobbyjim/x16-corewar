
#include <conio.h>

#include "common.h"
#include "arena.h"
#include "cell.h"

#define  OPCODE(pos) (arena[pos % CORESIZE].opcode)

Cell arena[CORESIZE];

char cellChar[16] = {
   46,      // hcf
   95,      // mov
   '+',
   '-',
   '*',
   '/',
   '%',
   94,      // jmp
   172,     // jmn
   187,     // jmz
   188,     // djn
   190,     // djz
   195,     // ske
   195,     // sne
   215,     // spl
   172      // nop
};

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

void clearArena()
{
   cputs("Arena cleared.\r\n");
}

void drawArena()
{
   int pos;
   gotoxy(1,4);
   for(pos=0; pos<CORESIZE; ++pos)
   {
      cputc(cellChar[OPCODE(pos)]);
      if (pos % 78 == 77) cputs("\r\n ");
   }
}

void drawCell(int pos)
{
   int row = 4 + pos / 78;
   int col = 1 + pos % 78;

   cputcxy(col,row,cellChar[OPCODE(pos)]);
}

void dumpArena(int start, int length)
{
   Cell *cell;
   int x = 0;

   for(x=0; x<length; ++x)
   {
      cell = getLocation(start+x);
      cprintf("%d: ", start + x);
      printCell(cell, "\r\n");
   }
}