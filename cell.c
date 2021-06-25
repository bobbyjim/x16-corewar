
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cell.h"
#include "arena.h"
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

Cell tempCell;
Cell* getTempCell() { return &tempCell; }
unsigned char loadedOpcode;

unsigned char encode(char *opcode)
{
    unsigned char x = 16;
    while(--x)
       if (!strcmp(opcode, opcodes[x])) 
           return x;

    if EQ(opcode, "cmp") return SEQ;
    if EQ(opcode, "dat") return HCF;

    return INVALID_OPCODE;
}

char modes[] = { 
        '#',    // value (0)
        ' ',    // address (1)
        '@',    // address indirect (2)
        '<'     // address indirect predecrement (3)
};

//
//  Rather than support the hackneyed parser here, I should
//  scrap all this and just support the "object" format.
//  This would free up needed RAM.
//
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

//       case '<': 
//            *mode = PREDECREMENT_INDIRECT;
//            ++src;
//            break;

       default:  
            *mode = DIRECT;
            break;
   }
   sscanf(src, "%d", &rawValue); // allows negative sign

   // convert raw value to core location
   while (rawValue < 0) rawValue += CORESIZE; // inefficient
   
   *val = rawValue; 
}

void loadProgramFromFile(char *name, unsigned int location)
{
    char buffer[LINE_BUFFER_SIZE];

#ifdef X16
#include    <conio.h>
#include    <peekpoke.h>

    int line;
    int x;

    unsigned int address = 0xa000;

    POKE(0x9f61, 1); // bank 1 r38-
    POKE(0x0000, 1); // bank 1 r39+

    cprintf("loading at %u\r\n", location);

    cbm_k_setnam(name);
    cbm_k_setlfs(IGNORE_LFN,EMULATOR_FILE_SYSTEM,SA_IGNORE_HEADER);
    cbm_k_load(LOAD_FLAG, address);

    for(line=0; line<MAX_WARRIOR_LINES; ++line)
    {
        // build up the buffer
        for(x=0; x<LINE_BUFFER_SIZE; ++x)
        {
            buffer[x] = PEEK(address); 
            if (buffer[x] > 96) buffer[x] -= 32;
            ++address;
            if (buffer[x] == 0x0a) // done
            {
                buffer[x] = '\0';
                break; // out of the buffer loop
            }
        }
        if (buffer[0] == ';') // this is just a comment
        {
            cputs(buffer);
            cputs("\r\n");
        }
        else
        if ( (strlen(buffer) > 0) && (loadCell(buffer, location) != INVALID_OPCODE) )
           location++;
    }
#else
    FILE *fp = fopen(name, "r");
    int ok;
    int i;

    printf("loading at %u\n", location);
    do
    {
        ok = 1;
        if (fgets(buffer, LINE_BUFFER_SIZE, fp) != NULL)
        {
           if (loadCell(buffer, location) != INVALID_OPCODE)
              location++;
        }
        else
           ok = 0;
    }
    while(ok);
    fclose(fp);
#endif
}

/*
 
    Return the loaded opcode

 */
unsigned char loadCell(char *input, int position)
{
    unsigned char value;
    value = buildTempCell(input);

    if (value != INVALID_OPCODE)
        setLocation(position, &tempCell);

    return value;
}

unsigned char buildTempCell(char *input)
{
    char opcode[3] = "";
    char a[8] = ""; 
    char b[8] = "";
//    char* label;

    unsigned char amode = 0;
    unsigned char bmode = 0;
    unsigned int aval   = 0;
    unsigned int bval   = 0;
    
//    if (sscanf(input, "%s %s %s %s", label, opcode, a, b) == 4)
//        cprintf("label found: %s\r\n", label);
    if (sscanf(input, " %3s %s %s", opcode, a, b) != 3)
        return INVALID_OPCODE;

    if (opcode[0] == ';') // comment!
        return INVALID_OPCODE;

    decodeOperand(a, &amode, &aval);
    decodeOperand(b, &bmode, &bval);

    tempCell.opcode  = encode(opcode);
    tempCell.aMode  = amode;
    tempCell.A      = aval;
    tempCell.bMode  = bmode;
    tempCell.B      = bval;

    return tempCell.opcode;
}

char* getOpcodeName(Cell *cell)
{
    return opcodes[cell->opcode];
}

char  getMode(unsigned char mode)
{
    return modes[mode];
}
