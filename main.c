#include <stdio.h>
#include <string.h>

#include "x16.h"
#include "common.h"
#include "arena.h"
#include "cell.h"
#include "vm.h"
#include "process.h"

extern unsigned char totalTests, testsPassed;
char lineInputBuffer[80];

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
   char filename[80];
   unsigned char warriorCount;

   unsigned int warrior = 0;
   int ip = 100;

   x16_puts("coreshell 1.0\r\n");
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
      else if (! strcmp(lineInputBuffer, "arena"))
      {
          x16_arena_draw();
      }
      else if (! strcmp(lineInputBuffer,"clear"))
      {
          arena_init(0);
      }
      else if (sscanf(lineInputBuffer,"load %s", filename) == 1)
      {
         //loadProgramFromFile("bomber.red", 100);
         loadProgramFromFile( filename, ip );
         x16_arena_dump(ip, ip+10);
         process_add(warrior, ip);
         ++warrior;
      }
      else if (! strcmp(lineInputBuffer,"randomize"))
      {
          arena_init(1);
      }
      else if (! strcmp(lineInputBuffer,"run"))
      {
          warriorCount = process_runCorewar();
          x16_arena_dump(ip, ip+25);
          process_dump();
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

      x16_top();
   }
}

int main()
{
    x16_init();
    process_init();
    setVerbosity(2);
    repl();
}
