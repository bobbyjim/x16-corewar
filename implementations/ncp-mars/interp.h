/* MARS redcode interpreter:
	Header file
	Started December '88
	Na Choon Piaw		*/

/* instruction set */
typedef enum { dat, mov, add, sub, jmp, jmz, jmn, djn, cmp, spl } instr;

/* addressing modes */
typedef enum { immed, direct, indirect } mode;

/* this is the new definition of a memory cell */
typedef struct
{
	instr	inst;		/* instruction */
	int	para1, para2;	/* first parameter, second parameter */
	mode	m1, m2;		/* addressing modes */
	int	lastmod;	/* last modified by */
} cell;

/* old assembler definition of memory cell */
typedef struct
{
	instr	inst;
	int	para1, para2;
	mode	m1, m2;
} memory;

/* doubly circularly linked list for streams of execution */
typedef struct tag0
{
	int		pc;	/* program counter */
	struct tag0	*next, *prev;
} stream;

/* interpreter limits */
#define MAXINST	200		/* maximum number of instructions */
#define SIZE	8000		/* size of array */
#define MAXPLAY	3		/* maximum number of players */
#define RANDIVISOR 33
