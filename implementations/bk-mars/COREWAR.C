/*
 *	CoreWar
 *
 *	Programs battle for supremacy in a simulated arena!  Based on A.K
 *	Dewdeney's May 1984 column in Scientific American.
 *
 *	Usage:
 *		corewar [-l][-d][-t] prog1 prog2
 *			prog1 and prog2 must be ascii redcode files.
 *		    -l: print listing while loading the redcode programs.
 *		    -t: print verbose execution trace
 *		    -d: print post-mortem memory dump.
 *
 *	This program assumes your C compiler supports structure assignment.
 *
 *	Copyright 1984, Berry Kercheval.  All rights reserved.  Permission
 *	for distribution is granted provided no direct commercial
 *	advantage is gained, and that this copyright notice appears 
 *	on all copies.
 */

#include <stdio.h>
#include <signal.h>
#include "corewar.h"

memword	mem[MEMSIZE];		/* the simulated memory 'arena' */

char	*pr_inst();		/* returns formatted string representation of a
				 * redcode instruction suitable for %s */

int	dumpflag = 0;		/* TRUE --> print post-mortem dump */
int	traceflag = 0;		/* TRUE --> print execution trace */
int	listflag = 0;		/* TRUE --> print listing of loaded program */

main (argc, argv)
int argc;
char **argv;
{
	char 	*file1 = NULL;
	char	*file2 = NULL;
	address	pc_1, pc_2;	/* addresses of program starting points. 
				 * filled in by load() */
	address load();		/* loads a file */
	int	winner;		/* who won? */
	int	draw();		/* what happens if we time out */

	while (*++argv && **argv == '-')
		switch (*++*argv) {
		case 'l':	/* print listing at load-time */
			listflag = 1;
			break;
		case 'd':	/* print memory dump at end */
			dumpflag = 1;
			break;
		case 't':	/* print execution trace */
			traceflag = 1;
			break;
		default:	/* huh??? */
			printf("Unknown flag '%s'\n",*argv);
			break;
		}
	/* now the last two arguments should be names of two redcode files */
	if (*argv) {
		if ( access (*argv, 4) != 0) { /* check read access */
			perror(*argv);
			exit (-1);
		}
		file1 = *argv;
	}
	else {
		printf ("Usage: corewar prog1 prog2\n");
		exit(-1);
	}
	argv++;
	if (*argv) {
		if ( access (*argv, 4) != 0)  { /* check read access */
			perror(*argv);
			exit (-1);
		}
		file2 = *argv;
	}
	else {
		printf ("Usage: corewar prog1 prog2\n");
		exit(-1);
	}
	srand(getpid());
			
	clear_mem();			/* Clear the arena! Clear the arena! */
	pc_1 = load(  -1, file1); 	/* load file1 anywhere */
	pc_2 = load(pc_1, file2); 	/* load file2 1000 away from pc_1 */
	signal ( SIGALRM, draw);	/* set up for timeout */
	alarm (TIMEOUT);		/* give up after TIMEOUT seconds */
	if(traceflag)			/* print trace header here because
					 * we know the filenames here */
	{
		printf ("       %10s                                 %10s\n",
			file1, file2);
		printf("-----------------------------------------------------------\n");
	}
	winner = execute(pc_1, pc_2);	/* run the programs */
	alarm(0);			/* turn off the alarm */
	switch (winner)			/* allocate blame */
	{
		case 0:	printf ("DRAW!\n"); break;

		case 1:
		case 2: printf ("%s(%d) wins.\n", winner==1?file1:file2,winner);
			break;

		default: printf ("Error in execute -- illegal return value\n");
	}
	if (dumpflag)			/* post-mortem dump if requested */
		dump();
	exit(0);			/* Th-th-that's all folks */
}

/*
 *	clear_mem:	clear the simulated memory
 */

clear_mem()
{
	address i;

	for ( i=0 ; i < MEMSIZE; i++ )
	{
		mem[i].op = 0;
		mem[i].a_mode = 0;
		mem[i].b_mode = 0;
		mem[i].a = 0;
		mem[i].b = 0;
	}
}

/*
 *	load:
 *		Load a redcode file.  If where is not -1, place the loaded file
 *		at a random place im mem, NOT WITHIN   'DISTANCE' of 'where'
 *		If where IS -1, put the code anywhere.
 */

address
load(where, filename)
address where;
char *filename;
{
	address	r;		/* where to load current instruction */
	FILE	*f;		/* stream pointer for input file */
	char	buf[256];	/* line buffer */
	char 	*ptr;		/* pointer into the line; used in parsing */
	char	*ip;		/* pointer into the line; used in parsing */
	char	*index();
	char	error;		/* error flag printed at beginning of each 
				 * listing line; currently either ' ' or 'E' */
	address	start;		/* address of first executable instruction */
	int	op;		/* op-code value */
	int	i;		/* counter for for loops */

	/* select starting address */

	start = 0;
	r = (rand() >> 12) % MEMSIZE;	/* 4.1 rand() sucks */
	if (where != -1)
	{
		/* second program must be at least DISTANCE addresses from
		 * first; I just make their load-points DISTANCE apart 
		 */
		while ( abs (r - where) < DISTANCE )
			r = (rand() >> 12) % MEMSIZE;
	}
	printf ("loading %s at %d\n", filename, r);

	/* now do load */

	f = fopen (filename, "r");
	if (f == NULL ) {
		perror (filename); exit(-1);
	}
	/*
	 * There now follows a moderately crufty ad-hoc redcode assembler.
	 * It's not modular or very structured, but it seems to work, and
	 * redcode was so simple I didn't want to use YACC or LEX or SSL
	 */

	while ( fgets ( buf, 256, f) != NULL)	/* for each line in the file */
	{
		error = ' ';			/* no error yet */
		/* zap trailing newline to make listing generation easier */
		if ( (ptr = index(buf, '\n')) != NULL ) *ptr = '\0';
		/* zap comment */
		if ( (ptr = index(buf, '/')) != NULL ) *ptr = '\0';
		 
		/* decode instruction */

		ip = buf;		/* start at the beginning of the line */
		op = -1;		/* Invalid op-code */
		/* skip leading whitespace */
		while ( *ip && (*ip == ' ' || *ip == 011)) ip++;	
		if ( ip == ptr ) {	/* it's a 'blank' line */
			if ( ptr != NULL ) *ptr = '/';	/* put comment back */
			if (listflag)
				printf ("%c                      '%s'\n",
					error, buf);
			break;
		}
		for ( i = 0; i <= CMP; i++)	/* CMP is highest op-code */
		{
			if ( strncmp(ip, op_str[i], 3) == 0){
				op = i;
				break;
			}
		}
		if ( op == -1 ) 		/* didn't find it! */
		{
			printf ("SYNTAX ERROR '%s' -- Bad opcode\n", buf);
			error = 'E';
		}
		mem[r].op = op;
		ip += 3;		/* skip over opcode mnemonic */
		/* skip whitespace */
		while ( *ip && (*ip == ' ' || *ip == 011)) ip++;	
		/* figure out addressing mode for operand A */
		if ( op != DAT ) { 	/* DAT has only B operand */
			if ( *ip == '#'){
				mem[r].a_mode = IMMEDIATE; 
				ip++;
			}
			else if ( *ip == '@') {
				mem[r].a_mode = INDIRECT; 
				ip++;
			}
			else 
			{
				mem[r].a_mode = DIRECT;
			}
			mem[r].a = atoi ( ip );
			if ( mem[r].a < 0 ) mem[r].a += MEMSIZE;
			while ( *ip && (*ip != ' ' && *ip != 011)) ip++;
		}
		/* skip whitespace */
		while ( *ip && ( *ip == ' ' || *ip == 011)) ip++;	

		if ( op != JMP ) {		/* JMP has only A operand */
			if ( *ip == '#'){
				mem[r].b_mode = IMMEDIATE; 
				ip++;
			}
			else if ( *ip == '@') {
				mem[r].b_mode = INDIRECT; 
				ip++;
			}
			else 
			{
				mem[r].b_mode = DIRECT;
			}
			mem[r].b = atoi ( ip );
			if ( mem[r].b < 0 ) mem[r].b += MEMSIZE;
		}

		/* check for start of executable code */
		if ( op != DAT && start == 0) start = r;
		/* DAT has zero modes... */
		if ( op == DAT ) mem[r].b_mode = mem[r].a_mode = 0;

		/* Do listing stuff... */

		if ( ptr != NULL ) *ptr = '/';		/* put comment back */
		if (listflag)
			printf ("%c%4d %s  '%s'\n", error, r, pr_inst(mem[r]),
				buf);
		r++;			/* Advance to next memory location */
		if ( r > MEMSIZE ) r = r % MEMSIZE;
	}

	if (listflag)
		fflush (stdout );
	fclose (f);
	/* return starting address */
	return start;
}

/*
 * Execute the two loaded Redcode programs starting at addresses p1 and p2
 * until one executes an illegal instruction
 */

execute (p1, p2)
address p1;
address p2;
{
	printf ("executing: p1 = %d, p2 = %d\n", p1, p2);
	for(;;)
	{
		if ( -1 == (p1 = do_instruction(p1))) return 1;	
		if (p1 >= MEMSIZE) p1 = p1 % MEMSIZE;

		/* separate the two instruction traces */
		if (traceflag)
			printf ("     ||     ");

		if ( -1 == (p2 = do_instruction(p2))) return 1;	
		if (p2 >= MEMSIZE) p2 = p2 % MEMSIZE;

		/* do_instruction prints a trace, which needs to be
		 * 'newlined' here */
		if(traceflag)
			printf ("\n");
	}
}

/*
 * interpret one instruction at addr; return address of next instruction to
 * be executed, or -1 if it was an illegal instruction
 */

do_instruction(addr)
address addr;
{
	memword inst;

	if(traceflag)
		printf ("@ %d", addr); fflush (stdout);
	/* fetch instruction */
	inst = mem[addr];
	if (traceflag)
		printf ("  %s %c%4d, %c%4d    ", op_str[inst.op], 
			mode_char[inst.a_mode], inst.a,
			mode_char[inst.b_mode], inst.b); 
	fflush (stdout);
	switch ( inst.op )
	{
		case MOV:
			if (do_mov(addr, inst)) return -1; else return addr + 1;
		case ADD:
			if (do_add(addr, inst)) return -1; else return addr + 1;
		case SUB:
			if (do_sub(addr, inst)) return -1; else return addr + 1;
		case JMP:
			return (addr + inst.a)%MEMSIZE;	
		case JMZ:
			if ( mem[inst.b].op == DAT && mem[inst.b].b == 0 )
				return (addr + inst.a)%MEMSIZE;	
			else
				return (addr + 1)%MEMSIZE;
		case JMG:
			if ( mem[inst.b].op == DAT && mem[inst.b].b > 0 )
				return (addr + inst.a)%MEMSIZE;	
			else
				return (addr + 1) % MEMSIZE;
		case DJZ:
			mem[inst.b].b -= 1;
			if ( mem[inst.b].b == 0 )
				return (addr + inst.a) % MEMSIZE;	
			else
				return (addr + 1) % MEMSIZE;
		case CMP:
			return addr + do_cmp(addr, inst);

		case DAT:
		default:
			printf ("Illegal instruction %s @ %d\n", 
				pr_inst(inst), addr); 
			return -1;

	}
}

do_mov(addr, inst)
address addr;
memword inst;
{
	address source, destination;
	memword data;

	data.op = 0;
	data.a_mode = data.b_mode = 0;
	data.a = data.b = 0;

	switch (inst.a_mode)
	{
		case IMMEDIATE:
			data.b = inst.a;
			break;
		case DIRECT:
			data = mem[(addr + inst.a) % MEMSIZE];
			break;
		case INDIRECT:
			source = mem[(addr + inst.a) % MEMSIZE].b;
			data =   mem[(source + addr + inst.a) % MEMSIZE];
			break;
		default: /* ERROR */
			printf ("do_mov: illegal addressing mode\n");
			return 1;
	}
	switch (inst.b_mode)
	{
		case IMMEDIATE: /* error */
			printf ("do_mov: illegal immediate destination\n");
			return 1;
		case DIRECT:
			mem[(addr + inst.b) % MEMSIZE] = data;
			break;
		case INDIRECT:
			destination = mem[(addr + inst.b) % MEMSIZE].b;
			mem[(destination + addr + inst.b) % MEMSIZE] = data;
			break;
		default: /* ERROR */
			printf ("do_mov: illegal addressing mode\n");
			return 1;
	}	
	return 0;
}

/*
 * CMP: compare a and b, return 1 if same, 2 if different 
 */

do_cmp(addr, inst)
address addr;
memword inst;
{
	address source, destination;
	memword data;

	data.op = 0;
	data.a_mode = data.b_mode = 0;
	data.a = data.b = 0;

	switch (inst.a_mode)
	{
		case IMMEDIATE:
			data.b = inst.a;
			break;
		case DIRECT:
			data = mem[(addr + inst.a) % MEMSIZE];
			break;
		case INDIRECT:
			source = mem[(addr + inst.a) % MEMSIZE].b;
			data =   mem[(source + addr + inst.a) % MEMSIZE];
			break;
		default: /* ERROR */
			printf ("do_add: illegal addressing mode\n");
			return 1;
	}
	switch (inst.b_mode)
	{
		case IMMEDIATE: /* error */
			if (data.a == inst.b) return 1;
			else return 2;
		case DIRECT:
			if ( data.a == mem[(addr + inst.b) % MEMSIZE].b)
				return 1;
			else return 2;
		case INDIRECT:
			destination = mem[(addr + inst.b) % MEMSIZE].b;
			if(data.b == mem[(destination+addr + inst.b)%MEMSIZE].b)
				return 1;
			else return 2;
		default: /* ERROR */
			printf ("do_add: illegal addressing mode\n");
			return 1;
	}	
}

do_sub(addr, inst)
address addr;
memword inst;
{
	address source, destination;
	memword data;

	data.op = 0;
	data.a_mode = data.b_mode = 0;
	data.a = data.b = 0;

	switch (inst.a_mode)
	{
		case IMMEDIATE:
			data.b = inst.a;
			break;
		case DIRECT:
			data = mem[(addr + inst.a) % MEMSIZE];
			break;
		case INDIRECT:
			source = mem[(addr + inst.a) % MEMSIZE].b;
			data =   mem[(source + addr + inst.a) % MEMSIZE];
			break;
		default: /* ERROR */
			printf ("do_sub: illegal addressing mode\n");
			return 1;
	}
	switch (inst.b_mode)
	{
		case IMMEDIATE: /* error */
			printf ("do_sub: illegal immediate destination\n");
			return 1;
		case DIRECT:
			mem[(addr + inst.b) % MEMSIZE].b -= data.b;
			break;
		case INDIRECT:
			destination = mem[(addr + inst.b) % MEMSIZE].b;
			mem[(destination + addr + inst.b) % MEMSIZE].b -= data.b;
			break;
		default: /* ERROR */
			printf ("do_sub: illegal addressing mode\n");
			return 1;
	}	
	return 0;
}

do_add(addr, inst)
address addr;
memword inst;
{
	address source, destination;
	memword data;

	data.op = 0;
	data.a_mode = data.b_mode = 0;
	data.a = data.b = 0;

	switch (inst.a_mode)
	{
		case IMMEDIATE:
			data.b = inst.a;
			break;
		case DIRECT:
			data = mem[(addr + inst.a) % MEMSIZE];
			break;
		case INDIRECT:
			source = mem[(addr + inst.a) % MEMSIZE].b;
			data =   mem[(source + addr + inst.a) % MEMSIZE];
			break;
		default: /* ERROR */
			printf ("do_add: illegal addressing mode\n");
			return 1;
	}
	switch (inst.b_mode)
	{
		case IMMEDIATE: /* error */
			printf ("do_add: illegal immediate destination\n");
			return 1;
		case DIRECT:
			mem[(addr + inst.b) % MEMSIZE].b += data.b;
			break;
		case INDIRECT:
			destination = mem[(addr + inst.b) % MEMSIZE].b;
			mem[(destination + addr + inst.b) % MEMSIZE].b += data.b;
			break;
		default: /* ERROR */
			printf ("do_add: illegal addressing mode\n");
			return 1;
	}	
	return 0;
}

dump()
{
	int r;
	int flag = 0;

	printf ("\n\n---------- MEMORY DUMP -------------\n");
	for ( r = 0; r < MEMSIZE; r++) {

		if ( iszero(mem[r]) && flag == 0) {
			printf ("%5d 0:0:0:0000:0000\n", r);
			flag = 1;
		}
		else if (iszero(mem[r]) && flag == 1) {
			printf (" *\n");
			flag = 2;
		}
		else if (iszero(mem[r]) && flag == 2) {
			/* skip it */
		}
		else
		{
			printf ("%5d %s\n", r, pr_inst(mem[r]));
			flag = 0;
		}
	}
}

iszero(x)
memword x;
{
	if (x.op == 0 && x.a_mode == 0 && 
	    x.b_mode == 0 && x.a == 0 && x.b == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

draw()
{
	printf ("game is a DRAW -- timed out after %d seconds\n", TIMEOUT);
	if (dumpflag)
		dump();
	exit(0);
}

char *
pr_inst(x)
memword x;
{
	char buf[200];

	sprintf(buf,"%1d:%1d:%1d:%04d:%04d",x.op,x.a_mode,x.b_mode,x.a,x.b);
	return buf;
}
