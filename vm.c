
#include <conio.h>

#include "cell.h"
#include "arena.h"

int ip;
void setIp(int val) { ip = val; }
int getIp() { return ip; }

int executeCorewar()
{
    int aLocation, bLocation;
     Cell *aCell, *bCell;
 
    int indirect = 0;
    int aMod = 0;
    int bMod = 0;

    Cell* cell = getLocation(ip);
//    cprintf("%d:", ip);
//    printCell("", cell, "\r\n");

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
    // aLocation and aCell are set.
 
    // Figure out the B-location and B-cell
    switch(cell->bMode)
    {
       default:
       case 0: 
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
    // bLocation and bCell are set.

    ++ip; // increment the instruction index in preparation for program X's next turn.

    switch(cell->opcode)
    {
        case HCF:
            return -1; // die

        case MOV: 
            if (bCell == aCell) // nop
               break; 

            setLocation(bLocation, aCell);
            break;

        case ADD:
            bCell->B += aCell->A;
            setLocation(bLocation, bCell);
            break;

        case MUL: 
            bCell->B *= aCell->A;
            setLocation(bLocation, bCell);
            break;

        case DIV:
            bCell->B /= aCell->A;
            setLocation(bLocation, bCell);
            break;

        case MOD:
            bCell->B %= aCell->A;
            setLocation(bLocation, bCell);
            break;

//
//      JMP is JMN #1 B  or  JMZ #0 B
//

        case DJN: // IP = B if --A != 0
            aCell->A = aCell->A - 1;
            setLocation(aLocation, aCell);
            // fall through...

        case JMN: // IP = B if A != 0
            if (aCell->A > 0) 
                ip = bCell->B;
            break;

        case DJZ: // IP = B if --A == 0
            aCell->A = aCell->A - 1;
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