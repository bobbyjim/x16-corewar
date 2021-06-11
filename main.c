
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

    setTestVerbosity(1);

    //RUN_TEST("hcf 0/0",  1000, "hcf #4 #5",  1000, "hcf #4 #5");
    RUN_TEST("mov #a b", 200, "mov #4   1",  201, "hcf #0  #4"); // OK
    //RUN_TEST(1000, "mov #0  @1",  1001, "mov #0  @1"); 
    //RUN_TEST(1004, "mov 0  2",  1004, "mov 0  2");

}
