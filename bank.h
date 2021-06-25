#ifndef _BANK_H_
#define _BANK_H_

#include    <peekpoke.h>

#define     CELLS_PER_BANK             1000
#define     SET_BANK(position)         POKE 0x9f61, position % CELLS_PER_BANK

#endif
