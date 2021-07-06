#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef X16
#include    <conio.h>
#include    <peekpoke.h>
#endif

#include "cell.h"
#include "arena.h"
#include "bank.h"
#include "common.h"
#include "x16.h"
#include "token.h"
#include "assemble.h"

char* opcodes[16] = {
	/*  0 */ "hcf", 
	/*  1 */ "mov",                            
	/*  2 */ "add",
    /*  3 */ "sub",
    /*  4 */ "jmp",
	/*  5 */ "jmz", 
	/*  6 */ "jmn", 
    /*  7 */ "seq",
    /*  8 */ "slt",
	/*  9 */ "sne", 
	/* 10 */ "flp", 
	/* 11 */ "djn", 
	/* 12 */ "inc", 
	/* 13 */ "dec", 
    /* 14 */ "xch",
	/* 15 */ "spl"  
};

Cell program[256];   // up to 256 parsed instructions
unsigned char programSize;

unsigned int location; // where the parsed instructions should go

Cell tempCell;
char buffer[LINE_BUFFER_SIZE+1];
extern unsigned int bankAddress; // bank.c
unsigned char c;

unsigned char buffer_position = 0;
unsigned int bankAddress;
unsigned char eoln = 0x0a;
unsigned char eof = '\0';


char modes[] = { 
        '#',    // value (0)
        ' ',    // address (1)
        '@',    // address indirect (2)
        '<'     // address indirect predecrement (3)
};

Cell* getCell(unsigned char index)
{
    return &program[index];
}

/********************************************************************
 *
 *   Load a redcode file.
 *
 *   If X16: load into bank 1.
 *   Else: read by line and load directly into the arena.
 *
 ********************************************************************/
void cell_loadFile(char *filename)
{
#ifdef X16
    //
    //  Load file into Bank 1
    // 
    setBank(REDCODE_BANK);
    bankAddress = 0xa000;
    memset( (unsigned char*) bankAddress, 0x00, 4096);
    cbm_k_setnam(filename);
    cbm_k_setlfs(IGNORE_LFN,EMULATOR_FILE_SYSTEM,SA_IGNORE_HEADER);
    cbm_k_load(LOAD_FLAG, bankAddress);
#else
    FILE *fp = fopen(filename, "r");
    int ok;

    if (fp == NULL)
    {
        printf("ERROR cannot open %s\n", filename);
        return;
    }

    tokenizer_init();
    while (fgets(buffer, LINE_BUFFER_SIZE, fp) != NULL)
    {
       tokenize(buffer);
    }

    printf("assembling\n");
    assemble();
    /*
       if (cell_parseInstruction(buffer) != INVALID_OPCODE)
       {
          arena_setLocation(location, &tempCell);
          location++;
       }
    */
    fclose(fp);
#endif
}

void cell_setLocation(unsigned int destination)
{
    location = destination;
}

#ifdef X16
void eatBankWhitespace()
{
    while( PEEK(bankAddress) == ' ' 
        || PEEK(bankAddress) == '\t' )
        ++bankAddress; 
}
#endif

#ifdef X16
unsigned char readBankLine()
{
    // eat initial whitespace
    eatBankWhitespace();

    // reset buffer
    memset(buffer, '\0', sizeof(buffer));
    buffer_position = 0;

    while(buffer_position < LINE_BUFFER_SIZE)
    {
       c = PEEK(bankAddress); // current char
       if (c == eof) // we are SO done
          return eof;

       if (c == eoln)  // line is ready?
       {
          ++bankAddress; // eat end of line
          return eoln;   // and return line for processing.  yeah you heard me.
       }

       ++bankAddress; // move read head...

       if (c == ',' || c == ':') // throw , and : on the floor
          continue;

       //
       //  Okay, we found a character we can keep.
       // 
       buffer[buffer_position] = c;
       
       if (buffer[buffer_position] > 96) 
          buffer[buffer_position] -= 32; // to PETSCII uppercase

       ++buffer_position;
   }

   if (buffer_position == LINE_BUFFER_SIZE) // eat rest of line
      while((PEEK(bankAddress) != eoln) || (PEEK(bankAddress) != eof))
         ++bankAddress;

   return 1;
}
#endif


void cell_resetProgram()
{
    programSize = 0;
}

void cell_copyProgramIntoCore()
{
    unsigned char x;
    for(x=0; x<programSize; ++x)
    {
        arena_setLocation(location, &program[x]);
        //cprintf("loaded cell into %u\r\n", location);
        ++location;
    }
}

void cell_storeInProgram()
{
    program[programSize].opcode = tempCell.opcode;
    program[programSize].aMode  = tempCell.aMode;
    program[programSize].A      = tempCell.A;
    program[programSize].bMode  = tempCell.bMode;
    program[programSize].B      = tempCell.B;
    ++programSize;
}

void cell_parseBank()
{
#ifdef X16
    bankAddress = 0xa000;
    programSize = 0;
    //
    //  Break up into lines and serve.
    //
    tokenizer_init();
    while(readBankLine())
    {
       if (strlen(buffer) == 0) // ignore
       {
       }
       else if (buffer[0] == ';') // this is just a comment
       {
          //cprintf("comment: %s\r\n", buffer);
       }
       else 
       {
           /*
          if (cell_parseInstruction(buffer) != INVALID_OPCODE )
             cell_storeInProgram();
          else
             cprintf("invalid opcode\r\n");
            */

          tokenize(buffer);
       }
    }
    assemble();
#endif
}

unsigned char cell_encode_opcode(char *opcode)
{
    unsigned char x;

#ifndef X16
    opcode[0] |= 32;
    opcode[1] |= 32;
    opcode[2] |= 32;
#endif

    //printf("checking opcode [%s]\n", opcode);    
    for(x=0; x<16; ++x)
       if EQ(opcode, opcodes[x])
           return x;

    if (EQ(opcode, "cmp") || EQ(opcode, "CMP")) return SEQ;
    if (EQ(opcode, "dat") || EQ(opcode, "DAT")) return HCF;

    return INVALID_OPCODE;
}

/*
unsigned char cell_loadInstruction(int location, char *input)
{
   unsigned char result = cell_parseInstruction(input);

   if (result != INVALID_OPCODE)
      arena_setLocation(location, &tempCell);
   
   return result;
}
*/

/*
void cell_decode_operand(char *src, unsigned char *mode, unsigned int *val)
{
   int rawValue = 0;
   switch(*src)
   {
       case '#': 
            *mode = IMMEDIATE;
            ++src;
            break;

       case '@': 
            *mode = INDIRECT;
            ++src;
            break;

       case '<':
            *mode = PREDECREMENT_INDIRECT;
            ++src;
            break;

       default:  
            *mode = DIRECT;
            break;
   }
   sscanf(src, "%d", &rawValue); // allows negative sign

   // convert raw value to core location
   while (rawValue < 0) rawValue += CORESIZE; // inefficient
   
   *val = rawValue; 
}
*/

/*
 
    Parse the instruction into *tempCell

 */
 /*
unsigned char cell_parseInstruction(char *input)
{
    char opcode[3] = "";
    char a[8] = ""; 
    char b[8] = "";
    unsigned char opcode_value;
    unsigned char argc;

    unsigned char amode = 0;
    unsigned char bmode = 0;
    unsigned int aval   = 0;
    unsigned int bval   = 0;

    input = strtok(input, ";"); // wipe out comments

    argc = sscanf(input, " %3s %s %s", opcode, a, b);
    opcode_value = cell_encode_opcode(opcode);

    if (opcode_value == INVALID_OPCODE)
       return INVALID_OPCODE;
   
    cell_decode_operand(a, &amode, &aval);
    cell_decode_operand(b, &bmode, &bval);

    tempCell.opcode  = opcode_value;
    tempCell.aMode  = amode;
    tempCell.A      = aval;
    tempCell.bMode  = bmode;
    tempCell.B      = bval;

    //
    //  OK it's a valid opcode.
    //  Now check for only one operand.
    //  This is handled elegantly for all cases EXCEPT DAT/HCF.
    //
    if (argc == 2 && opcode_value == HCF) 
    {
        // for DAT, move A into B
        tempCell.B     = tempCell.A;
        tempCell.bMode = tempCell.aMode;
        tempCell.A     = 0;
        tempCell.aMode = 0;
    }

    return opcode_value;
}
*/

char* getOpcodeName(unsigned char code)
{
    return opcodes[code];
}

char  getMode(unsigned char mode)
{
    return modes[mode];
}

