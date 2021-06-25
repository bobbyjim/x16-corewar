#include <string.h>
#include <ctype.h>

#include "common.h"

unsigned char verbosity = 0;
char* verbosityLabel[] = 
{
    "quiet",
    "verbose",
    "max verbosity"
};

void setVerbosity(unsigned char v) 
{
    verbosity = v;
}

void bumpVerbosity()
{
    if (++verbosity == 3) verbosity = 0;
    setVerbosity(verbosity);
}

unsigned char isVerbose()
{
    return verbosity;
}
