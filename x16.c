/****************************************************************************

        This file isolates all of the X16/CC65 specific code.

****************************************************************************/
#undef         X16   

#ifdef X16
#include <conio.h>
#include <peekpoke.h>
#else
#include <stdio.h>
#endif

#include <stdlib.h>

#include "x16.h"
#include "common.h"
#include "cell.h"
#include "arena.h"

void x16_init()
{
#ifdef X16
   cbm_k_bsout(0x8E); // revert to primary case
   cbm_k_setnam("petfont.bin");
   cbm_k_setlfs(0,8,0);
   cbm_k_load(2, 0x0f800);

   bgcolor(BLACK);
   textcolor(GREEN);
   clrscr();

   gotoxy(0,20);

   cputs("  @@@@@@@   @@@@@@   @@@@@@@   @@@@@@@@     @@@  @@@  @@@   @@@@@@   @@@@@@@ \r\n");
   cputs(" @@@@@@@@  @@@@@@@@  @@@@@@@@  @@@@@@@@     @@@  @@@  @@@  @@@@@@@@  @@@@@@@@\r\n");
   cputs(" !@@       @@!  @@@  @@!  @@@  @@!          @@!  @@!  @@!  @@!  @@@  @@!  @@@\r\n");
   cputs(" !@!       !@!  @!@  !@!  @!@  !@!          !@!  !@!  !@!  !@!  @!@  !@!  @!@\r\n");
   cputs(" !@!       @!@  !@!  @!@!!@!   @!!!:!       @!!  !!@  @!@  @!@!@!@!  @!@!!@! \r\n");
   cputs(" !!!       !@!  !!!  !!@!@!    !!!!!:       !@!  !!!  !@!  !!!@!!!!  !!@!@!  \r\n");
   cputs(" :!!       !!:  !!!  !!: :!!   !!:          !!:  !!:  !!:  !!:  !!!  !!: :!! \r\n");
   cputs(" :!:       :!:  !:!  :!:  !:!  :!:          :!:  :!:  :!:  :!:  !:!  :!:  !:!\r\n");
   cputs("  ::: :::  ::::: ::  ::   :::   :: ::::      :::: :: :::   ::   :::  ::   :::\r\n");
   cputs("  :: :: :   : :  :    :   : :  : :: ::        :: :  : :     :   : :   :   : :\r\n");

   textcolor(DEFAULT_COLOR);
   gotoxy(0,40);
   cputs("                    * * *   press a key to begin   * * *");
   cgetc();
   clrscr();

   _randomize(); 
#else
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

#endif

}


void x16_help()
{
#ifdef X16
   textcolor(LTBLUE);
   cputs("add    arena   cls     logout     reset     verbose     run      d %d      help\r\n");
   textcolor(DEFAULT_COLOR);
#else
    printf("add    arena    logout    reset    verbose    run    d <nnn>    help\n");
#endif
}

void x16_prompt(ip)
{
#ifdef X16
    cprintf("\r\n%u %u ", _heapmemavail(), ip);
#else
    printf("\n%u ", ip);
#endif
}

void x16_top()
{
#ifdef X16
      clrscr();
      gotoxy(0,0);
#endif
}

void x16_setbank(unsigned char bank)
{
#ifdef X16
    POKE(0x9f61, bank); // r38-
    POKE(0, bank);      // r39+
#endif
}

void x16_loadfile(char* name, unsigned int location)
{  
#ifdef X16
   cbm_k_setnam(name);
   cbm_k_setlfs(IGNORE_LFN,EMULATOR_FILE_SYSTEM,SA_IGNORE_HEADER);
   cbm_k_load(LOAD_FLAG, location);
#else
    // maybe open the file for reading or something?
#endif
}


void x16_execFail(char *reason)
{
    if (isVerbose())
    {
#ifdef X16
        textcolor(LTRED);
        cprintf("                                     rm: %s\r\n", reason);
        textcolor(DEFAULT_COLOR);
#else
        printf("rm: %s\b", reason);
#endif
    }
}

void x16_printCell(Cell *cell, char* postfix)
{
#ifdef X16
    cprintf("%s    %c%-5u  %c%-5u", 
        getOpcodeName(cell), 
        getMode(cell->aMode),
        cell->A,
        getMode(cell->bMode),
        cell->B
        );

    if (*postfix) 
       cputs(postfix);
#else
    printf("%s    %c%-5u  %c%-5u %s", 
        getOpcodeName(cell), 
        getMode(cell->aMode),
        cell->A,
        getMode(cell->bMode),
        cell->B,
        *postfix? postfix : ""
        );
#endif
}

void x16_pos(unsigned char x, unsigned char y)              
{ 
#ifdef X16
    gotoxy(x,y);
#endif
}

void x16_putc(char c)
{
#ifdef X16
    cputc(c);
#else   
    printf("%c", c);
#endif
}

void x16_putcxy(unsigned char x, unsigned char y, char c)
{
#ifdef X16
    cputcxy(x,y,c);
#else
    printf("%c", c);
#endif
}

void x16_puts(char* s)
{
#ifdef X16
    cputs(s);
#else
    printf("%s", s);
#endif
}

void x16_putsxy(unsigned char x, unsigned char y, char* s)
{
#ifdef X16
    cputsxy(x,y,s);
#else
    printf("%s", s);
#endif
}

void x16_arena_draw()
{
   int pos;

#ifdef X16
   gotoxy(1,5);
   for(pos=0; pos<CORESIZE; ++pos)
   {
      cputc(arena_getCellChar(pos));
      if (pos % 78 == 77) cputs("\r\n ");
   }
#else
    printf(" ");
    for(pos=0; pos<CORESIZE; ++pos)
    {
       printf("%c", arena_getCellChar(pos));
       if (pos % 78 == 77) printf("\n");
    }    
#endif
}

void x16_arena_ps(unsigned char owner, unsigned char pid, char *opcode)
{
#ifdef X16
   gotoxy((owner % 4) * 10, (pid % 4) * 10);
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
   int x = start;

   if (end < start) // swap
   {
      start = end;
      end = x;
   }  

   if (isVerbose() < 2) return;

#ifdef X16
   cprintf("\r\narena %5d:  ", start);
#else
   printf("\narena %5d:  ", start);
#endif

   cell = getLocation(start);
   x16_printCell(cell, "\r\n");

   for(x=start+1; x<=end; ++x)
   {
      cell = getLocation(x);
#ifdef X16
      cprintf("      %5d:  ", x);
#else
     printf("      %5d:  ", x);
#endif
      x16_printCell(cell, "\r\n");
   }
}

unsigned char x16_getByte(int address)
{
#ifdef X16
    return PEEK(address);
#else   
    return 0; // ???
#endif
}
