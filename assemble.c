
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "cell.h"
#include "token.h"
#include "common.h"
#include "x16.h"

unsigned char lineNumber;
Token *myToken, *searchToken;
Cell  *myCell;
int rawNumber;
extern unsigned char topToken; // token.h
extern Cell tempCell; // cell.c

unsigned char isLabel(char *thing)
{
   // it's a label if it starts with an alpha.
   return isalpha(thing[0]);
}

int getLabelLineNumber(char* label)
{
   int i;
   for(i=0; i < topToken; ++i)
   {
      searchToken = getToken(i);
      if EQ(searchToken->label, label)
         return i;
   }

   printf(" ** error - label [%s] not found\r\n", label);
   return -1; // ??
}

/*
 *  Compile token labels into relative offsets.
 *  Build cell array from tokens.
 *
 *  Return number of instructions assembled!
 */
unsigned char assemble()
{
   cell_resetProgram();
   for(lineNumber=0; lineNumber<topToken; ++lineNumber)
   {
       //printf("line number %u: ", lineNumber);

       myToken = getToken(lineNumber);
       //myCell  = getCell(lineNumber);
       tempCell.opcode = myToken->opcode;
       tempCell.aMode  = myToken->aMode;
       tempCell.bMode  = myToken->bMode;

       if (isLabel(myToken->A))
          rawNumber = getLabelLineNumber(myToken->A) - lineNumber;
       else
          sscanf(myToken->A, "%d", &rawNumber);
    
       // convert raw number to core location
       while (rawNumber < 0) rawNumber += CORESIZE; // inefficient
       tempCell.A = rawNumber;

       if (isLabel(myToken->B))
          rawNumber = getLabelLineNumber(myToken->B) - lineNumber;
       else 
          sscanf(myToken->B, "%d", &rawNumber);

       // convert raw number to core location
       while (rawNumber < 0) rawNumber += CORESIZE; // inefficient
       tempCell.B = rawNumber;

       //x16_printCell(myCell, "\n");
       cell_storeInProgram();
   }

   return getTokenCount();
}