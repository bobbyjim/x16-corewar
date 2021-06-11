/* program loader for MARS
   12/16/88    --- NCP   */

#include "interp.h"
#include <stdio.h>
#include <memory.h>
#include <malloc.h>

#define IO(i,j) if((i) < (j)){printf("error reading file\n");exit(1);}
extern	cell	a[];
extern	stream	*exe[];

/* generates starting position */
/* debugging non random version */
int	startposition()
{
/*  debugging code:
	static int	i = 0;

	if (i == 0)
	{
		i = 1;
		return(0);
	}
	else
		return((int) SIZE / 2)
*/
	return(rand()%SIZE);
}


load(f, no)
FILE	*f;		/* file */
int	no;		/* player number */
{
	memory	ele[MAXINST];
	int	i;		/* number of instructions */
	int	test;		/* test instructions read */
	int	start;		/* starting instruction */
	int	position;	/* loading position */
	int	counter;	/* general-purpose counter */

	test = fread(&i, sizeof(int), 1, f);
	IO(test, 1)
	printf("Found %d/%d instructions\n", i, MAXINST);
	if (i > MAXINST) {
		printf("load: Oops!  Too large!!\n");
		return(1);	/* oops - too large */
	}

	test = fread(&start, sizeof(int), 1, f);
	IO(test, 1)

	test = fread(ele, sizeof(memory), i, f);
	IO(test, i)

	/* get a new starting position */
	do {
		position = startposition();
		printf("Trying position %d\n", position);
	} while (testpos(position,i));

	/* and load the code there */
	printf("Loading %d instructions at location %d\n", i, position);
	for (counter = 0; counter < i; counter++) {
		memcpy(&a[(counter+position)%SIZE],&ele[counter],sizeof(memory));
#ifdef DEBUG
		if (!(((counter+position)%SIZE)%10)) {
			printf("%d%",(counter+position)%SIZE);
		} else {
			putchar('.');
		}
#endif
		a[(counter+position)%SIZE].lastmod = no;
		printf("Contents of location %d: %d %d %d %d %d %d\n",
			(counter+position)%SIZE,
			a[(counter+position)%SIZE].inst,
			a[(counter+position)%SIZE].para1,
			a[(counter+position)%SIZE].para2,
			a[(counter+position)%SIZE].m1,
			a[(counter+position)%SIZE].m2,
			a[(counter+position)%SIZE].lastmod);
	}

	if ((exe[no] = (stream *) malloc(sizeof(stream))) == NULL) {
		printf("load: Can't malloc the PC - aborting\n");
		exit(1);
	} else {
		exe[no]->pc = (position + start) % SIZE;
		exe[no]->next = exe[no]->prev = exe[no];
#ifdef DEBUG
		printf("Starting PC: %d %d %d\n", exe[no]->pc, exe[no]->next->pc, exe[no]->prev->pc);
#endif
	}

	return(0);	/* all OK */
}

/* tests for empty segment of *no* bytes starting at *start* */
testpos(start,no)
int	start,no;
{
	int	counter;	/* general-purpose counter */

	for (counter = 0; counter < no; counter++) {
#ifdef DEBUG
		if (!(((counter+start)%SIZE)%10)) {
			printf("%d%",(counter+start)%SIZE);
		} else {
			putchar('.');
		}
#endif
		if (a[(counter+start)%SIZE].lastmod) {
			printf("testpos: Oops -- something at %d\n",(counter+start)%SIZE);
			return(1);  /* oops - clash */
		}
	}
	return(0);	/* all clear - segment is free */
}
