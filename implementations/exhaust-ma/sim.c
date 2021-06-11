/* sim.c: simulator functions
 * $Id: sim.c,v 1.9 2003/07/13 10:12:47 martinus Exp $
 */

/* This file is part of `exhaust', a memory array redcode simulator.
 * Copyright (C) 2002 M Joonas Pihlaja
 * Public Domain.
 */

/*
 * Thanks go to the pMARS authors and Ken Espiritu whose ideas have
 * been used in this simulator.  Especially Ken's effective addressing
 * calculation code in pMARS 0.8.6 has been adapted for use here.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exhaust.h"
#include "asm.h"
#include "sim.h"
#include "insn.h"

/* Should we strip flags from instructions when loading?  By default,
   yes.  If so, then the simulator won't bother masking them off.  */
#ifndef SIM_STRIP_FLAGS
#define SIM_STRIP_FLAGS 1
#endif


/* DEBUG level:
 *     0: none
 *   >=1: disassemble each instruction (no output)
 *     2: print each instruction as it is executed
 */

/*
 * File scoped stuff
 */

/* internal warrior structure */
typedef struct w_st {
  insn_t **tail;		/* next free location to queue a process */
  insn_t **head;		/* next process to run from queue */
  struct w_st *succ;		/* next warrior alive */
  u32_t nprocs;			/* number of live processes in this warrior */
  struct w_st *pred;		/* previous warrior alive */
  int id;			/* index (or identity) of warrior */
} w_t;

#define DEF_MAX_WARS 2
#define DEF_CORESIZE 8000
#define DEF_PROCESSES 8000
#define DEF_CYCLES 80000

/*static unsigned int modes[8] = {0, 0, 0, 0, 0, 0, 0, 0};*/

static unsigned int Coresize  = 0;
static unsigned int Processes = 0;
static unsigned int NWarriors = 0;
static unsigned int Cycles    = 0;

static w_t          *War_Tab = NULL;
static insn_t       *Core_Mem = NULL;
static insn_t       **Queue_Mem = NULL;

/* P-space */
static unsigned int PSpace_size = 0;    /* # p-space slots per warrior. */
static pspace_t	    **PSpaces;	        /* p-spaces of each warrior. */

/* protos */
static int sim_proper( unsigned int, const field_t *, unsigned int * );
static void alloc_pspaces( unsigned int nwars, unsigned int pspacesize );
static void free_pspaces( unsigned int nwars );

/*---------------------------------------------------------------
 * Simulator memory management
 */

void
sim_clear_core()
{
  memset(Core_Mem, 0, Coresize*sizeof(insn_t));
}

/* NAME
 *     sim_alloc_bufs, sim_alloc_bufs2, sim_free_bufs --
 *				alloc and free buffers used in simulation
 * 
 * SYNOPSIS
 *     insn_t *sim_alloc_bufs( unsigned int nwars, unsigned int coresize,
 *                             unsigned int processes, unsigned int cycles );
 *     insn_t *sim_alloc_bufs2( unsigned int nwars, unsigned int coresize,
 *				unsigned int processes, unsigned int cycles,
 *				unsigned int pspacesize );
 *     void sim_free_bufs();
 * 
 * INPUTS
 *     nwar        -- number of warriors
 *     coresize    -- size of core
 *     processes   -- max no of processes / warrior
 *     cycles      -- the number of cycles to play before tie.
 *     pspacesize  -- size of p-space per warrior.  For sim_alloc_bufs(),
 *		      it defaults to min(1,coresize/16).
 * 
 * RESULTS
 *     These functions manage the core, queue, and w_t warrior info
 *     struct array memories.  
 *
 *     Core_Mem, Queue_Mem, War_Tab and PSpace_mem memories are allocated
 *     or freed as requested.  Any earlier memories are freed before
 *     new allocations are made.
 * 
 * RETURN VALUE
 *     sim_alloc_bufs(): the address of core memory, or NULL if
 *			 out of memory.
 *     sim_free_bufs(): none
 *
 * GLOBALS
 *     All file scoped globals.
 */

void
sim_free_bufs()
{
  free_pspaces(NWarriors);
  if ( Core_Mem ) free( Core_Mem ); Core_Mem = NULL; Coresize = 0;
  if ( Queue_Mem ) free( Queue_Mem ); Queue_Mem = NULL; Processes = 0;
  if ( War_Tab ) free( War_Tab ); War_Tab = NULL; NWarriors = 0;
}


insn_t *
sim_alloc_bufs2(unsigned int nwars, unsigned int coresize, unsigned int processes, unsigned int cycles, unsigned int pspace )
{
  unsigned int queue_size;

  sim_free_bufs();

  Core_Mem = (insn_t*)malloc(sizeof(insn_t) * coresize);
  queue_size = nwars*processes+1;
  Queue_Mem = (insn_t**)malloc( sizeof(insn_t*)*queue_size );
  War_Tab = (w_t*)malloc( sizeof(w_t)*nwars );
  alloc_pspaces(nwars, pspace);

  if ( Core_Mem && Queue_Mem && War_Tab && PSpaces ) {
    Cycles = cycles;
    NWarriors = nwars;
    Coresize = coresize;
    Processes = processes;
    sim_clear_pspaces();
    return Core_Mem;
  }
  sim_free_bufs();
  return NULL;
}

insn_t *
sim_alloc_bufs(unsigned int nwars, unsigned int coresize, unsigned int processes, unsigned int cycles)
{
  unsigned int pspace;
  pspace = coresize/16 == 0 ? 1 : coresize/16;
  return sim_alloc_bufs2( nwars, coresize, processes, cycles, pspace );
}


/* NAME
 *     sim_load_warrior -- load warrior code into core.
 * 
 * SYNOPSIS
 *     sim_load_warrior(unsigned int pos, insn_t *code, unsigned int len);
 * 
 * INPUTS
 *     pos -- The core address to load the warrior into.
 *     code -- array of instructions.
 *     len -- 
 * 
 * DESCRIPTION
 *     This function is the preferred method to load warriors into core.
 *     It strips the instructions of any flags they may possess, and
 *     copies the instructions into core.
 * 
 *     The code will happily overwrite any previous contents of core,
 *     so beware not to load warriors so that their code overlaps.
 * 
 * NOTE
 *     The core must have been allocated previously with sim(), or
 *     preferably sim_alloc_bufs() and not freed since.
 * 
 * RETURN VALUE
 *     0 -- warrior loaded OK.
 *    -1 -- core memory not allocated.
 *    -2 -- warrior length > core size.
 */
int
sim_load_warrior(unsigned int pos, const insn_t * const code, unsigned int len)
{
  unsigned int i;
  field_t k;
  u32_t in;

  if ( Core_Mem == NULL )  return -1;
  if ( len > Coresize ) return -2;

  for (i=0; i<len; i++) {
    k = (pos+i) % Coresize;

#if SIM_STRIP_FLAGS
    in = code[i].in & iMASK;
#else
    in = code[i].in;
#endif

    Core_Mem[k].in = in;
    Core_Mem[k].a = code[i].a;
    Core_Mem[k].b = code[i].b;
  }
  return 0;
}




/*---------------------------------------------------------------
 * P-Space management.
 */

static void
free_pspaces(unsigned int nwars)
{
  unsigned int i;
  if ( nwars>0 ) {
    if (PSpaces) {
      for (i=0; i<nwars; i++) {
	pspace_free(PSpaces[i]);
      }
      free(PSpaces);
    }
  }
  PSpace_size = 0;
  PSpaces = NULL;
}


static void
alloc_pspaces(unsigned int nwars, unsigned int pspacesize)
{
  unsigned int i;
  int success = 0;

  PSpaces = NULL;
  PSpace_size = 0;
  if (nwars==0) { return; }
  
  if (( PSpaces = (pspace_t**)malloc(sizeof(pspace_t*)*nwars))) {
    success = 1;
    for (i=0; i<nwars; i++) { PSpaces[i] = NULL; }
    for (i=0; success && i<nwars; i++) {
	PSpaces[i] = pspace_alloc(pspacesize);
	success = success && PSpaces[i] != NULL;
    }
  }

  if ( !success ) {
    free_pspaces(nwars);
  } else {
    PSpace_size = pspacesize;
  }
}


/* NAME
 *     sim_get_pspaces -- get array of p-spaces of warriors.
 *     sim_get_pspace -- get a single p-space.
 * 
 * SYNOPSIS
 *     pspace_t **sim_get_pspaces(void);
 *     pspace_t *sim_get_pspace(unsigned int war_id)
 *
 * DESCRIPTION
 *     Return an array of pointers to p-space structures of the warriors.
 *     The pspace at index i is used as the pspace of the ith warrior
 *     when fighting.
 */
pspace_t **
sim_get_pspaces()
{
    return PSpaces;
}

pspace_t *
sim_get_pspace(unsigned int war_id)
{
    return PSpaces[war_id];
}

/* NAME
 *     sim_clear_pspaces -- clear and/or reinitialise p-spaces
 *     sim_reset_pspaces 
 *
 * SYNOPSIS
 *     void sim_clear_pspaces(void);
 *     void sim_reset_pspaces(void);
 *
 * DESCRIPTION
 *	All p-spaces are cleared and the P-space locations 0 are set
 *	to CORESIZE-1.  For sim_reset_pspaces(): All p-spaces are made
 *	private.
 * */

void
sim_clear_pspaces()
{
  unsigned int i;
  for (i=0; i<NWarriors; i++) {
    pspace_clear(PSpaces[i]);
    pspace_set(PSpaces[i], 0, Coresize-1);
  }
}

void
sim_reset_pspaces()
{
  unsigned int i;
  for (i=0; i<NWarriors; i++) {
    pspace_privatise(PSpaces[i]);
  }
  sim_clear_pspaces();
}



/*---------------------------------------------------------------
 * Simulator interface
 */

/* NAME
 *     sim, sim_mw -- public functions to simulate a round of Core War
 * 
 * SYNOPSIS
 *     int sim_mw( unsigned int nwar, const field_t *war_pos_tab, 
 *                 unsigned int *death_tab );
 *     int sim( int nwar, field_t w1_start, field_t w2_start,
 * 		unsigned int cycles, void **ptr_result );
 * 
 * INPUTS
 *     nwar        -- number of warriors
 *     w1_start, w2_start -- core addresses of first processes
 *                    warrior 1 and warrior 2. Warrior 1 executes first.
 *     cycles      -- the number of cycles to play before tie.
 *     ptr_result  -- NULL, except when requesting the address of core.
 *     war_pos_tab -- core addresses where warriors are loaded in
 *		      the order they are to be executed.
 *     death_tab   -- the table where dead warrior indices are stored
 * 
 * DESCRIPTION
 *     The real simulator is inside sim_proper() to which sim() and
 *     sim_mw() are proxies.  sim_mw() reads the warrior position
 *     of the ith warrior from war_tab_pos[i-1].
 *
 * RESULTS
 *     The warriors fight their fight in core which gets messed up in
 *     the process.  If a warrior died during the fight then its p-space
 *     location 0 is cleared.  Otherwise the number of warriors alive
 *     at the end of the battle is stored into its p-space location 0.
 *
 *     sim_mw() stores indices of warriors that die into the death_tab
 *     array in the order of death.  Warrior indices start from 0.
 *
 *     For sim(): If nwar == -1 then buffers of default size for
 *     max. two warriors are allocated and the address of the core
 *     memory is returned via the ptr_result pointer.
 * 
 * RETURN VALUE
 *     sim_mw(): the number of warriors still alive at the end of the
 *		 battle.
 *       -1: simulator panic attack -- something's gone wrong
 *
 *     sim(): 
 *       single warrior: 0: warrior suicided, 1: warrior didn't die.
 *       one-on-one two warriors:
 * 	      0: warrior 1 won, 1: warrior 2 won, 2: tie
 *       -1: simulator panic attack -- something's gone wrong
 * 
 * GLOBALS
 *     All file scoped globals */

int
sim_mw(unsigned int nwar, const field_t *const war_pos_tab, unsigned int *death_tab)
{
  int alive_count;
  if ( !Core_Mem || !Queue_Mem || !War_Tab || !PSpaces ) return -1;

  alive_count = sim_proper( nwar, war_pos_tab, death_tab );

  /* Update p-space locations 0. */
  if (alive_count >= 0) {
    unsigned int nalive = alive_count;
    unsigned int i;

    for (i=0; i<nwar; i++) {
      pspace_set( PSpaces[i], 0, nalive);
    }
    for (i=0; i<nwar-nalive; i++) {
      pspace_set( PSpaces[death_tab[i]], 0, 0);
    }
  }
  return alive_count;
}


int
sim( int nwar,
     field_t w1_start,
     field_t w2_start,
     unsigned int cycles,
     void **ptr_result )
{
  field_t war_pos_tab[2];
  unsigned int death_tab[2];
  int alive_cnt;

  /* if the caller requests for the address of core, allocate 
   * the default buffers and give it
   */
  if ( nwar < 0 ) {
    if ( nwar == -1 && ptr_result ) {
      *ptr_result = sim_alloc_bufs( DEF_MAX_WARS, DEF_CORESIZE,
				    DEF_PROCESSES, DEF_CYCLES );
      return 0;
    }
    return -1;
  }
  if ( nwar > 2 ) return -1;

  /* otherwise set up things for sim_mw() */
  Cycles = cycles;
  war_pos_tab[0] = w1_start;
  war_pos_tab[1] = w2_start;

  alive_cnt = sim_mw( nwar, war_pos_tab, death_tab );
  if ( alive_cnt < 0 ) return -1;

  if ( nwar == 1) return alive_cnt;

  if ( alive_cnt == 2 ) return 2;
  return death_tab[0] == 0 ? 1 : 0;
}



/*-------------------------------------------------------------------------
 * private functions
 */

/* NAME
 *     sim_proper -- the real simulator code
 * 
 * SYNOPSIS
 *     int sim_proper( unsigned int nwar,
 *                     const field_t *war_pos_tab,
 *                     unsigned int *death_tab );
 * 
 * INPUTS
 *     nwar        -- number of warriors
 *     war_pos_tab -- core addresses where warriors are loaded in
 *		      the order they are to be executed.
 *     death_tab   -- the table where dead warrior indices are stored
 * 
 * RESULTS
 *     The warriors fight their fight in core which gets messed up in
 *     the process.  The indices of warriors that die are stored into
 *     the death_tab[] array in the order of death.  Warrior indices
 *     start from 0.
 * 
 * RETURN VALUE
 *     The number of warriors still alive at the end of the
 *     battle or -1 on an anomalous condition.
 * 
 * GLOBALS
 *     All file scoped globals
 */

/* Various macros:
 *
 *  queue(x): Inserts a core address 'x' to the head of the current
 *            warrior's process queue.  Assumes the warrior's
 * 	      tail pointer is inside the queue buffer.
 *
 * x, y must be in 0..coresize-1 for the following macros:
 *
 * INCMOD(x): x = x+1 mod coresize
 * DECMOD(x): x = x-1 mod coresize
 * ADDMOD(z,x,y): z = x+y mod coresize
 * SUBMOD(z,x,y): z = x-y mod coresize
 */


/*#define queue(x)  do { *w->tail++ = (x); if ( w->tail == queue_end )\
                                          w->tail = queue_start; } while (0)
*/
#define queue(x) \
	do { *(w->tail) = (x); if (++(w->tail) == queue_end) w->tail = queue_start; } while(0)

#define INCMOD(x) do { if ( ++(x) == coresize ) (x) = 0; } while (0)
#define IPINCMOD(x) do { if ( ++(x) == CoreEnd ) (x) = core; } while (0)
#define DECMOD(x) do { if ((x)-- == 0) (x) = coresize1; } while (0)
#define IPDECMOD(x) do { if ((x)==0) x=CoreEnd1; else --(x); } while (0)
#define ADDMOD(z,x,y) do { (z) = (x)+(y); if ((z)>=coresize) (z) -= coresize; } while (0)
/*#define SUBMOD(z,x,y) do { (z) = (x)-(y); if ((int)(z)<0) (z) += coresize; } while (0)*/
/* z is unsigned! overflow occurs. */
#define SUBMOD(z,x,y) do { (z) = (x)-(y); if ((z)>=coresize) (z) += coresize; } while (0)

/* private macros to access p-space. */
#define UNSAFE_PSPACE_SET(warid, paddr, val) do {\
    if (paddr) {\
	PSpaces[(warid)]->mem[(paddr)] = (val);\
    } else {\
	PSpaces[(warid)]->lastresult = (val);\
    }\
} while(0)

#define UNSAFE_PSPACE_GET(warid, paddr) \
	( (paddr) ? PSpaces[(warid)]->mem[(paddr)]\
		  : PSpaces[(warid)]->lastresult )


int
sim_proper(unsigned int  nwar, const field_t * const war_pos_tab, unsigned int *death_tab )
{
  /*
   * Core and Process queue memories.
   *
   * The warriors share a common cyclic buffer for use as a process
   * queue which the contains core addresses where active processes
   * are.  The buffer has size N*P+1, where N = number of warriors,
   * P = maximum number of processes / warrior.
   *
   * Each warrior has a fixed slice of the buffer for its own process
   * queue which are initially allocated to the warriors in reverse
   * order. i.e. if the are N warriors w1, w2, ..., wN, the slice for
   * wN is 0..P-1, w{N-1} has P..2P-1, until w1 has (N-1)P..NP-1.
   *
   * The core address of the instruction is fetched from the head of
   * the process queue and processes are pushed to the tail, so the
   * individual slices slide along at one location per executed
   * instruction.  The extra '+1' in the buffer size is to have free
   * space to slide the slices along.
   * 
   * For two warriors w1, w2:
   *
   * |\......../|\......../| |
   * | w2 queue | w1 queue | |
   * 0          P         2P 2P+1
   */

  /*
   * Cache Registers.
   *
   * The '94 draft specifies that the redcode processor model be
   * 'in-register'.  That is, the current instruction and the
   * instructions at the effective addresses (ea's) be cached in
   * registers during instruction execution, rather than have
   * core memory accessed directly when the operands are needed.  This
   * causes differences from the 'in-memory' model.  e.g. MOV 0,>0
   * doesn't change the instruction's b-field since the instruction at
   * the a-field's effective address (i.e. the instruction itself) was
   * cached before the post-increment happened.
   *
   * There are conceptually three registers: IN, A, and B.  IN is the
   * current instruction, and A, B are the ones at the a- and
   * b-fields' effective addresses respectively.
   *
   * We don't actually cache the complete instructions, but rather
   * only the *values* of their a- and b-field.  This is because
   * currently there is no way effective address computations can
   * modify the opcode, modifier, or addressing modes of an
   * instruction.
   */


  /*
   * misc.
   */
  insn_t* const core = Core_Mem;
  insn_t** const queue_start = Queue_Mem;
  insn_t** const queue_end = Queue_Mem + NWarriors*Processes+1;
  w_t* w;			/* current warrior */
  const unsigned int coresize = Coresize;
  const unsigned int coresize1 = coresize-1; /* size of core, size of core - 1 */
  insn_t* const CoreEnd = core + coresize; // point after last instruction
  insn_t* const CoreEnd1 = CoreEnd - 1; // point to last instruction
  int cycles = nwar * Cycles; /* set instruction executions until tie counter */
  int alive_cnt = nwar;
  int max_alive_proc = nwar * Processes; 
  insn_t **pofs = queue_end-1;

#if DEBUG >= 1
  insn_t insn;			/* used for disassembly */
  char debug_line[256];		/* ditto */
#endif


	War_Tab[0].succ = &War_Tab[nwar-1];
	War_Tab[nwar-1].pred = &War_Tab[0];
	{
	u32_t ftmp = 0;		/* temps */
		
	do {
		int t = nwar-1-ftmp;
		if ( t > 0 ) War_Tab[t].succ = &(War_Tab[t-1]);
		if ( t < nwar-1 ) War_Tab[t].pred = &(War_Tab[t+1]);
		pofs -= Processes;
		*pofs = &(core[war_pos_tab[ftmp]]);
		War_Tab[t].head = pofs;
		War_Tab[t].tail = pofs+1;
		War_Tab[t].nprocs = 1;
		War_Tab[t].id = ftmp;
		ftmp++;
	} while ( ftmp < nwar );
	}
	
	/*******************************************************************
	 * Main loop - optimize here
	 */
	w = &War_Tab[ nwar-1 ];
	do {
		/* 'in' field of current insn for decoding */
		u32_t in;

		/* A register values */
		u32_t ra_a, ra_b;

		/* B register values */
		u32_t rb_a, rb_b;

		insn_t *pta;
		insn_t *ptb;
		unsigned int mode;

		insn_t* ip = *(w->head);
		if ( ++(w->head) == queue_end ) w->head = queue_start;
		in = ip->in;		/* note: flags must be unset! */
#if !SIM_STRIP_FLAGS
		in = in & iMASK;		/* strip flags. */
#endif
		rb_a = ra_a = ip->a;
		rb_b = ip->b;

#if DEBUG >= 1
		insn = *ip;
		dis1( debug_line, insn, coresize);
#endif
		
		mode = in & mMASK;
		
		/* a-mode calculation */
		if (mode == IMMEDIATE) {
			/*printf("IMMEDIATE\n");*/
			ra_b = rb_b;
			pta = ip;
		} else if (mode == DIRECT) {
			/*printf("DIRECT\n");*/
			pta = ip + ra_a; if (pta >= CoreEnd) pta -= coresize;
			ra_a = pta->a;
			ra_b = pta->b;
		} else if (mode == BINDIRECT) {
			/*printf("BINDIRECT\n");*/
			pta = ip + ra_a; if (pta >= CoreEnd) pta -= coresize;
			pta = pta + pta->b; if (pta >= CoreEnd) pta -= coresize;
			ra_a = pta->a;		/* read in registers */
			ra_b = pta->b;
		} else if (mode == APOSTINC) {
			/*printf("APOSTINC\n");*/
			pta = ip + ra_a; if (pta >= CoreEnd) pta -= coresize;
			{field_t* f = &(pta->a);
			pta = pta + pta->a; if (pta >= CoreEnd) pta -= coresize;
			ra_a = pta->a;		/* read in registers */
			ra_b = pta->b;
			INCMOD(*f);}
		} else if (mode == BPOSTINC) {
			/*printf("BPOSTINC\n");*/
			pta = ip + ra_a; if (pta >= CoreEnd) pta -= coresize;
			{field_t* f = &(pta->b);
			pta = pta + pta->b; if (pta >= CoreEnd) pta -= coresize;
			ra_a = pta->a;		/* read in registers */
			ra_b = pta->b;
			INCMOD(*f);}
		} else if (mode == APREDEC) {
			/*printf("APREDEC\n");*/
			pta = ip + ra_a; if (pta >= CoreEnd) pta -= coresize;
			DECMOD(pta->a);
			pta = pta + pta->a; if (pta >= CoreEnd) pta -= coresize;
			ra_a = pta->a;		/* read in registers */
			ra_b = pta->b;
		} else if (mode == BPREDEC) {
			/*printf("BPREDEC\n");*/
			pta = ip + ra_a; if (pta >= CoreEnd) pta -= coresize;
			DECMOD(pta->b);
			pta = pta + pta->b; if (pta >= CoreEnd) pta -= coresize;
			ra_a = pta->a;		/* read in registers */
			ra_b = pta->b;
		} else { /* AINDIRECT */
			/*printf("AINDIRECT\n");*/
			pta = ip + ra_a; if (pta >= CoreEnd) pta -= coresize;
			pta = pta + pta->a; if (pta >= CoreEnd) pta -= coresize;
			ra_a = pta->a;		/* read in registers */
			ra_b = pta->b;
		}
	
		mode = in & (mMASK<<mBITS);

		/* special mov.i code to improve performance */
		if ((in & 16320) == (_OP(MOV, mI) << (mBITS*2))) {
			if (mode == APREDEC<<mBITS) {
				/*++modes[0];*/
				ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
				DECMOD(ptb->a);
				ptb = ptb + ptb->a; if (ptb >= CoreEnd) ptb -= coresize;
			} else if (mode == DIRECT<<mBITS) {
				/*++modes[1];*/
				ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
			} else if (mode == APOSTINC<<mBITS) {
				/*++modes[2];*/
				ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
				{field_t* f = &(ptb->a);
				ptb = ptb + *f; if (ptb >= CoreEnd) ptb -= coresize;
				INCMOD(*f);}
			} else if (mode == BPREDEC<<mBITS) {
				/*++modes[3];*/
				ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
				DECMOD(ptb->b);
				ptb = ptb + ptb->b; if (ptb >= CoreEnd) ptb -= coresize;
			} else if (mode == IMMEDIATE<<mBITS) {
				/*++modes[4];*/
				ptb = ip;
			} else if (mode == BPOSTINC<<mBITS) {
				/*++modes[5];*/
				ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
				{field_t* f = &(ptb->b);
				ptb = ptb + *f; if (ptb >= CoreEnd) ptb -= coresize;
				INCMOD(*f);}
			} else if (mode == BINDIRECT<<mBITS) {
				/*++modes[6];*/
				ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
				ptb = ptb + ptb->b; if (ptb >= CoreEnd) ptb -= coresize;
			} else { /* AINDIRECT */
				/*++modes[7];*/
				ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
				ptb = ptb + ptb->a; if (ptb >= CoreEnd) ptb -= coresize;
			}
			ptb->a = ra_a;
			ptb->b = ra_b;
			ptb->in = pta->in;
			IPINCMOD(ip);
			queue(ip);
			goto noqueue;
		}
		
		/*15360: 
		 *              0  0  1  1  1  1  0  0  0  0  0  0  0  0  0  0
		 * bit         15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
		 * field   | flags | |- op-code  -| |-.mod-| |b-mode| |a-mode|
		 */
		if (!(in & 15360)) {
			if (mode == IMMEDIATE<<mBITS) {
			} else if (mode == DIRECT<<mBITS) {
			} else if (mode == BPOSTINC<<mBITS) {
				ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
				INCMOD(ptb->b);
			} else if (mode == BPREDEC<<mBITS) {
				ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
				DECMOD(ptb->b);
			} else if (mode == APREDEC<<mBITS) {
				ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
				DECMOD(ptb->a);
			} else if (mode == APOSTINC<<mBITS) {
				ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
				INCMOD(ptb->a);
			} /* BINDIRECT, AINDIRECT */
			
			if (in & 512) {
			spl:
				IPINCMOD(ip);
				queue(ip);
				if ( w->nprocs < Processes ) {
					++w->nprocs;
					queue(pta);
				}
				/* in the endgame, check if a tie is inevitable */
				if (cycles < max_alive_proc) {
					w_t* w_iterator = w->succ;
				
					/* break if all warriors have more processes than cycles */
					while ((w_iterator->nprocs * alive_cnt > cycles) && (w_iterator != w)) w_iterator = w_iterator->succ;
						if (w_iterator->nprocs*alive_cnt  > cycles) {
						/*printf("stopping at %d\n", cycles);*/
						goto out;
					}
				}
			}
			else {
			die:
				if (--w->nprocs) goto noqueue;
				w->pred->succ = w->succ;
				w->succ->pred = w->pred;
				*death_tab++ = w->id;
				cycles = cycles - cycles/alive_cnt; /* nC+k -> (n-1)C+k */
				max_alive_proc = alive_cnt * Processes;
				if ( --alive_cnt <= 1 ) 
					goto out;
			}
			goto noqueue;
		}

		
		/* b-mode calculation */
		if (mode == APREDEC<<mBITS) {
			/*printf("APREDEC\n");*/
			ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
			DECMOD(ptb->a);
			ptb = ptb + ptb->a; if (ptb >= CoreEnd) ptb -= coresize;
			rb_a = ptb->a;		/* read in registers */
			rb_b = ptb->b;
		} else if (mode == DIRECT<<mBITS) {
			/*printf("DIRECT\n");*/
			ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
			rb_a = ptb->a;
			rb_b = ptb->b;
		} else if (mode == APOSTINC<<mBITS) {
			/*printf("APOSTINC\n");*/
			ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
			{field_t* f = &(ptb->a);
			ptb = ptb + ptb->a; if (ptb >= CoreEnd) ptb -= coresize;
			rb_a = ptb->a;		/* read in registers */
			rb_b = ptb->b;
			INCMOD(*f);}
		} else if (mode == BPREDEC<<mBITS) {
			/*printf("BPREDEC\n");*/
			ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
			DECMOD(ptb->b);
			ptb = ptb + ptb->b; if (ptb >= CoreEnd) ptb -= coresize;
			rb_a = ptb->a;		/* read in registers */
			rb_b = ptb->b;
		} else if (mode == IMMEDIATE<<mBITS) {
			/*printf("IMMEDIATE\n");*/
			ptb = ip;
		} else if (mode == BPOSTINC<<mBITS) {
			/*printf("BPOSTINC\n");*/
			ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
			{field_t* f = &(ptb->b);
			ptb = ptb + ptb->b; if (ptb >= CoreEnd) ptb -= coresize;
			rb_a = ptb->a;		/* read in registers */
			rb_b = ptb->b;
			INCMOD(*f);}
		} else if (mode == BINDIRECT<<mBITS) {
			/*printf("BINDIRECT\n");*/
			ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
			ptb = ptb + ptb->b; if (ptb >= CoreEnd) ptb -= coresize;
			rb_a = ptb->a;		/* read in registers */
			rb_b = ptb->b;
		} else { /* AINDIRECT */
			/*printf("AINDIRECT\n");*/
			ptb = ip + rb_b; if (ptb >= CoreEnd) ptb -= coresize;
			ptb = ptb + ptb->a; if (ptb >= CoreEnd) ptb -= coresize;
			rb_a = ptb->a;		/* read in registers */
			rb_b = ptb->b;
		}
	
#if DEBUG == 2
    /* Debug output */
    printf("%6d %4ld  %s  |%4ld, d %4ld,%4ld a %4ld,%4ld b %4ld,%4ld\n",
	   cycles, ip-core, debug_line,
	   w->nprocs, pta-core, ptb-core, 
	   ra_a, ra_b, rb_a, rb_b );
#endif

    /*
     * Execute the instruction on opcode.modifier
     */

		
    switch ( in>>(mBITS*2) ) {

    case _OP(MOV, mA):
      ptb->a = ra_a;
      break;
    case _OP(MOV, mF):
      ptb->a = ra_a;
    case _OP(MOV, mB):
      ptb->b = ra_b;
      break;
    case _OP(MOV, mAB):
      ptb->b = ra_a;
      break;
    case _OP(MOV, mX):
      ptb->b = ra_a;
    case _OP(MOV, mBA):
      ptb->a = ra_b;
      break;
    
    case _OP(MOV, mI):
	    printf("unreachable code reached. You have a problem!\n");
	    break;

    case _OP(DJN,mBA):
    case _OP(DJN,mA):
      DECMOD(ptb->a);
      if ( rb_a == 1 ) break;
      queue(pta);
      goto noqueue;

    case _OP(DJN,mAB):
    case _OP(DJN,mB):
      DECMOD(ptb->b);
      if ( rb_b == 1 ) break;
      queue(pta);
      goto noqueue;

    case _OP(DJN,mX):
    case _OP(DJN,mI):
    case _OP(DJN,mF):
      DECMOD(ptb->a);
      DECMOD(ptb->b);
      if ( rb_a == 1 && rb_b == 1 ) break;
      queue(pta);
      goto noqueue;


    case _OP(ADD, mI):
    case _OP(ADD, mF):
      ADDMOD(ptb->b, ra_b, rb_b );
    case _OP(ADD, mA):
      ADDMOD(ptb->a, ra_a, rb_a );
      break;
    case _OP(ADD, mB):
      ADDMOD(ptb->b, ra_b, rb_b );
      break;
    case _OP(ADD, mX):
      ADDMOD(ptb->a, ra_b, rb_a );
    case _OP(ADD, mAB):
      ADDMOD(ptb->b, ra_a, rb_b );
      break;
    case _OP(ADD, mBA):
      ADDMOD(ptb->a, ra_b, rb_a );
      break;


    case _OP(JMZ, mBA):
    case _OP(JMZ, mA):
      if ( rb_a )
	break;
      queue(pta);
      goto noqueue;

    case _OP(JMZ, mAB):
    case _OP(JMZ, mB):
      if ( rb_b )
	break;
      queue(pta);
      goto noqueue;

    case _OP(JMZ, mX):
    case _OP(JMZ, mF):
    case _OP(JMZ, mI):
      if ( rb_a || rb_b )
	break;
      queue(pta);
      goto noqueue;


    case _OP(SUB, mI):
    case _OP(SUB, mF):
      SUBMOD(ptb->b, rb_b, ra_b );
    case _OP(SUB, mA):
      SUBMOD(ptb->a, rb_a, ra_a);
      break;
    case _OP(SUB, mB):
      SUBMOD(ptb->b, rb_b, ra_b );
      break;
    case _OP(SUB, mX):
      SUBMOD(ptb->a, rb_a, ra_b );
    case _OP(SUB, mAB):
      SUBMOD(ptb->b, rb_b, ra_a );
      break;
    case _OP(SUB, mBA):
      SUBMOD(ptb->a, rb_a, ra_b );
      break;


    case _OP(SEQ, mA):
      if ( ra_a == rb_a )
	IPINCMOD(ip);
      break;
    case _OP(SEQ, mB):
      if ( ra_b == rb_b )
	IPINCMOD(ip);
      break;
    case _OP(SEQ, mAB):
      if ( ra_a == rb_b )
	IPINCMOD(ip);
      break;
    case _OP(SEQ, mBA):
      if ( ra_b == rb_a )
	IPINCMOD(ip);
      break;

    case _OP(SEQ, mI):
      if ( pta->in != ptb->in )
	break;
    case _OP(SEQ, mF):
      if ( ra_a == rb_a && ra_b == rb_b )
	IPINCMOD(ip);
      break;
    case _OP(SEQ, mX):
      if ( ra_a == rb_b && ra_b == rb_a )
	IPINCMOD(ip);
      break;


    case _OP(SNE, mA):
      if ( ra_a != rb_a )
	IPINCMOD(ip);
      break;
    case _OP(SNE, mB):
      if ( ra_b != rb_b )
	IPINCMOD(ip);
      break;
    case _OP(SNE, mAB):
      if ( ra_a != rb_b )
	IPINCMOD(ip);
      break;
    case _OP(SNE, mBA):
      if ( ra_b != rb_a )
	IPINCMOD(ip);
      break;

    case _OP(SNE, mI):
      if ( pta->in != ptb->in ) {
	IPINCMOD(ip);
	break;
      }
      /* fall through */
    case _OP(SNE, mF):
      if ( ra_a != rb_a || ra_b != rb_b )
	IPINCMOD(ip);
      break;
    case _OP(SNE, mX):
      if ( ra_a != rb_b || ra_b != rb_a )
	IPINCMOD(ip);
      break;


    case _OP(JMN, mBA):
    case _OP(JMN, mA):
      if (! rb_a )
	break;
      queue(pta);
      goto noqueue;

    case _OP(JMN, mAB):
    case _OP(JMN, mB):
      if (! rb_b )
	break;
      queue(pta);
      goto noqueue;

    case _OP(JMN, mX):
    case _OP(JMN, mF):
    case _OP(JMN, mI):
      if (rb_a || rb_b) {
        queue(pta);
        goto noqueue;
      }
      break;


    case _OP(JMP, mA):
    case _OP(JMP, mB):
    case _OP(JMP, mAB):
    case _OP(JMP, mBA):
    case _OP(JMP, mX):
    case _OP(JMP, mF):
    case _OP(JMP, mI):
      queue(pta);
      goto noqueue;



    case _OP(SLT, mA):
      if (ra_a < rb_a)
	IPINCMOD(ip);
      break;
    case _OP(SLT, mAB):
      if (ra_a < rb_b)
	IPINCMOD(ip);
      break;
    case _OP(SLT, mB):
      if (ra_b < rb_b)
	IPINCMOD(ip);
      break;
    case _OP(SLT, mBA):
      if (ra_b < rb_a)
	IPINCMOD(ip);
      break;
    case _OP(SLT, mI):
    case _OP(SLT, mF):
      if (ra_a < rb_a && ra_b < rb_b)
	IPINCMOD(ip);
      break;
    case _OP(SLT, mX):
      if (ra_a < rb_b && ra_b < rb_a)
	IPINCMOD(ip);
      break;


    case _OP(MODM, mI):
    case _OP(MODM, mF):
      if ( ra_a ) ptb->a = rb_a % ra_a;
      if ( ra_b ) ptb->b = rb_b % ra_b;
      if (!ra_a || !ra_b) goto die;
      break;
    case _OP(MODM, mX):
      if ( ra_b ) ptb->a = rb_a % ra_b;
      if ( ra_a ) ptb->b = rb_b % ra_a;
      if (!ra_b || !ra_a) goto die;
      break;
    case _OP(MODM, mA):
      if ( !ra_a ) goto die;
      ptb->a = rb_a % ra_a;
      break;
    case _OP(MODM, mB):
      if ( !ra_b ) goto die;
      ptb->b = rb_b % ra_b;
      break;
    case _OP(MODM, mAB):
      if ( !ra_a ) goto die;
      ptb->b = rb_b % ra_a;
      break;
    case _OP(MODM, mBA):
      if ( !ra_b ) goto die;
      ptb->a = rb_a % ra_b;
      break;


    case _OP(MUL, mI):
    case _OP(MUL, mF):
      ptb->b = (rb_b * ra_b) % coresize;
    case _OP(MUL, mA):
      ptb->a = (rb_a * ra_a) % coresize;
      break;
    case _OP(MUL, mB):
      ptb->b = (rb_b * ra_b) % coresize;
      break;
    case _OP(MUL, mX):
      ptb->a = (rb_a * ra_b) % coresize;
    case _OP(MUL, mAB):
      ptb->b = (rb_b * ra_a) % coresize;
      break;
    case _OP(MUL, mBA):
      ptb->a = (rb_a * ra_b) % coresize;
      break;


    case _OP(DIV, mI):
    case _OP(DIV, mF):
      if ( ra_a ) ptb->a = rb_a / ra_a;
      if ( ra_b ) ptb->b = rb_b / ra_b;
      if (!ra_a || !ra_b) goto die;
      break;
    case _OP(DIV, mX):
      if ( ra_b ) ptb->a = rb_a / ra_b;
      if ( ra_a ) ptb->b = rb_b / ra_a;
      if (!ra_b || !ra_a) goto die;
      break;
    case _OP(DIV, mA):
      if ( !ra_a ) goto die;
      ptb->a = rb_a / ra_a;
      break;
    case _OP(DIV, mB):
      if ( !ra_b ) goto die;
      ptb->b = rb_b / ra_b;
      break;
    case _OP(DIV, mAB):
      if ( !ra_a ) goto die;
      ptb->b = rb_b / ra_a;
      break;
    case _OP(DIV, mBA):
      if ( !ra_b ) goto die;
      ptb->a = rb_a / ra_b;
      break;


    case _OP(NOP,mI):
    case _OP(NOP,mX):
    case _OP(NOP,mF):
    case _OP(NOP,mA):
    case _OP(NOP,mAB):
    case _OP(NOP,mB):
    case _OP(NOP,mBA):
      break;

    case _OP(LDP,mA):
      ptb->a = UNSAFE_PSPACE_GET(w->id, ra_a % PSpace_size);
      break;
    case _OP(LDP,mAB):
      ptb->b = UNSAFE_PSPACE_GET(w->id, ra_a % PSpace_size);
      break;
    case _OP(LDP,mBA):
      ptb->a = UNSAFE_PSPACE_GET(w->id, ra_b % PSpace_size);
      break;
    case _OP(LDP,mF):
    case _OP(LDP,mX):
    case _OP(LDP,mI):
    case _OP(LDP,mB):
      ptb->b = UNSAFE_PSPACE_GET(w->id, ra_b % PSpace_size);
      break;

    case _OP(STP,mA):
      UNSAFE_PSPACE_SET(w->id, rb_a % PSpace_size, ra_a);
      break;
    case _OP(STP,mAB):
      UNSAFE_PSPACE_SET(w->id, rb_b % PSpace_size, ra_a);
      break;
    case _OP(STP,mBA):
      UNSAFE_PSPACE_SET(w->id, rb_a % PSpace_size, ra_b);
      break;
    case _OP(STP,mF):
    case _OP(STP,mX):
    case _OP(STP,mI):
    case _OP(STP,mB):
      UNSAFE_PSPACE_SET(w->id, rb_b % PSpace_size, ra_b);
      break;

#if DEBUG > 0
    default:
      alive_cnt = -1;
      goto out;
#endif
    }

    IPINCMOD(ip);
    queue(ip);
  noqueue:
    w = w->succ;
  } while(--cycles > 0);

 out:
#if DEBUG == 1
  printf("cycles: %d\n", cycles);
#endif
  
  return alive_cnt;
}

/*
void print_counts(void) {
	printf("%10ld;", modes[0]);
	printf("%10ld;", modes[1]);
	printf("%10ld;", modes[2]);
	printf("%10ld;", modes[3]);
	printf("%10ld;", modes[4]);
	printf("%10ld;", modes[5]);
	printf("%10ld;", modes[6]);
	printf("%10ld",  modes[7]);
	printf("\n");
}
*/