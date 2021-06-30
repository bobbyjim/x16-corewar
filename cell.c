
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

char* opcodes[16] = {
	/*  0 */ "hcf", 
	/*  1 */ "mov",                            
	/*  2 */ "add",
    /*  3 */ "sub",
    /*  4 */ "jmp",
	/*  5 */ "jmn", 
	/*  6 */ "jmz", 
    /*  7 */ "seq",
    /*  8 */ "slt",
	/*  9 */ "sne", 
	/* 10 */ "flp", 
	/* 11 */ "djn", 
	/* 12 */ "...", 
	/* 13 */ "...", 
    /* 14 */ "xch",
	/* 15 */ "spl"  
};

/////////////////////////////////////////////////////////////////
//
//  It appears that we're blowing the call stack of the 65C02.
//  So in an effort to sidestep nested calls, let's break this
//  into two separate pieces:
//
//  First, see if we can build a list of parsed cells.
//
//  If we can do that, then we can load them in a loop.
//  Maybe.
//
/////////////////////////////////////////////////////////////////
Cell program[256]; // up to 256 parsed instructions
unsigned char programSize;


unsigned int location; // where the parsed instructions should go

Cell tempCell;
char buffer[LINE_BUFFER_SIZE];
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

void cell_loadFile(char *filename)
{
#ifdef X16
    //
    //  Load file into Bank 1
    // 
    setBank(1);
    bankAddress = 0xa000;
    memset( (unsigned char*) bankAddress, 0x00, 4096);
    cprintf("loading %s\r\n", filename);
    cbm_k_setnam(filename);
    cbm_k_setlfs(IGNORE_LFN,EMULATOR_FILE_SYSTEM,SA_IGNORE_HEADER);
    cbm_k_load(LOAD_FLAG, bankAddress);
#else
    FILE *fp = fopen(filename, "r");
    int ok;

    while (fgets(buffer, LINE_BUFFER_SIZE, fp) != NULL)
       if (cell_load(buffer) != INVALID_OPCODE)
       {
          arena_setLocation(location, &tempCell);
          location++;
       }

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
       if (PEEK(bankAddress) == eof) // we are SO done
          return eof;

       if (PEEK(bankAddress) == eoln)  // line is ready?
       {
          ++bankAddress; // eat end of line
          if (buffer_position > 0) // yeah, line is ready.
            return eoln;
       }

       buffer[buffer_position] = PEEK(bankAddress);
       
       if (buffer[buffer_position] > 96) 
          buffer[buffer_position] -= 32; // to PETSCII uppercase

       ++bankAddress;
       ++buffer_position;
   }

   if (buffer_position == LINE_BUFFER_SIZE) // eat rest of line
      while((PEEK(bankAddress) != eoln) || (PEEK(bankAddress) != eof))
         ++bankAddress;

   return 1;
}
#endif

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
          if (cell_load(buffer) != INVALID_OPCODE )
             cell_storeInProgram();
          else
             cprintf("fail\r\n");
       }
    }
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

void cell_decode_operand(char *src, unsigned char *mode, unsigned int *val)
{
   int rawValue;
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

/*
 
    Load the instruction into *tempCell

 */
unsigned char cell_load(char *input)
{
    char opcode[3] = "";
    char a[8] = ""; 
    char b[8] = "";
    unsigned char opcode_value;

    unsigned char amode = 0;
    unsigned char bmode = 0;
    unsigned int aval   = 0;
    unsigned int bval   = 0;
    
    if (sscanf(input, " %3s %s %s", opcode, a, b) != 3)
        return INVALID_OPCODE;
    
    if (opcode[0] == ';') // comment!
        return INVALID_OPCODE;

    cell_decode_operand(a, &amode, &aval);
    cell_decode_operand(b, &bmode, &bval);

    opcode_value = cell_encode_opcode(opcode);
    tempCell.opcode  = opcode_value;
    tempCell.aMode  = amode;
    tempCell.A      = aval;
    tempCell.bMode  = bmode;
    tempCell.B      = bval;

    return opcode_value;
}

char* getOpcodeName(Cell *cell)
{
    return opcodes[cell->opcode];
}

char  getMode(unsigned char mode)
{
    return modes[mode];
}
