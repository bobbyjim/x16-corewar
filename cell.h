#ifndef _CELL_H_
#define _CELL_H_

typedef struct {
	unsigned char opcode: 4;
/*
	unsigned char flag1 : 1;
	unsigned char flag2 : 1;
	unsigned char flag3 : 1;
	unsigned char flag4 : 1;
*/
	unsigned char aMode : 2;
	unsigned int A  	: 14; 
	unsigned char bMode : 2;
	unsigned int B  	: 14; 
} Cell;

#define			IMMEDIATE					0
#define			DIRECT						1
#define			INDIRECT					2
#define			PREDECREMENT_INDIRECT		3

#define			HCF				0
#define			MOV				1
#define			ADD				2
#define			SUB				3
#define			JMP				4
#define			JMZ				5
#define			JMN				6
#define			CMP				7
// these two are the same ---------
#define			SEQ				7
#define			SLT				8
#define			SNE				9
#define			FLP				10
#define			DJN				11
#define			INC				12
#define			DEC				13
#define			XCH				14
#define			SPL				15
#define			INVALID_OPCODE	16

/*
There was discussion on the rec.games.corewar in 1991 about limiting the 
address "window" that an instruction could access to something less than 
the full core.  

There was also this from A.K. Dewdney in March 1985:

...the range-limitation rule suggested by William J. Mitchell of the mathematics 
department at Pennsylvania State University. ... Mitchell's suggestion requires some 
elaboration. Allow each battle program to alter the contents of any location up to a 
distance of some fixed number of addresses. Such a rule automatically keeps DWARF 
from doing any damage outside this neighborhood. The rule has many other effects as 
well, including a strong emphasis on movement. How else can a battle program get within 
range of an opponent? The rule has much merit and I hope that some of the many readers 
with a Core War system of their own will give it the further exploration it deserves.
*/

/*

   I have a confession.  I am a huge fan of the Motorola 68xxx family of
microprocessors.  My apologies to anyone with some interest or sympathy for
segmented architectures, but I just think they are plain dumb and no matter
what numbers or figures or reasoning you may throw at me I will probably go
to the grave with that bias (so please don't try to change my closed mind)!
But, I am in favor of adding a kind of segmentation to Core War to improve
game play.  On the other hand, I do not want segment size set in stone.  If
I want core size to be 529361 and segment size to be 529361, I should have
that option.

- Mark Durham, 24 Nov 91, rec.games.corewar

*/

/*
    I just wrote a very hasty Perl script which counts the use of operands
	in core warrior code shown in every post of rec.games.corewar from November
	1991 through December 2006.  This frequency was observed -- it is almost
	guaranteed to be inaccurate, but the general trend and usage is incontrovertible.

	MOV: 4859
	DAT: 3852
	JMP: 2997
	ADD: 2480
	SPL: 2069

	JMZ: 923
	JMN: 906

	CMP: 773	CMP was the only Skip instruction until 1994.

	DJN: 638	enables tight looping

	SUB: 237    This would be more valuable if DJN went away.
	SNE: 169	I think this is more useful than CMP.

	SLT: 81		not significant
	NOP: 51		not significant
	MUL: 26		not significant
	MOD: 25		not significant
	DIV: 16		not significant
*/

/*
SPL was added to Redcode before anyone truly knew what it would mean
to the game.  Is SPL good or bad for the game?  Makes no difference
really, we should have found out before we added it to the standard.

In ICWS'88, the SPL instruction was changed so that new tasks are added 
to the end of the task queue instead of the beginning in order to make 
SPL 0-type programs less effective.

- Mark Durham, 24 Nov 1991, rec.games.corewar 
*/

/*
...In Core Wars '86 standard it was far easier to write large and sophisticated 
programs which would not lose to small sophisticated programs never mind brain-less 
two liners. I had a whole suite of programs with lengths ranging from 100-300 lines 
in length which beat everything from dwarf/dervish/7 League Boot (as my friend named 
his) to programs in the nature of XTC but far superior (written by another friend).
They absolutely do not convert easily to '88 standard.

- Campbell Fraser, 10 Dec 1991, rec.games.corewar
*/

Cell* getCell(unsigned char index);

void cell_setLocation(unsigned int destination);
void cell_loadFile(char *filename);
void cell_parseBank();

void cell_resetProgram();
void cell_storeInProgram();
void cell_copyProgramIntoCore();


void loadProgram(char *buffer[], unsigned char bufsiz, int startLocation);
unsigned char cell_encode_opcode(char *opcode);
//unsigned char cell_parseInstruction(char *input);
//unsigned char cell_loadInstruction(int location, char* input);
char* getOpcodeName(unsigned char code);
char  getMode(unsigned char mode);

#endif 
