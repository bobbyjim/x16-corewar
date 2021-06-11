/*
 *	General defines for CoreWar, based on A.K. Dewdney's May 1984
 *	column in Scientific American.
 *
 *	Copyright 1984, Berry Kercheval.  All rights reserved.  Permission
 *	for distribution is granted provided no direct commercial
 *	advantage is gained, and that this copyright notice appears 
 *	on all copies.
 */

#define TIMEOUT 120		/* only play for two minutes */
struct	memword
	{
		short op;
		short a_mode;
		short b_mode;
		int   a;
		int   b;
	};

typedef struct memword memword;
typedef	long	address;

#define 	MEMSIZE		8000
#define 	DISTANCE	1000	/* programs at laest this far apart */

/*
 *	Redcode Instruction Format:
 *
 *	N    N     N   NNNN NNNN	<-- decimal digits
 *	OP ModeA ModeB AAAA BBBB
 *
 *	Max. instruction: 82279997999 = 0x1 3245 222F
 *	Too bad it won't fit in a Long
 *
 */

/*
 * The redcode Op-code values
 */

#define		MOV	1
#define		ADD	2
#define		SUB	3
#define		JMP	4
#define		JMZ	5
#define		JMG	6
#define		DJZ	7
#define		CMP	8
#define		DAT	0

char *op_str[] =
{
	"dat",	/* 0 */
	"mov",	/* 1 */
	"add",	/* 2 */
	"sub",	/* 3 */
	"jmp",	/* 4 */
	"jmz",	/* 5 */
	"jmg",	/* 6 */
	"djz",	/* 7 */
	"cmp",	/* 8 */
	0
};
	
/*
 * Operand modes
 */

#define IMMEDIATE	0
#define DIRECT		1
#define INDIRECT	2

char mode_char[] = { '#', ' ', '@', 0};
