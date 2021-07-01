#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "x16.h"
#include "common.h"
#include "arena.h"
#include "cell.h"
#include "vm.h"
#include "process.h"

char lineInputBuffer[LINE_BUFFER_SIZE];
char filename[LINE_BUFFER_SIZE];
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

   x16_msg("coreshell 1.0");
   x16_help();

   for(;;)
   {      
      process_dump();

      if (! strcmp(lineInputBuffer,"help")) 
      {
        x16_help();
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
        /*
            loadWarrior()

            Creates a new warrior from "filename" into the process queue
            and loads the instructions into a random location in the Core.

            Increments "warrior".

            Sets the ip of the first executable instruction loaded.
        */
        ip = rand() % CORESIZE;

        x16_msg2("loading:", filename);
        cell_setLocation( ip );
        cell_loadFile( filename );
        cell_parseBank();
        cell_copyProgramIntoCore();

        x16_arena_dump(ip, ip+10);

        //
        // Let's skip any initial HCF (i.e. DAT) instructions.
        //
        while(arena_getLocation(ip)->opcode == HCF) ++ip;

        process_add(warrior, ip);
        ++warrior;
      }
      else if (! strcmp(lineInputBuffer,"step"))
      {
          x16_stepMode = 1;
          if (process_prepareNextToRun())
          {
             ip = vm_execute();
             process_postExecute(ip);
          }
          x16_arena_dump(ip, ip+25);
          process_dump();
      }
      else if (! strcmp(lineInputBuffer,"run"))
      {
         x16_stepMode = 0;
         x16_clrscr();
         x16_arena_draw();
         origWarriorCount = process_countWarriors();
         while (process_prepareNextToRun() && process_continue())
         {
            ip = vm_execute();
            process_postExecute(ip);
         }

         x16_top();
         warriorCount = process_countWarriors();
         if (warriorCount == 1)
         {
            lastWarrior = process_lastWarrior();
            x16_putValue("winner: warrior no. ", lastWarrior);
         }
         else if (origWarriorCount == 1)
            x16_msg("suicide!");
         else
            x16_msg("stalemate!");

         //x16_msg("press a key to return to menu");
         //x16_getc();
      }
      else if (sscanf(lineInputBuffer,"d %d", &ip) == 1)
      {
          x16_arena_dump(ip, ip+25);
      }
      else if (strlen(lineInputBuffer) > 0) // maybe its a line?
      {
          cell_setLocation(ip);
          if (cell_loadInstruction(lineInputBuffer) != INVALID_OPCODE)             
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
