#ifdef X16
#include <peekpoke.h>
#include <conio.h>
#endif
#include <string.h>

#include "bank.h"

unsigned char currentBank;

void switchToRedcodeBank()
{
    setBank(REDCODE_BANK);
}
void switchToHelpBank()
{
    setBank(HELP_BANK);
}

void setBank(unsigned char bank)
{
    if (currentBank == bank) return;
    currentBank = bank;
#ifdef X16
    POKE(0x9f61, bank);  // r38-
    POKE(0, bank);       // r39+
#endif
}

void setCoreBank(int position)
{
    unsigned char bank = 10 + (position / CELLS_PER_BANK);

    if (bank == currentBank) return;

    currentBank = bank;

#ifdef X16
    POKE(0x9f61, currentBank); // r38-
    POKE(0, currentBank);      // r39+
#endif
}


