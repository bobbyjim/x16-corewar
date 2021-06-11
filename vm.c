
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
           //
           //  I think this is not the ICWS predecrement.
           //  This version seems to remove the need for
           //  DJN and DJZ, and in fact makes the "D" 
           //  possible for all opcodes.  Sounds like a win.
           //
           cell->A = cell->A - 1;
           setLocation(ip, cell); // store it
           // fall through

        case 2: // address indirect
           aCell = getLocation(ip + cell->A);
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
           // see comment for aMode case.
           cell->B = cell->B-1;
           setLocation(ip, cell); // store it
           // fall through

        case 2: // address indirect
           bCell = getLocation(ip + cell->B);
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
//      JMP is JMN A #1  or  JMZ A #0
//

        case JMN: // IP = A if B != 0
            if (bCell->B > 0) 
                ip = aCell->A;
            break;

        case JMZ: // IP = A if B == 0
            if (bCell->B == 0) 
                ip = aCell->A;
            break;

        case SKE: // IP++ if A == B
            if (bCell->B == aCell->A) 
                ++ip;
            break;

        case SNE: // IP++ if A != B
            if (bCell->B != aCell->A) 
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