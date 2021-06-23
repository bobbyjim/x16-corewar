#include <stdio.h>

#include "cell.h"
#include "arena.h"
#include "process.h"

extern Cell arena[CORESIZE];

Cell            inst;
Cell            operandData;
Cell            *aCellref, *bCellref;
int             ip, source, destination, final;
int             tempInt;

typedef struct {
    unsigned int status : 12;
} System;

System system;

unsigned char getOperandAData()
{
    switch(inst.aMode)
    {
        case IMMEDIATE: // 
            operandData.B = inst.A; // why are we assigning into B ??
            break;
        case DIRECT: // 
            operandData = arena[(ip + inst.A) % CORESIZE];
            break;
/*
        case PREDECREMENT_INDIRECT: 
            --arena[(ip + inst.A) % CORESIZE].B;
            // fall through
*/
        case INDIRECT: // 
            source = arena[(ip + inst.A) % CORESIZE].B;
            operandData = arena[(source + ip + inst.A) % CORESIZE];
            break;
    }
    return 0;
}

unsigned char getOperandBData()
{
    switch(inst.bMode)
    {
        case IMMEDIATE: 
            //cprintf("illegal immediate destination\r\n");
            //return 1;
            /* 
                Officially an error, but here, Immediate B-modes are allowed 
                and have the same effect as in 86.
            */
            final = ip;
            break;

        case DIRECT: 
            final = ip + inst.B + CORESIZE;
            break;

/*
        case PREDECREMENT_INDIRECT: // pre-decrement INDIRECT
            --arena[(ip + inst.B) % CORESIZE].B;
            // fall through
*/

        case INDIRECT: 
            destination = arena[(ip + inst.B) % CORESIZE].B;
            final = destination + ip + inst.B + CORESIZE;
            break;
    }

    // adjust the "final" address.
    if (final > CORESIZE) final -= CORESIZE;
    return 0;
}

char compareEqual()
{
    switch (inst.bMode)
	{
		case IMMEDIATE: /* error */
			if (operandData.B > inst.B) return 1;
            if (operandData.B < inst.B) return -1;
            break;

		case DIRECT:
			if (operandData.B > arena[(ip + inst.B) % CORESIZE].B) return 1;
			if (operandData.B < arena[(ip + inst.B) % CORESIZE].B) return -1;
            break;
/*
		case PREDECREMENT_INDIRECT:
			destination = --arena[(ip + inst.B) % CORESIZE].B;
            // fall through
*/
		case INDIRECT:
			destination = arena[(ip + inst.B) % CORESIZE].B;
			if (operandData.B > arena[(destination+ip+inst.B)%CORESIZE].B) return 1;
			if (operandData.B < arena[(destination+ip+inst.B)%CORESIZE].B) return -1;
	}	
    return 0; // equal I assume
}

int vm_execute(unsigned char owner, int ip)
{
    unsigned char data_bad = 0;
    char skip = 0;

    unsigned char tempMode;
    unsigned char tempOperand;

    //cprintf("vm_execute %u at address %d\r\n", owner, ip);

    inst = arena[ip];
    
    operandData.opcode = 0;
    operandData.aMode = 0;
    operandData.bMode = 0;
    operandData.A = 0;
    operandData.B = 0;

    if (inst.opcode >= MOV && inst.opcode < SEQ) // e.g. not the skips
    {
        data_bad = getOperandAData() 
                 + getOperandBData();
        if (data_bad > 0)
            inst.opcode = HCF;
    }

    switch(inst.opcode)
    {
        case MOV: 
            if (inst.aMode == IMMEDIATE)
            {
                arena[final].B = operandData.A; // ? or .B ?
                arena[final].bMode = IMMEDIATE; 
            }
            else
                arena[final] = operandData;
            ++ip;
            break;

        case SUB: 
            operandData.A = -operandData.A;
            // fall thru
        case ADD:
            tempInt = arena[final].B + operandData.A + CORESIZE;
            if (tempInt > CORESIZE) tempInt -= CORESIZE;
            arena[final].B = tempInt;
            ++ip;
            break;

        case JMP:
            ip += final; 
            break;

/*
        case DJN: // IP = A if --B != 0
            final = (ip + inst.B) % CORESIZE;
            if (arena[final].B == 0)
                arena[final].B = CORESIZE - 1;
            else
                --arena[final].B;

            // fall through
*/

        case JMN: // IP = A if B != 0
            final = (ip + inst.B) % CORESIZE;
            if (/*arena[final].opcode == DAT &&*/ arena[final].B != 0)
                ip += inst.A;
            else   
                ++ip;
            break;

/*
        case DJZ: // IP = B if --A == 0
            final = (ip + inst.B) % CORESIZE;
            if (arena[final].B == 0)
                arena[final].B = CORESIZE - 1;
            else
                --arena[final].B;

            // fall through
*/
        case JMZ: // IP = A if B == 0
            final = (ip + inst.B) % CORESIZE;
            if (/*arena[final].opcode == DAT &&*/ arena[final].B == 0)
                ip += inst.A;
            else
                ++ip;
            break;

        case SEQ: // IP++ if A == B     fall-through
        case SNE: // IP++ if A != B     fall-through
        case SLT: // IP++ if A  < B
            getOperandAData();
            skip = compareEqual();
            if ((skip == -1 && inst.opcode == SLT)
              ||(skip !=  0 && inst.opcode == SNE)
              ||(skip ==  0 && inst.opcode == SEQ)) ++ip; 
            ++ip;
            break;

        case FLP: // Jump to location B if A > status word.
            if (  (inst.A > system.status)
               && (getOperandBData() == 0) ) // ok
                 ip = final;
            break;

        case SPL: // SPLIT!
            final = (ip + inst.A) % CORESIZE;
            process_add(owner, final);
            ++ip;
            break;

        case XCH: // exhange operands at location B
            getOperandBData();
            tempMode    = arena[final].aMode;
            tempOperand = arena[final].A;
            arena[final].aMode = arena[final].bMode;
            arena[final].A     = arena[final].B;
            arena[final].bMode = tempMode;
            arena[final].B     = tempOperand;
            break;

        case HCF:
        fail:
        default:
            process_remove(owner, ip);
            return -1; // die
    }

    ++system.status; // whatever

    if (ip >= CORESIZE) 
        ip -= CORESIZE; // cheaper than a mod

    return ip; // updated ip
}