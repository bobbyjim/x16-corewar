#include <conio.h>

#include "arena.h"
#include "cell.h"
#include "common.h"
#include "test.h"
#include "vm.h"

unsigned char lastTestResult;

unsigned char getLastTestResult()
{
    return lastTestResult;
}

//
//  Compares a cell at a core location against the Temp cell.
//
unsigned char testCell(char *testname, int resultLocation)
{
    Cell* expected = getTempCell();
    Cell *actual = getLocation(resultLocation);

    lastTestResult = TEST_PASS;

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

    if (isVerbose())
    { 
        textcolor(LTBLUE);
        cprintf("test  %5d:  ", resultLocation);
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
        cprintf("-----------------------------------\r\n");
    }

    return lastTestResult;
}
