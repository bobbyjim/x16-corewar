
#include <conio.h>

#include "cell.h"
#include "arena.h"

int ip;
void setIp(int val) { ip = val; }
int getIp() { return ip; }

void execFail(char *reason)
{
    if (isVerbose())
    {
        textcolor(LTRED);
        cprintf("                                     rm: %s\r\n", reason);
        textcolor(DEFAULT_COLOR);
    }
}

int aLocation, bLocation;
Cell *aCell, *bCell;
int indirect;
int aMod;
int bMod;
int temp;
Cell* cell;
int aValue, bValue;

void debug()
{
    cprintf("*** debug ***");
    cprintf("%5d ", aLocation);
    printCell(aCell, "   ");
    cprintf("%5d ", bLocation);
    printCell(bCell, "\r\n");
}

int executeCorewar()
{
    cell = getLocation(ip);
    aCell = cell;
    bCell = cell;
    aLocation = ip;
    bLocation = ip;
    indirect = 0;
    aMod = 0;
    bMod = 0;
    temp = 0;
    aValue = 0;
    bValue = 0;

    // Figure out the A location and cell.
    switch(cell->aMode)
    {
        case 0: // direct
           aLocation = ip;
           aCell = cell;
           break;

        case 3: // address indirect predecrement
           aMod = -1;
           // fall through

        case 2: // address indirect
           aCell = getLocation(ip + cell->A + aMod);
           indirect = aCell->A;
           // fall through

        case 1: // address
           aLocation = ip + cell->A + indirect;
           aCell = getLocation(aLocation);
           break;
    }
    aValue = aCell->A;
 
    // Figure out the B-location and B-cell
    switch(cell->bMode)
    {
       default:
       case 0: 
            if (isVerbose()) execFail("#b not permitted");
           return -1; // die

       case 3: // address indirect predecrement
           bMod = -1;
           // fall through

        case 2: // address indirect
           bCell = getLocation(ip + cell->B + bMod);
           indirect = bCell->B;
           // fall through

        case 1: // address
            bLocation = ip + cell->B + indirect;
            bCell = getLocation(bLocation);
            break;
    }
    bValue = bCell->B;

    ++ip; // increment the instruction index in preparation for program X's next turn.

    switch(cell->opcode)
    {
        case HCF:
            if (isVerbose()) execFail("hcf");
            return -1; // die

        case MOV: 
            switch(cell->aMode)
            {
                case 0: // mov A value to B operand 
                    bCell->B = aCell->A;
                    setLocation(bLocation, bCell);
                    break;
            
                default: // copy A cell to B location
                    setLocation(bLocation, aCell);
                    break;
            }
            break;

        case SUB: // subtract A value from B operand
            aValue = -aValue;
            // fall through

        case ADD: // add A value to B operand
            // 
            // Per the 1986 rules, this operator 
            // updates the B value only.
            //
            temp = aValue + bValue;
            if (temp < 0)        temp += CORESIZE;
            if (temp > CORESIZE) temp -= CORESIZE;
            bCell->B = temp;
            setLocation(bLocation, bCell);
            break;

        case MUL: 
            bValue *= aValue;
            bCell->B = bValue % CORESIZE;
            setLocation(bLocation, bCell);
            break;

        case DIV:
            bValue /= aValue;
            bCell->B = bValue % CORESIZE;
            setLocation(bLocation, bCell);
            break;

        case MOD:
            bValue %= aValue;
            bCell->B = bValue % CORESIZE;
            setLocation(bLocation, bCell);
            break;

//
//      JMP is JMN #1 B  or  JMZ #0 B
//

        case DJN: // IP = B if --A != 0
            temp = aCell->A - 1;
            if (temp < 0)        temp += CORESIZE;
            aCell->A = temp;
            setLocation(aLocation, aCell);
            // fall through...

        case JMN: // IP = B if A != 0
            if (aCell->A > 0) 
                ip = bCell->B;
            break;

        case DJZ: // IP = B if --A == 0
            temp = aCell->A - 1;
            if (temp < 0)        temp += CORESIZE;
            aCell->A = temp;
            setLocation(aLocation, aCell);
            // fall through...

        case JMZ: // IP = B if A == 0
            if (aCell->A == 0) 
                ip = bCell->B;
            break;

        case SKE: // IP++ if A == B
            if (bCell->B == aCell->A) 
                ++ip;
            break;

        case SLT: // IP++ if A <!=> B
            if (bCell->B < aCell->A) 
                ++ip;
            break;

        case SPL: // SPLIT!
            return bCell->B;
            break;
    }

    if (ip >= CORESIZE) 
        ip -= CORESIZE; // cheaper than a mod

    return 0;
}