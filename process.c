#include "common.h"

#ifdef X16
#include    <conio.h>
#else
#include    <stdio.h>
#endif

#include "arena.h"
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

void process_remove(unsigned char owner, unsigned char pid)
{
    process[owner][pid] = PROCESS_INVALID; // killed
    x16_ps_log("remove-process", owner, pid, process[owner][pid]);
}

void process_dump()
{
    int x, y;
    int count = 0;

    for(x=0; x<WARRIORS_MAX; ++x)
       for(y=0; y<WARRIOR_PROCESSES_MAX; ++y)
          if (process[x][y] > -1)
          {
#ifdef X16
             cprintf("%2d: warrior no. %d, process %d: [%u]\r\n", ++count, x, y, process[x][y]);
#else
             printf("%2d: warrior no. %d, process %d: [%u]\n", ++count, x, y, process[x][y]);
#endif
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
    unsigned char w;
    unsigned char alive;

    for(w=0; w<WARRIORS_MAX; ++w)
    {
       alive = 0;
       // scan to next valid process
       for(i = currentProcess[w]; i < (currentProcess[w] + WARRIOR_PROCESSES_MAX); ++i)
       {
           currentPid = i % WARRIOR_PROCESSES_MAX;
           if (process[w][currentPid] != PROCESS_INVALID)
           {
               process[w][currentPid] = vm_execute(w, currentPid, process[w][currentPid]);
               //
               // Housekeeping
               // 
               currentProcess[w] = currentPid + 1;
               alive = 1;
               break; // next warrior
           }
       }
       liveWarriors += alive;
    }
    return liveWarriors;
}
