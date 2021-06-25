#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "x16.h"
#include "common.h"
#include "arena.h"
#include "cell.h"
#include "vm.h"
#include "process.h"

char lineInputBuffer[80];
unsigned int epoch;
char filename[80];
unsigned char warriorCount, origWarriorCount, curWarriorCount, lastWarrior;
unsigned int warrior = 0;

extern unsigned char x16_stepMode;

int readLine()
{
   if (!fgets(lineInputBuffer, sizeof(lineInputBuffer), stdin)) 
   {
      return 0;
   }
   lineInputBuffer[strlen(lineInputBuffer)-1] = '\0'; // chop newline
   return 1;
}

void repl()
{
    int ip = 100;

   x16_putLine("coreshell 1.0");
   x16_help();

   for(;;)
   {      
      process_dump();
      if (! strcmp(lineInputBuffer,"help")) 
      {
        x16_help();
      }
      if (! strcmp(lineInputBuffer,"opcodes"))
      {
          x16_opcode_help();
      }
      else if (sscanf(lineInputBuffer, "new %u", &warrior) == 1)
      {
          process_add(warrior,ip);
      }
      else if (! strcmp(lineInputBuffer,"clear"))
      {
          arena_init(0);
      }
      else if (! strcmp(lineInputBuffer,"random"))
      {
          arena_init(1);
      }
      else if (sscanf(lineInputBuffer,"load %s", filename) == 1)
      {
         ip = rand() % CORESIZE;
         loadProgramFromFile( filename, ip );
         x16_arena_dump(ip, ip+10);

         //
         // Let's skip any initial HCF / DAT instructions.
         //
         while(getLocation(ip)->opcode == HCF) ++ip;

         process_add(warrior, ip);
         ++warrior;
      }
      else if (! strcmp(lineInputBuffer,"step"))
      {
          x16_stepMode = 1;
          warriorCount = process_runCorewar();
          x16_arena_dump(ip, ip+25);
          process_dump();
      }
      else if (! strcmp(lineInputBuffer,"run"))
      {
          x16_stepMode = 0;
          x16_clrscr();
          x16_arena_draw();
          epoch = 0;
          warriorCount = process_runCorewar();
          origWarriorCount = warriorCount;
          for(epoch = 0; epoch < MAXIMUM_EPOCHS; ++epoch)
          {
              curWarriorCount = process_runCorewar();
              if ( curWarriorCount != warriorCount )
              {
                  //epoch = 0;
                  if ( (curWarriorCount == 0) 
                    || (origWarriorCount > 1 && curWarriorCount == 1) )
                  {
                    epoch = MAXIMUM_EPOCHS; // done
//                  x16_putValue("**** warriors remaining: ", curWarriorCount);
                  }
              }
              else if (epoch > 0 && epoch % 100 == 0)
              {
//                  x16_putValue("**** epoch count: ", epoch);
              }
              warriorCount = curWarriorCount;
              //process_dump();
          }
          x16_top();
          lastWarrior = process_lastWarrior();
          if (warriorCount == 1)
             x16_putValue("winner: warrior no. ", lastWarrior);
          else if (origWarriorCount == 1)
             x16_putLine("suicide!");
          else
             x16_putLine("stalemate!");

      }
      else if (sscanf(lineInputBuffer,"%u", &ip) == 1)
      {
          // ip has been reset
      }
      else if (sscanf(lineInputBuffer,"d %d", &ip) == 1)
      {
          x16_arena_dump(ip, ip+25);
      }
      else // maybe its a line?
      {
          if (loadCell(lineInputBuffer, ip) != INVALID_OPCODE)             
            ++ip;
      }

      x16_prompt(ip);
      
      if (! readLine()) continue;
      if (! strcmp(lineInputBuffer,"logout")) return;

      x16_clrscr();
   }
}

int main()
{
    x16_init();
    process_init();
    repl();
    return 0;
}
