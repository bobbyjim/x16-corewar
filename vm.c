#include <stdio.h>

#include "cell.h"
#include "arena.h"
#include "process.h"
#include "x16.h"

extern Cell arena[CORESIZE];

#define     INST        arena[ip]

Cell            operandData;
int             ip, ipPrev, source, destination, final;
int             tempInt;

Cell            SRA,    SRB;
int             PCA,    PCB;
int             AValue, BValue;

typedef struct {
    unsigned int status : 12;
} System;

System system;

void getOperandAData()
{
    switch(INST.aMode)
    {
        case IMMEDIATE: // 
            PCA = ip;
            AValue = arena[PCA].A;

            operandData.A = INST.A; // only assign the B field into operand
            break;

        case DIRECT: // 
            PCA = (ip + INST.A) % CORESIZE;
            AValue = arena[PCA].B;

            operandData = arena[(ip + INST.A) % CORESIZE];
            break;

        case INDIRECT: // 
            PCA = (ip + INST.A) % CORESIZE;
            SRA = arena[PCA];
            PCA = (PCA + SRA.B) % CORESIZE;
            AValue = arena[PCA].B;

            source = arena[(ip + INST.A) % CORESIZE].B;
            operandData = arena[(source + ip + INST.A) % CORESIZE];
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
            final = ip;
            break;

        case DIRECT: 
            PCB = (ip + INST.B) % CORESIZE;
            final = ip + INST.B;
            break;

        case INDIRECT: 
            PCB = (ip + INST.B) % CORESIZE;
            SRB = arena[PCB];
            PCB = (PCB + SRB.B) % CORESIZE;

            final = ip + INST.B + arena[(ip + INST.B)].B;
            break;
    }
    SRB = arena[PCB];
    BValue = SRB.B;

    // adjust the "final" address.
    if (final > CORESIZE) final -= CORESIZE;
    if (final < 0)        final += CORESIZE;
}

int vm_execute(unsigned char owner, unsigned char pid, int address)
{
    unsigned char data_bad = 0;
    int skip = 0;

    unsigned char tempMode;
    unsigned char tempOperand;
    
    ip = address; // gotta do this first!!

    operandData.opcode = 0;
    operandData.aMode = 0;
    operandData.bMode = 0;
    operandData.A = 0;
    operandData.B = 0;

    x16_printCell(&INST, "\r\n");
    printf("   warrior %u, pid %u\n", owner, pid);

    getOperandAData();
    getOperandBData();

    ipPrev = ip;
    ++ip;

    switch(INST.opcode)
    {
        //
        //  MOV works
        //
        case MOV: 
            if (INST.aMode == IMMEDIATE)
                arena[final].B = operandData.A;
            else
            {
                x16_printCell(&arena[final], "<-- before\r\n");
                arena[final] = operandData;
                x16_printCell(&arena[final], "<-- after\r\n");
            }
            break;

        //
        //  ADD and SUB work
        //
        case SUB: 
            operandData.B = CORESIZE - operandData.A;
            printf("op.b = %d\r\n", operandData.B);
            // fall thru
        case ADD:
            tempInt = arena[final].B + operandData.B;
            if (tempInt > CORESIZE) tempInt -= CORESIZE;

            printf("arena[final].b = %d,  op.a = %d,  sum = %d\r\n", arena[final].B, operandData.A, tempInt);
            arena[final].B = tempInt;           
            break;

        case JMP:
            ip = PCA;
            break;

        case JMN: // IP = A if B != 0
            if (BValue != 0)
                ip = PCA;
            break;

        case JMZ: // IP = A if B == 0
            if (BValue == 0)
                ip = PCA;
            break;

        case FLP: // Jump to location A if B > status word.
            if (  (BValue > system.status) )
                 ip = PCA;
            break;

        case SEQ: // IP++ if A == B     fall-through
            if (AValue == BValue) ++ip;
            break;

        case SNE: // IP++ if A != B     fall-through
            if (AValue != BValue) ++ip;
            break;

        case SLT: // IP++ if A  < B
            if (AValue < BValue) ++ip;
            break;

        // 
        // SPL works
        //
        case SPL: // SPLIT!
            final = (ipPrev + INST.A) % CORESIZE;
            process_add(owner, final);
            break;

        //
        // The XCH (exchange) instructions exchanges the A-field and
        // the B-field of the instruction pointed to by the A-operand.
        //
        case XCH: 
            tempMode    = SRA.aMode;
            SRA.aMode   = SRA.bMode;
            SRA.bMode   = tempMode;
            tempOperand = SRA.A;
            SRA.A       = SRA.B;
            SRA.B       = tempOperand;
            break;

        //
        // HCF works
        //
        case HCF:
        default:
            process_remove(owner, pid);
            return -1; // die
    }

    ++system.status; // whatever

    if (ip >= CORESIZE) 
        ip -= CORESIZE; // cheaper than a mod

    printf("\n");
    return ip; // updated ip
}