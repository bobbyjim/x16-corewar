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
#include "arena.h"

extern unsigned char corewar_system_status;

unsigned char x16_stepMode = 0;

#ifdef  X16
void x16_show_banked_message(unsigned int index)
{
    unsigned int x;

    for (x=index; x<index+800; ++x)
       if (PEEK(x) == 0)
          cputs("\r\n");
       else  
          cputc(PEEK(x));

    cputs("\r\n");
}
#endif

void x16_init()
{
#ifdef X16
   cbm_k_bsout(0x8E); // revert to primary case
   cbm_k_setnam("petfont.bin");
   cbm_k_setlfs(0,8,0);
   cbm_k_load(2, 0x0f800);

   x16_loadfile( "text.bin", 0xb000 ); // put useful text at the end of bank 1
   bgcolor(BLACK);
   textcolor(GREEN);
   clrscr();

   gotoxy(0,20);

   x16_show_banked_message(0xb000);

   textcolor(DEFAULT_COLOR);
   gotoxy(0,40);
   cputs("                    * * *   press a key to begin   * * *");
   cgetc();
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

}

void x16_help()
{
#ifdef X16
   textcolor(GREEN);
   x16_show_banked_message(0xb000 + 800);
   textcolor(DEFAULT_COLOR);
#else
puts("-------------------------- CORESHELL COMMANDS --------------------------------");
puts("");
puts("cls: clear screen                       logout: quit program                  ");
puts("reset: clear arena memory                         ");
puts("run: run!                               d nnn: display arena from nnn         ");
puts("help: show this text                    new n: add a process                  ");
puts("");
puts("hcf a b: halt-catch-fire                mov a b                           ");
puts("add a b: b += a                         sub a b: b -= a   ");
puts("");
puts("jmp   b: jump to b                      jmn a b: jmp if a!=0");
puts("jmz a b: jmp if a==0                    seq a b: ++ip if a==b");
puts("slt a b: ++ip if a<b                    sne a b: ++ip if a!=b");
puts("");
puts("xch a b: exchange a,b at a              spl a b: split to b ");
puts("");
puts("------------------------------------------------------------------------------");
#endif
}

void x16_opcode_help()
{
#ifdef X16
    textcolor(GREEN);
    x16_show_banked_message(0xb000 + 1600);
    textcolor(DEFAULT_COLOR);
#else

#endif
}

void x16_prompt(int ip)
{
#ifdef X16
    //cprintf("\r\ncoreshell %u bytes free [%u] ", _heapmemavail(), ip);
    cprintf("\r\ncoreshell [%u] ", ip);
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

void x16_loadfile(char* name, unsigned int location)
{  
#ifdef X16
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

void x16_putLine(char* s)
{
#ifdef X16
    cprintf("%s\r\n", s);
#else
    printf("%s\n", s);
#endif
}

void x16_putValue(char* label, unsigned int value)
{
#ifdef X16
   cprintf("%s %u\r\n", label, value);
#else
   printf("%s %u\n", label, value);
#endif
}

void x16_arena_draw()
{
   int pos;
   unsigned char y;
   unsigned char x;

#ifdef X16
   gotoxy(1,6);
   for(pos=0; pos<CORESIZE; ++pos)
   {
       y = pos / 156;
       x = pos % 156;
       cputcxy(1+x,6+y,'.');
   }
#else
    printf(" ");
    for(pos=0; pos<CORESIZE/2; ++pos)
    {
       printf("%c", '.');
       if (pos % 78 == 77) printf("\n ");
    }  
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

void x16_arena_touch(int ip, unsigned char owner)
{
    unsigned char y = ip / 156;
    unsigned char x = ip % 156;
    unsigned char c = ip % 2;

    if (x16_stepMode > 0) return;

    c = c==0? SQUARE_SW : SQUARE_SE;

#ifdef X16
   textcolor(owner+2);
   cputcxy(1+x, 6+y, c); // was: CIRCLE_FILLED
   textcolor(DEFAULT_COLOR);
#else
    printf("%u @ %u\n\n", owner, ip);
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
   int len = end - start;

   if (len < 0) // swap
   {
      start = end;
      len = -len;
   }  

   for(x=0; x<len; ++x)
   {
      cell = arena_getLocation(x+start);
#ifdef X16
      cprintf(" %5d:  ", x+start);
#else
     printf(" %5d:  ", x+start);
#endif
      x16_printCell(cell, "     ");

      cell = arena_getLocation(start+x+len);
#ifdef X16
      cprintf(" %5d:  ", start+x+len);
#else
     printf(" %5d:  ", start+x+len);
#endif
      x16_printCell(cell, "\r\n");
   }

#ifdef X16
    cprintf("st:%u\r\n", corewar_system_status);
#else
    printf("st:%u\n", corewar_system_status);
#endif
}
