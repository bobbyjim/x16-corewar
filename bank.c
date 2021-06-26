#include <conio.h>
#include <peekpoke.h>

#include "bank.h"

unsigned char currentBank;

void switchToHelpBank()
{
    if (currentBank == 1) return;

    currentBank = 1;
    POKE(0x9f61, 1); // r38-
    POKE(0, 1);      // r39+
}

void setBank(unsigned char bank)
{
    if (currentBank = bank) return;

    cprintf("bank -> %u\n", bank);
    currentBank = bank;
    POKE(0x9f61, bank);  // r38-
    POKE(0, bank);       // r39+
}

void setCoreBank(int position)
{
    unsigned char bank = 10 + (position / CELLS_PER_BANK);

    if (bank == currentBank) return;

    cprintf("bank -> %u\n", bank);
    currentBank = bank;
    POKE(0x9f61, currentBank); // r38-
    POKE(0, currentBank);      // r39+
}
