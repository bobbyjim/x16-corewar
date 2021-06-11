#include <conio.h>

#include "arena.h"
#include "cell.h"
#include "test.h"
#include "vm.h"

unsigned char testVerbosity = 0;
unsigned char lastTestResult;

unsigned char getLastTestResult()
{
    return lastTestResult;
}

void setTestVerbosity(unsigned char v) 
{
    testVerbosity = v;
    if (v > 0)
        cprintf("verbose test mode!\r\n\r\n");
    else
        cprintf("(quiet mode)\r\n\r\n");
}

//
//  Compares a cell at a core location against the Temp cell.
//
unsigned char testCell(char *testname, int resultLocation)
{
    Cell* expected = getTempCell();
    unsigned char lastTestResult = TEST_PASS;
 
    Cell *actual = getLocation(resultLocation);

    if (actual->opcode != expected->opcode)
    {
        lastTestResult += TEST_FAIL_OPCODE;
    }

    if (actual->aMode != expected->aMode)
    {
        lastTestResult += TEST_FAIL_A_MODE;
    }

    if (actual->A != expected->A)
    {
        lastTestResult += TEST_FAIL_A_OPERAND;
    }

    if (actual->bMode != expected->bMode)
    {
        lastTestResult += TEST_FAIL_B_MODE;
    }

    if (actual->B != expected->B)
    {
        lastTestResult += TEST_FAIL_B_OPERAND;
    }

    if (testVerbosity)
    { 
        cprintf("-----------------------------------\r\n");
        textcolor(LTBLUE);
        cprintf("want %5d:  ", resultLocation);
        printCell(expected, "  ");
        cprintf("%s  ", testname);
        if (lastTestResult) 
        {
            textcolor(LTRED);
            cputs("** fail **\r\n");
        }
        else
        {
           textcolor(GREEN);
           cputs("ok\r\n");
        }
        textcolor(DEFAULT_COLOR);
    }

    return lastTestResult;
}
