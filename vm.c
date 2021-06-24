#include <stdio.h>

#include "cell.h"
#include "arena.h"
#include "process.h"
#include "x16.h"

extern Cell arena[CORESIZE];
extern System corewar_system;

#define     INST        arena[ip]

//Cell            operandData;
int             ip, ipNext; //, source, destination, final;
//int             tempInt;

Cell            SRA,    SRB;
int             PCA,    PCB;
int             AValue, BValue;

void getOperandAData()
{
    switch(INST.aMode)
    {
        case IMMEDIATE: // 
            PCA = ip;
            AValue = arena[PCA].A;

            //operandData.A = INST.A; // only assign the B field into operand
            break;

        case DIRECT: // 
            PCA = (ip + INST.A) % CORESIZE;
            AValue = arena[PCA].B;

            //operandData = arena[(ip + INST.A) % CORESIZE];
            break;

        case INDIRECT: // 
            PCA = (ip + INST.A) % CORESIZE;
            SRA = arena[PCA];
            PCA = (PCA + SRA.B) % CORESIZE;
            AValue = arena[PCA].B;

            //source = arena[(ip + INST.A) % CORESIZE].B;
            //operandData = arena[(source + ip + INST.A) % CORESIZE];
            break;
    }
    SRA = arena[PCA];
}

void getOperandBData()
{
    switch(INST.bMode)
    {
        case IMMEDIATE: 
            PCB = ip;
            //final = ip;
            break;

        case DIRECT: 
            PCB = (ip + INST.B) % CORESIZE;
            //final = ip + INST.B;
            break;

        case INDIRECT: 
            PCB = (ip + INST.B) % CORESIZE;
            SRB = arena[PCB];
            PCB = (PCB + SRB.B) % CORESIZE;

            //final = ip + INST.B + arena[(ip + INST.B)].B;
            break;
    }
    SRB = arena[PCB];
    BValue = SRB.B;

    // adjust the "final" address.
    //if (final > CORESIZE) final -= CORESIZE;
    //if (final < 0)        final += CORESIZE;
}

int vm_execute(unsigned char owner, unsigned char pid, int address)
{
    unsigned char data_bad = 0;
    int skip = 0;

    unsigned char tempMode;
    unsigned char tempOperand;
    
    ip = address; // do this first
/*
    operandData.opcode = 0;
    operandData.aMode = 0;
    operandData.bMode = 0;
    operandData.A = 0;
    operandData.B = 0;
*/
    //x16_printCell(&INST, "\r\n");
    //printf("   warrior %u, pid %u\n", owner, pid);

    getOperandAData();
    getOperandBData();

    ipNext = ip + 1;

    switch(INST.opcode)
    {
        // 
        //  MOV works
        //
        case MOV:    
            if (INST.aMode == IMMEDIATE)
               arena[PCB].B = AValue;
               //arena[final].B = operandData.A;
            else
               arena[PCB] = SRA;
            break;

        //
        //  ADD and SUB work
        //
        case SUB: 
            ++corewar_system.status;
            AValue = -AValue;
            // fall thru
        case ADD:
            ++corewar_system.status;
            arena[PCB].B = (AValue + BValue + CORESIZE) % CORESIZE;
            if (INST.aMode != IMMEDIATE)
                arena[PCB].A = (SRA.A + SRB.A) % CORESIZE;
            break;

        //
        //  JMP works
        //
        case JMP:
            ipNext = PCA;
            break;

        // 
        // JMN works
        //
        case JMN: // IP = A if B != 0
            if (BValue != 0) ipNext = PCA;
            break;

        case JMZ: // IP = A if B == 0
            if (BValue == 0) ipNext = PCA;
            break;

        case FLP: // Jump to location A if B > status word.
            if ( (BValue > corewar_system.status) ) ipNext = PCA;
            break;

        case SEQ: // IP++ if A == B     fall-through
            ++corewar_system.status;
            if (AValue == BValue) ++ipNext;
            break;

        case SNE: // IP++ if A != B     fall-through
            ++corewar_system.status;
            if (AValue != BValue) ++ipNext;
            break;

        case SLT: // IP++ if A  < B
            ++corewar_system.status;
            if (AValue < BValue) ++ipNext;
            break;

        case SPL: // SPLIT!
            corewar_system.status += 3;
            process_add(owner, PCA);
            break;

        //
        // The XCH (exchange) instructions exchanges the A-field and
        // the B-field of the instruction pointed to by the A-operand.
        //
        case XCH: 
            corewar_system.status += 3;
            tempMode    = SRA.aMode;
            SRA.aMode   = SRA.bMode;
            SRA.bMode   = tempMode;
            tempOperand = SRA.A;
            SRA.A       = SRA.B;
            SRA.B       = tempOperand;
            break;

        case HCF:
            corewar_system.status += 3;
            // fall thru
        default:
            process_remove(owner, pid);
            return -1; // die
    }

    if (ipNext >= CORESIZE) 
        ipNext -= CORESIZE; // cheaper than a mod

    return ipNext; // updated ip
}