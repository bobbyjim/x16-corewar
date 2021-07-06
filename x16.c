/****************************************************************************

        This file isolates all of the X16/CC65 specific code.

****************************************************************************/
#include "common.h"


#ifdef X16
#include <conio.h>
#include <peekpoke.h>
#else
#include <stdio.h>
#include <time.h>
#endif

#include <stdlib.h>

#include "x16.h"
#include "cell.h"
#include "bank.h"
#include "arena.h"
#include "process.h"

extern unsigned char corewar_system_status; // from arena.c

unsigned char x16_stepMode = 0;
unsigned char arena_square[2][2] = {{SQUARE_NW, SQUARE_NE }, { SQUARE_SW, SQUARE_SE }}; 

extern unsigned char currentBank; // from bank.c
extern unsigned int epoch; // from process.c

#ifdef  X16
void x16_show_banked_message(unsigned int index)
{
    unsigned int x;

    setBank(HELP_BANK);

    for (x=index; x<index+800; ++x)
       if (PEEK(x) == 0)
          cputs("\r\n");
       else  
          cputc(PEEK(x));

    cputs("\r\n");
}
#endif

unsigned char x16_init()
{
   unsigned char demo = 0;

#ifdef X16
   unsigned long wait = 800000;

   cbm_k_bsout(0x8E); // revert to primary case
   cbm_k_setnam("petfont.bin");
   cbm_k_setlfs(0,8,0);
   cbm_k_load(2, 0x0f800);

   x16_loadfile( "text.bin", HELP_BANK, 0xa000 ); // put useful text in bank 2
   bgcolor(BLACK);
   textcolor(GREEN);
   clrscr();

   gotoxy(0,20);

   x16_show_banked_message(0xa000);

   gotoxy(0,40);
   textcolor(LTRED);
   cputs("                    * * *   press a key to begin   * * *");
   textcolor(DEFAULT_COLOR);

   demo = 1;
   while(--wait)
   {
      if (kbhit())
      {
          demo = 0;
          break;
      }
   }

   clrscr();

   _randomize(); 
#else
   puts("\n\n\n\n");
   puts("  @@@@@@@   @@@@@@   @@@@@@@   @@@@@@@@     @@@  @@@  @@@   @@@@@@   @@@@@@@ ");
   puts(" @@@@@@@@  @@@@@@@@  @@@@@@@@  @@@@@@@@     @@@  @@@  @@@  @@@@@@@@  @@@@@@@@");
   puts(" !@@       @@!  @@@  @@!  @@@  @@!          @@!  @@!  @@!  @@!  @@@  @@!  @@@");
   puts(" !@!       !@!  @!@  !@!  @!@  !@!          !@!  !@!  !@!  !@!  @!@  !@!  @!@");
   puts(" !@!       @!@  !@!  @!@!!@!   @!!!:!       @!!  !!@  @!@  @!@!@!@!  @!@!!@! ");
   puts(" !!!       !@!  !!!  !!@!@!    !!!!!:       !@!  !!!  !@!  !!!@!!!!  !!@!@!  ");
   puts(" :!!       !!:  !!!  !!: :!!   !!:          !!:  !!:  !!:  !!:  !!!  !!: :!! ");
   puts(" :!:       :!:  !:!  :!:  !:!  :!:          :!:  :!:  :!:  :!:  !:!  :!:  !:!");
   puts("  ::: :::  ::::: ::  ::   :::   :: ::::      :::: :: :::   ::   :::  ::   :::");
   puts("  :: :: :   : :  :    :   : :  : :: ::        :: :  : :     :   : :   :   : :");
   puts("\n\n\n\n");

   srand(time(0));
#endif

   return demo;
}

void x16_help()
{
#ifdef X16
   textcolor(GREEN);
   x16_show_banked_message(0xa000 + 800);
   textcolor(DEFAULT_COLOR);
#else
puts(" ---------------------------- CORESHELL 1.0 ----------------------------------");
puts("");
puts("");
puts("                  https://github.com/bobbyjim/x16-corewar");
puts("");
puts("");
puts("      clear   : clear core                     logout: quit program           ");
puts("      random  : randomize core                 help  : show this text        ");
puts("      run     : begin or continue run          step  : run one epoch         ");
puts("");
puts("      d <nnn> : display core from <nnn> and set ip ");
puts("      new <n> : add warrior <n> at current ip");
puts("");
puts("      load <redcode file> and add warrior");
puts("");
puts("");
puts("      * redcode can also be entered at the prompt");
puts("");
puts("");
puts("      --> begin every redcode file with three semicolons ';;;'");
puts("");
puts("");
puts("      --> put space between the operands");
puts("");
puts("");
puts("     ");
puts("");
puts(" -----------------------------------------------------------------------------");
#endif
}

void x16_opcode_help()
{
#ifdef X16
    textcolor(GREEN);
    x16_show_banked_message(0xa000 + 1600);
    textcolor(DEFAULT_COLOR);
#else

#endif
}

void x16_prompt(int ip)
{
#ifdef X16
    cprintf("\r\ncoreshell %u (%u) ", _heapmemavail(), ip);
    //cprintf("\r\ncoreshell (@%u) ", ip);
#else
    printf("\ncoreshell [%u] ", ip);
#endif
}

void x16_top()
{
#ifdef X16
      gotoxy(0,0);
#endif
}

void x16_clrscr()
{
#ifdef X16
      clrscr();
      gotoxy(0,0);
#endif
}

int x16_getc()
{
#ifdef X16
    return cgetc();
#else
    return getc(stdin);
#endif
}

void x16_loadfile(char* name, unsigned char bank, unsigned int location)
{  
#ifdef X16
   setBank(bank);
   cbm_k_setnam(name);
   cbm_k_setlfs(IGNORE_LFN,EMULATOR_FILE_SYSTEM,SA_IGNORE_HEADER);
   cbm_k_load(LOAD_FLAG, location);
#else
   printf("%s (%d)\n", name, location);
#endif
}

void x16_printCell(Cell *cell, char* postfix)
{
#ifdef X16
    cprintf("%s    %c%-5u  %c%-5u", 
        getOpcodeName(cell->opcode), 
        getMode(cell->aMode),
        cell->A,
        getMode(cell->bMode),
        cell->B
        );

    if (*postfix) 
       cputs(postfix);
#else
    printf("%s    %c%-5u  %c%-5u %s", 
        getOpcodeName(cell->opcode), 
        getMode(cell->aMode),
        cell->A,
        getMode(cell->bMode),
        cell->B,
        *postfix? postfix : ""
        );
#endif
}

void x16_msg(char* s)
{
#ifdef X16
    cprintf("%s\r\n", s);
#else
    printf("%s\n", s);
#endif
}

void x16_msg2(char* a, char *b)
{
#ifdef X16
    cprintf("%s %s\r\n", a, b);
#else
    printf("%s %s\n", a, b);
#endif
}


void x16_putValue(char* label, unsigned int value)
{
#ifdef X16
   cprintf("%s: %u\r\n", label, value);
#else
   printf("%s: %u\n", label, value);
#endif
}

void x16_putString(char* label, char* value)
{
   #ifdef X16
   cprintf("%s: %s\r\n", label, value);
#else
   printf("%s: %s\n", label, value);
#endif 
}

void x16_arena_draw()
{
   int pos;

#ifdef X16
   unsigned char y;
   unsigned char x;

   /*
   gotoxy(WARRIOR_LIST_LEFT,0);
   for(x=0; x<WARRIORS_MAX; ++x)
   {
       textcolor(x+2);
       cputc('1'+x);
   }
   textcolor(DEFAULT_COLOR);
   */

   textcolor(DKGREY);
   for(pos=0; pos<CORESIZE/2; ++pos)
   {
       y = pos / 80;
       x = pos % 80;
       cputcxy(ARENA_LEFT+x,ARENA_TOP+y/2,SQUARE_SW);
   }
   textcolor(DEFAULT_COLOR);
#else
    printf(" ");
    for(pos=0; pos<CORESIZE/2; ++pos)
    {
       printf("%c", '.');
       if (pos % 78 == 77) printf("\n ");
    }  
#endif
}

void x16_arena_touch(int ip, unsigned char owner)
{
    unsigned char y = ip / 160;
    unsigned char x = ip % 160;

    if (x16_stepMode > 0) return;

#ifdef X16
   textcolor(owner+2);

   //
   //  y ranges from 0 to 50.
   //  x ranges from 0 to 160.  
   //
   //  We need to "compress" x into 80 positions.
   //
   cputcxy(ARENA_LEFT+x/2, ARENA_TOP+y/2, arena_square[y%2][x%2]); // was: CIRCLE_FILLED
   textcolor(DEFAULT_COLOR);
   /*
   if ( epoch % 250 == 0)
   {
      cputcxy(79,1, '0' + (epoch*10)/MAXIMUM_EPOCHS);
      gotoxy(0,0);
   }*/
#else
    printf("%u @ %u\n\n", owner, ip);
#endif
}

/*
 *  Mark the space underneath this warrior's ID number.
 */
void x16_ps(unsigned char owner, char state)
{
#ifdef X16
   cputcxy(WARRIOR_LIST_LEFT+owner,1, state);
#endif
}

void x16_arena_ps(unsigned char owner, unsigned char pid, char *opcode)
{
#ifdef X16
   gotoxy((owner % 4) * 20, pid % 4);
   cputs(opcode);
#else
    printf("%u:%u %s", owner, pid, opcode);
#endif
}

void x16_ps_log(char *msg, unsigned char owner, unsigned char pid, int ip)
{
#ifdef X16
    cprintf("%s(%u:%u @%u)\r\n", msg, owner, pid, ip);
#else
    printf("%s(%u:%u @%u)\n", msg, owner, pid, ip);
#endif
}

void x16_arena_dump(int start, int end)
{
   Cell *cell;
   int x;
   int pos;
   int len = end - start;

   if (len < 0) // swap
   {
      start = end;
      len = -len;
   }  

   for(x=0; x<len; ++x)
   {
      pos = x + start;
      if (pos < 0) pos += CORESIZE;
      if (pos > CORESIZE) pos -= CORESIZE;

      cell = arena_getLocation(pos);
#ifdef X16
      cprintf(" %5d:  ", pos);
#else
     printf(" %5d:  ", pos);
#endif
      x16_printCell(cell, "     ");

      pos += len;
      if (pos > CORESIZE) pos -= CORESIZE;
      cell = arena_getLocation(pos);
#ifdef X16
      cprintf(" %5d:  ", pos);
#else
     printf(" %5d:  ", pos);
#endif
      x16_printCell(cell, "\r\n");
   }

    x16_putValue("system status", corewar_system_status);
/*
#ifdef X16
    cprintf("st:%u\r\n", corewar_system_status);
#else
    printf("st:%u\n", corewar_system_status);
#endif*/
}
