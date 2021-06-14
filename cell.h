#ifndef _CELL_H_
#define _CELL_H_

typedef struct {
	unsigned char opcode: 4;
	unsigned char aMode : 2;
	unsigned char bMode : 2;
	unsigned int A  	: 12; 
	unsigned int B  	: 12;
} Cell;

#define			HCF				0
#define			MOV				1
#define			ADD				2
#define			SUB				3
#define			MUL				4
#define			DIV				5
#define			MOD  			6
// 7
#define			JMN				8
#define			JMZ				9
#define			DJN				10
#define			DJZ				11
#define			SKE				12
#define			SLT				13
#define			XCH				14
#define			SPL				15


#define			IMMEDIATE					0
#define			DIRECT						1
#define			INDIRECT					2
#define			PREDECREMENT_INDIRECT		3

void printCell(Cell *cell, char *postfix);
unsigned char loadCell(char *input, int location);
unsigned char buildTempCell(char *input);
Cell* getTempCell();

#endif 