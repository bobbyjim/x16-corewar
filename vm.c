#include <stdio.h>

#include "cell.h"
#include "bank.h"
#include "arena.h"
#include "process.h"
#include "x16.h"

//extern Cell arena[CORESIZE];
extern unsigned char corewar_system_status;

//#define     INST        arena[ip]
extern unsigned char currentBank; // from bank.c

Cell           *inst_ptr;
Cell            inst;
int             ip,     ipNext; 
int             PCA,    PCB;
int             PCAI,   PCBI;
Cell           *SRA,   *SRB;
int             AValue, BValue, BAValue;
int             tempValue;
unsigned char   tempMode;
unsigned char   tempOperand;

/////////////////////////////////
//
//  From process.c
//
/////////////////////////////////
extern unsigned char owner;
extern unsigned char pid;
extern int address;

void getOperandAData()
{
    switch(inst.aMode)
    {
        case IMMEDIATE:
            PCA = ip;
            AValue = inst.A;
            break;

        case DIRECT:
            PCA = (ip + inst.A) % CORESIZE;
            AValue = arena_getLocation(PCA)->B;
            break;

        case INDIRECT:
            PCA = (ip + inst.A) % CORESIZE;
            SRA = arena_getLocation(PCA);
            PCA = (PCA + SRA->B) % CORESIZE;
            AValue = arena_getLocation(PCA)->B;
            break;

        case PREDECREMENT_INDIRECT: // what a mess!
            PCA = (ip + inst.A) % CORESIZE;
            BValue = arena_getLocation(PCA)->B;
            PCAI = PCA;
            PCA  = (PCAI + CORESIZE - 1) % CORESIZE;
            tempValue = arena_getLocation(PCAI)->B;
            arena_getLocation(PCAI)->B = (tempValue + CORESIZE - 1) % CORESIZE;
            PCA = (PCA + BValue) % CORESIZE;
            AValue = arena_getLocation(PCA)->B;
            break;
    }
    SRA = arena_getLocation(PCA);
}

void getOperandBData()
{
    switch(inst.bMode)
    {
        case IMMEDIATE: 
            PCB = ip;
            break;

        case DIRECT: 
            PCB = (ip + inst.B) % CORESIZE;
            break;

        case INDIRECT: 
            PCB = (ip + inst.B) % CORESIZE;
            SRB = arena_getLocation(PCB);
            PCB = (PCB + SRB->B) % CORESIZE;
            break;

        case PREDECREMENT_INDIRECT:
            PCB = (ip + inst.B) % CORESIZE;
            BValue = arena_getLocation(PCB)->B;
            PCBI = PCB;
            PCB = (PCB + CORESIZE - 1) % CORESIZE;
            tempValue = arena_getLocation(PCBI)->B;
            arena_getLocation(PCBI)->B = (tempValue + CORESIZE - 1) % CORESIZE;
            PCB = (PCB + BValue) % CORESIZE;
            break;
    }
    SRB = arena_getLocation(PCB);
    BValue = SRB->B;
    BAValue = SRB->A;

    // adjust the "final" address.
    //if (final > CORESIZE) final -= CORESIZE;
    //if (final < 0)        final += CORESIZE;
}

int vm_execute() // unsigned char owner, unsigned char pid, int address)
{    
    ip = address; // do this first
    x16_arena_touch(ip, owner);

    inst_ptr = arena_getLocation(ip);
    inst = *inst_ptr; // copy

    getOperandAData();
    getOperandBData();

    ipNext = ip + 1;

    switch(inst.opcode)
    {
        // 
        //  MOV works
        //
        case MOV:    
            x16_arena_touch(PCB, owner);
            if (inst.aMode == IMMEDIATE)
               arena_getLocation(PCB)->B = AValue; // arena[PCB].B = AValue;
            else
               arena_setData(PCB, PCA);  //arena_setLocation(PCB, SRA); // [PCB] = SRA;
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
            arena_getLocation(PCB)->B = (AValue + BValue + CORESIZE) % CORESIZE;
            if (inst.aMode != IMMEDIATE)
                arena_getLocation(PCB)->A = (AValue + BAValue) % CORESIZE; // (SRA->A + SRB->A) % CORESIZE; // arena[PCB].A = (SRA.A + SRB.A) % CORESIZE;
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
           //arena[PCB].B = (arena[PCB].B + CORESIZE - 1) % CORESIZE;
           //arena_getLocation(PCB)->B = (arena_getLocation(PCB)->B + CORESIZE - 1) % CORESIZE;
           BValue = (BValue + CORESIZE - 1) % CORESIZE;
           arena_getLocation(PCB)->B = BValue;
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

        case DEC:
        case INC:  // TO DO
           break;  

        //
        // The XCH (exchange) instructions exchanges the A-field and
        // the B-field of the instruction pointed to by the A-operand.
        //
        case XCH: 
            x16_arena_touch(PCA, owner);
            x16_arena_touch(PCB, owner);
            corewar_system_status += 3;
            SRA          = arena_getLocation(PCA);
            tempMode     = SRA->aMode;
            SRA->aMode   = SRA->bMode;
            SRA->bMode   = tempMode;
            tempOperand  = SRA->A;
            SRA->A       = SRA->B;
            SRA->B       = tempOperand;
            break;

        case HCF:
            corewar_system_status += 3;
            // fall thru
        default:
            x16_arena_touch(ip, owner);
            process_remove(owner, pid);
            return -1; // die
    }

    if (ipNext >= CORESIZE) 
        ipNext -= CORESIZE; // cheaper than a mod

    x16_arena_touch(ipNext, -1);

    return ipNext; // updated ip
}