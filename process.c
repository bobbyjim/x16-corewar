#include "arena.h"
#include "common.h"
#include "process.h"
#include "vm.h"
#include "x16.h"


int process[WARRIORS_MAX][WARRIOR_PROCESSES_MAX];
int currentProcess[WARRIORS_MAX];

void process_init()
{
    int w, wp;

    for(w=0; w<WARRIORS_MAX; ++w)
    {
        currentProcess[w] = 0;
        for(wp=0; wp<WARRIOR_PROCESSES_MAX; ++wp)
            process[w][wp] = PROCESS_INVALID;
    }
}

void process_add(unsigned char owner, unsigned int address)
{
    unsigned char pid;
    unsigned char foundpid;

    address %= CORESIZE;
    foundpid = PROCESS_INVALID;

    for(pid=0; pid<WARRIOR_PROCESSES_MAX; ++pid)
        if (process[owner][pid] == PROCESS_INVALID) // available slot
        {
            foundpid = pid;
            break;
        }

    if (foundpid <= WARRIOR_PROCESSES_MAX)
    {
       process[owner][foundpid] = address; // added
       x16_ps_log("add-process", owner, foundpid, address);
    }
    else
    {
       x16_ps_log("FAILED add-process", owner, foundpid, address);
    }

}

void process_remove(unsigned char owner, unsigned int address)
{
    unsigned char pid;
    unsigned char found = 255;

    address %= CORESIZE;

    for(pid=0; pid<WARRIOR_PROCESSES_MAX; ++pid)
        if (process[owner][pid] == address) // found it
            found = pid;

    if (found <= WARRIOR_PROCESSES_MAX)
    {
       process[owner][found] = PROCESS_INVALID; // killed
       x16_ps_log("remove-process", owner, found, address);
    }
    else
    {
       x16_ps_log("FAILED remove-process", owner, found, address);
    }
}

//
// run for 1 epoch
// return the number of live warriors
//
unsigned char process_runCorewar() 
{
    unsigned char currentPid;
    unsigned char i;
    unsigned char liveWarriors = 0;
    unsigned char w = WARRIORS_MAX;

    while(w) 
    {
       --w;
       // scan to next valid process
       for(i = currentProcess[w]; i < (currentProcess[w] + WARRIOR_PROCESSES_MAX); ++i)
       {
           currentPid = i % WARRIOR_PROCESSES_MAX;
           if (process[w][currentPid] != PROCESS_INVALID)
           {
               currentProcess[w] = currentPid;
               process[w][currentPid] = vm_execute(w, process[w][currentPid]);
               //x16_arena_ps(w,currentPid,process[w][currentPid]);
               //arena_draw();
               ++liveWarriors;
               break; // next warrior
           }
       }
    }
    return liveWarriors;
}

