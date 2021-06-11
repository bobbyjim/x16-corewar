
#include <cbm.h>
#include <conio.h>

#include "common.h"
#include "arena.h"
#include "cell.h"
#include "test.h"
#include "vm.h"

void resetTestArena()
{
    // wipe core
    clearArena();

    // set up some test data 
    loadCell("hcf #7   #0", 4049);
    loadCell("hcf #17 #19", 4050);
    loadCell("hcf @-1  #7", 4051);
}

void main() //int argc, char* argv[])
{
    cbm_k_bsout(0x8E); // revert to primary case

    bgcolor(BLACK);
    textcolor(LTGREY);
    clrscr();

    setVerbosity(1);
    resetTestArena();

// These tests pass

    RUN_TEST("t/hcf #a #b",  100, "hcf #4 #5",  100, "hcf #4 #5");
    RUN_TEST("t/hcf #a @b",  100, "hcf #5 @5",  105, "hcf #0 #0");
    RUN_TEST("t/mov #a  b",  100, "mov #4   1", 101, "hcf #0  #4");  
    RUN_TEST("t/mov  a  b",  100, "mov 0  2",   102, "mov 0  2");
    clearLocation(101);
    RUN_TEST("t/add #a  b",  100, "add #4 1",   101,  "hcf #0 #4" );
    RUN_TEST("t/add  a  b",   50, "add 4000 1",  51,  "hcf #0 #17");   // assumes test data
    RUN_TEST("t/add @a  b",   50, "add @4001 1", 50, "add @4001 18");  // assumes test data
    RUN_TEST("t/sub #a  b", 4040, "sub #3 10", 4050, "hcf #17 #16");   // assumes test data
    resetTestArena();
    RUN_TEST("t/sub  a  b", 4040, "sub 9 10",  4050, "hcf #17 #12");   // assumes test data


// These are works in progress


}
