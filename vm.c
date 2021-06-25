#include <stdio.h>

#include "cell.h"
#include "arena.h"
#include "process.h"
#include "x16.h"

extern Cell arena[CORESIZE];
extern unsigned char corewar_system_status;

#define     INST        arena[ip]

int             ip,     ipNext; 
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
    x16_arena_touch(ip, owner);

    getOperandAData();
    getOperandBData();

    ipNext = ip + 1;

    switch(INST.opcode)
    {
        // 
        //  MOV works
        //
        case MOV:    
            x16_arena_touch(PCB, owner);
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
            ++corewar_system_status;
            AValue = -AValue;
            // fall thru
        case ADD:
            ++corewar_system_status;
            x16_arena_touch(PCB, owner);
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
            if ( (BValue > corewar_system_status) ) ipNext = PCA;
            break;

        case DJN:
           x16_arena_touch(PCB, owner);
           arena[PCB].B = (arena[PCB].B + CORESIZE - 1) % CORESIZE;
            if (BValue != 1)     // ((BValue-1) != 0) 
               ipNext = PCA;
            break;

        case SEQ: // IP++ if A == B     fall-through
            ++corewar_system_status;
            if (AValue == BValue) ++ipNext;
            break;

        case SNE: // IP++ if A != B     fall-through
            ++corewar_system_status;
            if (AValue != BValue) ++ipNext;
            break;

        case SLT: // IP++ if A  < B
            ++corewar_system_status;
            if (AValue < BValue) ++ipNext;
            break;

        case SPL: // SPLIT!
            corewar_system_status += 3;
            process_add(owner, PCA);
            break;

        //
        // The XCH (exchange) instructions exchanges the A-field and
        // the B-field of the instruction pointed to by the A-operand.
        //
        case XCH: 
            x16_arena_touch(PCA, owner);
            x16_arena_touch(PCB, owner);
            corewar_system_status += 3;
            tempMode    = SRA.aMode;
            SRA.aMode   = SRA.bMode;
            SRA.bMode   = tempMode;
            tempOperand = SRA.A;
            SRA.A       = SRA.B;
            SRA.B       = tempOperand;
            //
            //  I betcha we have to write these back to the arena!
            //
            break;

        case HCF:
            corewar_system_status += 3;
            // fall thru
        default:
            process_remove(owner, pid);
            return -1; // die
    }

    if (ipNext >= CORESIZE) 
        ipNext -= CORESIZE; // cheaper than a mod

    x16_arena_touch(ipNext, -2);

    return ipNext; // updated ip
}