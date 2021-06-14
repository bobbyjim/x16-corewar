#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cbm.h>
#include <conio.h>

#include "common.h"
#include "arena.h"
#include "cell.h"
#include "test.h"
#include "vm.h"

extern unsigned char totalTests, testsPassed;
char lineInputBuffer[80];

void resetTestArena()
{
    // wipe core
    clearArena();

    // set up some test data 
    loadCell("hcf #49 #17", 49);
    loadCell("hcf #1   #9", 4041);
    loadCell("hcf #2   #8", 4042);
    loadCell("hcf #3   #7", 4043);
    loadCell("hcf #4   #6", 4044);
    loadCell("hcf #5   #5", 4045);
    loadCell("hcf #6   #4", 4046);
    loadCell("hcf #7   #3", 4047);
    loadCell("hcf #8   #2", 4048);
    loadCell("hcf #9   #1", 4049);
    loadCell("hcf #17 #19", 4050);
    loadCell("hcf @-1  #7", 4051);
    loadCell("hcf #5   #5", 4052);
    loadCell("hcf #6  #22", 4053);
}

int readLine()
{
   if (!fgets(lineInputBuffer, sizeof(lineInputBuffer), stdin)) 
   {
      cprintf("\r\n");
      return 0;
   }
   lineInputBuffer[strlen(lineInputBuffer)-1] = '\0'; // chop newline
   return 1;
}

void repl()
{
   int address = 1000; // whatever
   int toAddress;

   cprintf("coreshell 1.0\r\n");
   setVerbosity(2);

   for(;;)
   {
      cprintf("\r\n%u %c ", _heapmemavail(), '%');
      if (! readLine()) continue;
      if (! strcmp(lineInputBuffer,"logout")) return;

      //
      // Now do some work.
      //
      if (!strcmp(lineInputBuffer,"cls"))
      {
          clrscr();
          gotoxy(0,0);
      }
      else if (!strcmp(lineInputBuffer,"verbose")) 
      {
         bumpVerbosity();
      }
      else if (!strcmp(lineInputBuffer,"reset"))
      {
         resetTestArena();
      }
      else if (sscanf(lineInputBuffer,"run %d", &address)==1)
      {
          address %= CORESIZE;
          setIp(address);
          execute();
      }
      else if (sscanf(lineInputBuffer,"d %d %d", &address, &toAddress) == 2)
      {
          if (toAddress < address) toAddress = address + 20;
          if (toAddress - address > 20) toAddress = address + 20;
          //cprintf("%d -> %d\r\n", address, toAddress);
          dumpArena(address, toAddress);
      }
      else // maybe its a line?
      {
          loadCell(lineInputBuffer, address);
          dumpArena(address, address);
      }
   }
}

void main() //int argc, char* argv[])
{
    cbm_k_bsout(0x8E); // revert to primary case

    bgcolor(BLACK);
    textcolor(LTGREY);
    clrscr();

    repl();

    /*  These work.
    RUN_TEST("t/hcf #a #b",  100, "hcf #4 #5",  100, "hcf #4 #5");
    RUN_TEST("t/mov #a  b",  100, "mov #4   1", 101, "hcf #0  #4");  
    RUN_TEST("t/mov 0 1", 100, "mov 0 1", 101, "mov 0 1");
    RUN_TEST("t/mov  a  b",  100, "mov 0  2",   102, "mov 0  2");
    */

    /*
    clearLocation(101);
    RUN_TEST("t/add #a  b",  100, "add #4 1",   101,  "hcf #0 #4" );
    RUN_TEST("t/add  a  b",   50, "add -1 1",   51,  "hcf #0 #17");   // assumes test data
    RUN_TEST("t/add @a  b",   50, "add @4001 1", 50, "add @4001 18");  // assumes test data
    RUN_TEST("t/sub #a  b", 4040, "sub #3 10", 4050, "hcf #17 #16");   // assumes test data
    */

    /*
    resetTestArena();
    RUN_TEST("t/sub  a  b", 4040, "sub 9 10",  4050, "hcf #17 #12");   // assumes test data
    RUN_IP_TEST("t/jmp", 4038, "jmp #0 5", 4043 );
    */

    //cprintf("\r\n\r\ntotal tests %u, total passed %u, total failed %u\r\n", totalTests, testsPassed, totalTests - testsPassed);
}
