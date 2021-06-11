
#include <cbm.h>
#include <conio.h>

#include "common.h"
#include "arena.h"
#include "cell.h"
#include "test.h"
#include "vm.h"

void main() //int argc, char* argv[])
{
    cbm_k_bsout(0x8E); // revert to primary case

    clearArena();
    bgcolor(BLACK);
    textcolor(LTGREY);
    clrscr();

    setVerbosity(1);

    //RUN_TEST("t/hcf #0 #0",  1000, "hcf #4 #5",  1000, "hcf #4 #5");
    //RUN_TEST("t/hcf #5 @5",  1000, "hcf #5 @5",  1005, "hcf #0 #0");

    RUN_TEST("t/mov #a b",  200, "mov #4   1",  201, "hcf #0  #4"); 
    //RUN_TEST("t/mov #a @b", 1000, "mov #0  @1",  1001, "mov #0  @1"); 
    //RUN_TEST("t/mov a b",   1004, "mov 0  2",  1004, "mov 0  2");

}
