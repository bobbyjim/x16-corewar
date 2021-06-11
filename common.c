
#include <conio.h>
#include "common.h"

unsigned char verbosity = 0;

void setVerbosity(unsigned char v) 
{
    verbosity = v;
    if (v > 0)
        cprintf("verbose mode!\r\n\r\n");
}

unsigned char isVerbose()
{
    return verbosity;
}
