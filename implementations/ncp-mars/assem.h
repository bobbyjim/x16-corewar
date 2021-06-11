/*	Copyrighted (C) 1989 by Na Choon Piaw.  All rights reserved       */



/*	This program and documentation is Public Domain, and may be	  */
/*	distributed and copied by everyone provided this header		  */
/*	remains intact							  */

/* MARS redcode assembler:
	Header file
	Restarted in Novemeber '88
	Na Choon Piaw		*/

/* note that tag is just a generic name for something that I can't think
   of a name for.                            ---- CP                 */

/* define instruction set */
typedef enum { dat, mov, add, sub, jmp, jmz, jmn, djn, cmp, spl } instr;

/* define addressing modes */
typedef enum { immed, direct, indirect } mode;

/* define structure of an instruction */
/* NOTE: this will probably differ from the interpreter's version */
typedef struct
{
	instr	inst;		/* instruction */
	int	para1,para2;	/* first parameter, second parameter */
	mode	m1,m2;		/* addressing modes for parameters */
} memory;	/* memory element */

/* define compiler limits */
#define MAXINST	200	/* maximum number of instructions */
#define SYMBOLS 100	/* maximum number of symbols in symbol table */
#define MAXBUFFER 256	/* maximum size of string buffer */
#define COMMENT ';'	/* comment character */

/* linked list of tokens for tokenizer and assembler to work on */
typedef struct tag0
{
	char		*token;		/* token as a string */
	struct tag0	*next;		/* next token */
} tokenlist;

/* symbol table structure */
typedef struct
{
        char    *symbol;                /* pointer to string of symbol */
        int     position;               /* position the symbol belongs to */
} tag1;

/* now to define the strings that the assembler recognizes */
#define MOV	"MOV"
#define ADD	"ADD"
#define SUB	"SUB"
#define JMP	"JMP"
#define JMZ	"JMZ"
#define JMN	"JMN"
#define DJN	"DJN"
#define CMP	"CMP"
#define SPL	"SPL"
#define DAT	"DAT"
