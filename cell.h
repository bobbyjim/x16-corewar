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
#define			SKE				10
#define			SKN				11
// 12
// 13
// 14
#define			SPL				15

void printCell(Cell *cell, char *postfix);
void parseCell(char *input, int location);

#endif 