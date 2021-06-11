
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "cell.h"
#include "arena.h"
#include "common.h"

char* opcodes[16] = {
	"hcf", // halt, catch fire
	"mov",
	"add",
	"mul",
	"div",
	"mod", 
	"...", 
    "...",
	"jmn", // jmp != 0
	"jmz", // jmp == 0
    "djn",
    "djz",
	"ske", // skip if == 
	"slt", // skip if <
    "...",
	"spl"  // split 
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
        '#',    // value
        ' ',    // address 
        '@',    // address indirect 
        '<'     // address indirect predecrement
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
            *mode = 0;
            ++src;
            break;

       case '@': 
            *mode = 2;
            ++src;
            break;

       case '<': 
            *mode = 3;
            ++src;
            break;

       default:  
            *mode = 1;
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

void buildTempCell(char *input)
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
    // SUB isn't a true opcode; it's a negative ADD.
    //
    if EQ(opcode, "SUB") 
    {
        strcpy(opcode, "ADD");   // switch to ADD
        aval = CORESIZE - aval;  // invert the value
    }

    //
    // JMP isn't a true opcode; it's a JMZ #0 B.
    //
    if EQ(opcode, "JMP")
    {
        strcpy(opcode, "JMZ");
        amode = 0; // #
        aval  = 0; // 0
    }

    tempCell.opcode  = encode(opcode);
    tempCell.aMode  = amode;
    tempCell.A      = aval;
    tempCell.bMode  = bmode;
    tempCell.B      = bval;
}