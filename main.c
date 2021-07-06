#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "x16.h"
#include "common.h"
#include "arena.h"
#include "cell.h"
#include "vm.h"
#include "process.h"
#include "token.h"
#include "assemble.h"

char lineInputBuffer[LINE_BUFFER_SIZE];
char filename[LINE_BUFFER_SIZE];
unsigned char warriorCount, origWarriorCount, curWarriorCount, lastWarrior;
unsigned int warrior = 0;

extern unsigned char x16_stepMode;
extern unsigned char topToken; // from token.c
int i;

int readLine()
{
   if (!fgets(lineInputBuffer, sizeof(lineInputBuffer), stdin)) 
   {
      return 0;
   }
   lineInputBuffer[strlen(lineInputBuffer)-1] = '\0'; // chop newline
   return 1;
}

void run()
{
   int ip = 0;

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
      x16_putValue("winner", lastWarrior+1);
   }
   else if (origWarriorCount == 1)
      x16_msg("suicide!");
   else
      x16_msg("stalemate!");
}

void repl()
{
   int ip = 100;
   for(;;)
   {      
      process_dump();

      if (! strcmp(lineInputBuffer,"help")) x16_help();
      else if (sscanf(lineInputBuffer, " new %u ", &warrior) == 1) process_add(warrior,ip);
      else if (! strcmp(lineInputBuffer,"clear"))
      {
         x16_msg("initializing core.");
         arena_init(0);
         process_init();
      }
      else if (! strcmp(lineInputBuffer,"random")) 
      {
         x16_msg("initializing core with random data.");
         arena_init(1);
         process_init();
      }
      else if (sscanf(lineInputBuffer," d %d ", &ip) == 1) x16_arena_dump(ip, ip+30);
      else if (sscanf(lineInputBuffer," load %s ", filename) == 1)
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
          x16_arena_dump(ip-1, ip+24);
          process_dump();
      }
      else if (! strcmp(lineInputBuffer,"run"))
      {
         run();
      }
      else if (strlen(lineInputBuffer) > 0) // maybe its a line?
      {
          cell_setLocation(ip);
          tokenizer_init();
          if (tokenize(lineInputBuffer) != INVALID_OPCODE)
          {
             assemble();
             cell_copyProgramIntoCore();
             ++ip;
          }
          //if (cell_loadInstruction(ip, lineInputBuffer) != INVALID_OPCODE)     
          //  ++ip;
      }
      x16_prompt(ip);
      
      if (! readLine()) continue;
      if (! strcmp(lineInputBuffer,"logout")) return;

      x16_clrscr();
   }
}

void load(int num)
{
   int ip = rand() % CORESIZE;
   cell_setLocation(ip);
   cell_copyProgramIntoCore();
   //
   // Skip any initial HCF (i.e. DAT) instructions.
   //
   while(arena_getLocation(ip)->opcode == HCF) ++ip;
   process_add(num, ip);
}

void runDemo()
{
   // plague
   tokenizer_init();
   tokenize("   spl 13  #0");
   tokenize("   spl 12  #0");
   tokenize("   add #24 10");
   tokenize("   jmz -1  @9");
   tokenize("   mov 12  @8");
   tokenize("   mov 12  <7");
   tokenize("   add #1  6");
   tokenize("   jmp -5  #0");
   tokenize("   dat #0  #1");
   tokenize("   dat #0  #2");
   tokenize("   dat #0  #3");
   tokenize("   dat #0  #4");
   tokenize("   dat #0  #5");
   tokenize("   mov 2   <2");
   tokenize("   jmp -1  #0");
   tokenize("   dat #0  #-16");
   tokenize("   jmp -1  #0");
   tokenize("   spl 0   #0");
   assemble();
   load(0);

   // agony
   tokenizer_init();
   tokenize("scan    sub incr   comp");
   tokenize("comp    cmp -5341  -5353");
   tokenize("        slt #in13  comp");
   tokenize("        djn scan   <-5512");
   tokenize("        mov #12    count");
   tokenize("        mov comp   bptr");
   tokenize("bptr    dat #0");
   tokenize("split   mov bomb   <bptr");
   tokenize("count   djn split  #0");
   tokenize("        jmn scan   scan");
   tokenize("bomb    spl 0");
   tokenize("incr    mov -28    <-28");
   tokenize("        dat     #0");
   tokenize("        dat     #0");
   tokenize("        dat     #0");
   tokenize("        dat     #0");
   tokenize("        dat     #0");
   tokenize("        dat     #0");
   tokenize("        dat     #0");
   tokenize("        dat     #0");
   tokenize("        dat     #0");
   tokenize("        dat     #0");
   tokenize("        dat     #0");
   tokenize("in12    dat     #0");
   tokenize("in13    dat     #0");
   assemble();
   load(1);

   // xtc
   tokenizer_init();
   tokenize("loop  add #412 ptr");
   tokenize("ptr   jmz loop trap");
   tokenize("      mov ptr  dest");
   tokenize("cnt   mov #23  cnt");
   tokenize("kill  mov @trap <dest");
   tokenize("      djn kill cnt");
   tokenize("      jmp loop");
   tokenize("      dat 0");
   tokenize("      dat 0");
   tokenize("      dat 0");
   tokenize("      dat 0");
   tokenize("      dat 0");
   tokenize("      dat 0");
   tokenize("dest  dat 0");
   tokenize("      dat 0");
   tokenize("      dat 0");
   tokenize("      dat 0");
   tokenize("      dat 0");
   tokenize("      dat 0");
   tokenize("trap  dat bomb");
   tokenize("bomb  spl trap trap");
   assemble();
   load(2);

   run();
}

int main()
{
    unsigned char demo = x16_init();

    process_init();

    if (demo)
    {
       runDemo();
       repl();
    }
    else
    {
       x16_help(); 
       x16_msg("** operand expressions not supported");
       x16_msg(""); // newline
       repl();
    }
    return 0;
}
