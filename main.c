
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
    loadCell("hcf #1   #9", 4041);
    loadCell("hcf #2   #8", 4042);
    loadCell("hcf #3   #7", 4043);
    loadCell("hcf #4   #6", 4044);
    loadCell("hcf #5   #5", 4045);
    loadCell("hcf #6   #4", 4046);
    loadCell("hcf #7   #3", 4047);
    loadCell("hcf #8   #2", 4048);
    loadCell("hcf #9   #1", 4049);
    loadCell("hcf #17 #19", 4050);
    loadCell("hcf @-1  #7", 4051);
    loadCell("hcf #5   #5", 4052);
    loadCell("hcf #6  #22", 4053);
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

    RUN_TEST("t/mul #a  b", 4045, "mul #5 7",  4052, "hcf #5  #25");   // assumes test data
    RUN_TEST("t/div @a  b", 4045, "div @3 8",  4053, "hcf #6  #4");    // assumes test data




}
