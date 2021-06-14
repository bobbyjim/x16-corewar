
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

extern Cell arena[CORESIZE];

Cell            inst;
Cell            operandData;
int             ip, source, destination, final;
unsigned char   owner;
int             tempInt;

void debug()
{
    cprintf("*** debug ***");
    cprintf("%5d -> %5d  ", source, destination);
    printCell(&operandData, "\r\n");
}

void addProcess(unsigned char owner, unsigned int address)
{
    cprintf("add-process(%u @ @u)\r\n", owner, address);
}

void removeProcess(unsigned char owner, unsigned int address)
{
    cprintf("remove-process(%u @ @u)\r\n", owner, address);
}

unsigned char getOperandAData()
{
    switch(inst.aMode)
    {
        case IMMEDIATE: // 
            operandData.B = inst.A;
            break;
        case DIRECT: // 
            operandData = arena[(ip + inst.A) % CORESIZE];
            break;
        case PREDECREMENT_INDIRECT: 
            --arena[(ip + inst.A) % CORESIZE].B;
            // fall through
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

        case PREDECREMENT_INDIRECT: // pre-decrement INDIRECT
            --arena[(ip + inst.B) % CORESIZE].B;
            // fall through
        case INDIRECT: 
            destination = arena[(ip + inst.B) % CORESIZE].B;
            final = destination + ip + inst.B + CORESIZE;
            break;
    }

    // adjust the "final" address.
    if (final > CORESIZE) final -= CORESIZE;
    return 0;
}

unsigned char compareEqual()
{
    switch (inst.bMode)
	{
		case IMMEDIATE: /* error */
			if (operandData.B == inst.B) return 1;
			return 2;

		case DIRECT:
			if ( operandData.B == arena[(ip + inst.B) % CORESIZE].B)
				return 1;
			return 2;

		case PREDECREMENT_INDIRECT:
			destination = --arena[(ip + inst.B) % CORESIZE].B;
            // fall through
		case INDIRECT:
			destination = arena[(ip + inst.B) % CORESIZE].B;
			if(operandData.B == arena[(destination+ip+inst.B)%CORESIZE].B)
				return 1;
			return 2;
	}	
    return 0;
}

int execute()
{
    unsigned char data_bad = 0;
    unsigned char skip = 0;

    inst = arena[ip];
    
    operandData.opcode = 0;
    operandData.aMode = 0;
    operandData.bMode = 0;
    operandData.A = 0;
    operandData.B = 0;

    if (inst.opcode >= MOV && inst.opcode <= MOD)
    {
        data_bad = getOperandAData() 
                 + getOperandBData();
        if (data_bad > 0)
        {
            execFail("Error retrieving operand data");
            inst.opcode = HCF;
        }
    }

    printCell(&inst, " <-- current cell\r\n");

    switch(inst.opcode)
    {
        case MOV: 
            cprintf("final address: %d\r\n", final);
            printCell(&operandData, " <-- operand data\r\n");
            if (inst.aMode == IMMEDIATE)
            {
                arena[final].B = operandData.A; /// ???
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

//
//      JMP is JMN #1 B  or  JMZ #0 B
//

        case DJN: // IP = A if --B != 0
            final = (ip + inst.B) % CORESIZE;
            if (arena[final].B == 0)
                arena[final].B = CORESIZE - 1;
            else
                --arena[final].B;

            // fall through

        case JMN: // IP = A if B != 0
            final = (ip + inst.B) % CORESIZE;
            if (/*arena[final].opcode == DAT &&*/ arena[final].B != 0)
                ip += inst.A;
            else   
                ++ip;
            break;

        case DJZ: // IP = B if --A == 0
            final = (ip + inst.B) % CORESIZE;
            if (arena[final].B == 0)
                arena[final].B = CORESIZE - 1;
            else
                --arena[final].B;

            // fall through

        case JMZ: // IP = A if B == 0
            final = (ip + inst.B) % CORESIZE;
            if (/*arena[final].opcode == DAT &&*/ arena[final].B == 0)
                ip += inst.A;
            else
                ++ip;
            break;

        case SKE: // IP++ if A == B
        case SLT: // IP++ if A != B
            getOperandAData();
            skip = compareEqual();
            if (skip == 2) goto fail;
            if ((skip == 0 && inst.opcode == SLT)
              ||(skip == 1 && inst.opcode == SKE)) ++ip; // "skip"
            ++ip;
            break;

        case SPL: // SPLIT!
            final = (ip + inst.A) % CORESIZE;
            addProcess(owner, final);
            ++ip;
            break;

        case MUL: // later
        case DIV: // later
        case MOD: // later
        case XCH: // later
        case HCF:
        fail:
        default:
            if (isVerbose()) execFail("hcf");
            removeProcess(owner, ip);
            return -1; // die
    }

    if (ip >= CORESIZE) 
        ip -= CORESIZE; // cheaper than a mod

    return 0;
}