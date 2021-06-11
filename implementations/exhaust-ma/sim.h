/* sim.h: prototype
 * $Id: sim.h,v 1.3 2003/06/06 21:31:15 martinus Exp $
 */
#ifndef SIM_H
#define SIM_H

#include "pspace.h"

/*
void print_counts(void);
void clear_counts(void);
*/

insn_t *sim_alloc_bufs( unsigned int nwar, unsigned int coresize,
			unsigned int processes, unsigned int cycles );

insn_t *sim_alloc_bufs2( unsigned int nwar, unsigned int coresize,
			 unsigned int processes, unsigned int cycles,
			 unsigned int pspace );

void sim_free_bufs();

void sim_clear_core(void);


pspace_t **sim_get_pspaces(void);

pspace_t *sim_get_pspace(unsigned int war_id);

void sim_clear_pspaces(void);

void sim_reset_pspaces(void);

int sim_load_warrior(unsigned int pos, insn_t const *code, unsigned int len);



int sim( int nwar_arg, field_t w1_start, field_t w2_start,
	 unsigned int cycles, void **ptr_result );

int sim_mw( unsigned int nwar, const field_t *war_pos_tab,
	    unsigned int *death_tab );

#endif /* SIM_H */
