
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "cell.h"
#include "arena.h"
#include "common.h"

char* opcodes[16] = {
	/*  0 */ "hcf", 
	/*  1 */ "mov",                            
	/*  2 */ "add",
    /*  3 */ "sub",
	/*  4 */ "mul",
	/*  5 */ "div",
	/*  6 */ "mod",  
    /*  7 */ "???",
	/*  9 */ "jmz", 
	/*  8 */ "jmn", 
    /* 10 */ "djn",
    /* 11 */ "djz",
	/* 12 */ "ske", 
	/* 13 */ "slt", 
    /* 14 */ "xch",
	/* 15 */ "spl"  
};

Cell tempCell;

Cell* getTempCell() { return &tempCell; }

unsigned char encode(char *opcode)
{
    unsigned char x = 16;
    while(--x)
       if (!strcmp(opcode, opcodes[x])) return x;

//    if EQ(opcode, "nop") return MOV;
    if EQ(opcode, "cmp") return SKE;

    return 0;
}

char modes[] = { 
        '#',    // value (0)
        ' ',    // address (1)
        '@',    // address indirect (2)
        '<'     // address indirect predecrement (3)
};

void printCell(Cell *cell, char* postfix)
{
    cprintf("%s    %c%-5u  %c%-5u", 
        opcodes[cell->opcode], 
        modes[cell->aMode],
        cell->A,
        modes[cell->bMode],
        cell->B
        );

    if (*postfix) 
       cputs(postfix);
}

void decodeOperand(char *src, unsigned char *mode, unsigned int *val)
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
   rawValue %= CORESIZE;
   
   *val = rawValue; 
}

unsigned char loadCell(char *input, int position)
{
    buildTempCell(input);
    setLocation(position, &tempCell);
    return 0;
}

unsigned char buildTempCell(char *input)
{
    char opcode[3];
    unsigned char amode;
    char a[8]; 
    unsigned char bmode;
    char b[8];
    unsigned int aval;
    unsigned int bval;
    
    sscanf(input, "%s %s %s", opcode, a, b);

    decodeOperand(a, &amode, &aval);
    decodeOperand(b, &bmode, &bval);

    //
    // JMP isn't a true opcode; it's a JMZ #0 B.
    //
    if EQ(opcode, "jmp")
    {
        strcpy(opcode, "jmz");
        amode = 0; // #
        aval  = 0; // 0
    }

    tempCell.opcode  = encode(opcode);
    tempCell.aMode  = amode;
    tempCell.A      = aval;
    tempCell.bMode  = bmode;
    tempCell.B      = bval;
    return 1;
}