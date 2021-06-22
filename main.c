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
   unsigned char warriorCount;

   int ip = 100;

   x16_puts("coreshell 1.0\r\n");
   x16_help();

   for(;;)
   {      
      if (! strcmp(lineInputBuffer,"help")) 
      {
        x16_help();
      }
      else if (! strcmp(lineInputBuffer,"add"))
      {
          process_add(0,ip);
      }
      else if (! strcmp(lineInputBuffer, "arena"))
      {
          x16_arena_draw();
      }
      else if (! strcmp(lineInputBuffer,"clear"))
      {
          arena_init(0);
      }
      else if (! strcmp(lineInputBuffer,"randomize"))
      {
          arena_init(1);
      }
      else if (! strcmp(lineInputBuffer,"run"))
      {
          warriorCount = process_runCorewar();
      }   
      else if (sscanf(lineInputBuffer,"ip %d", &ip) == 1)
      {
          // ip has been reset
      }
      else if (sscanf(lineInputBuffer,"d %d", &ip) == 1)
      {
          x16_arena_dump(ip, ip+25);
      }
      else // maybe its a line?
      {
          loadCell(lineInputBuffer, ip);
          ++ip; // why not
      }

      x16_prompt(ip);
      
      if (! readLine()) continue;
      if (! strcmp(lineInputBuffer,"logout")) return;

      x16_top();
   }
}

int main()
{
    unsigned char warriorCount = 0;

    x16_init();

    process_init();
    arena_init(0);
    setVerbosity(2);

    loadProgramFromFile("bomber.red", 100); // TODO: move this into the repl 
    repl();
}
