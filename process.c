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

////////////////////////////////////////////////
//
//  State variables
//
////////////////////////////////////////////////
int process[WARRIORS_MAX][WARRIOR_PROCESSES_MAX]; // current IPs for all PIDs of all warriors
int currentProcess[WARRIORS_MAX];                 // current PIDs per warrior
unsigned char owner;                              // current owner
unsigned char pid;                                // current pid of current owner
int address;                                      // current ip  of current owner
unsigned int epoch;                               // run counter

unsigned char x, y;

unsigned char i,j;
unsigned char liveWarriors;
unsigned char alive;

void process_init()
{
    int w, wp;

    epoch = 0;

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

    epoch = 0; // reset epoch

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
    x16_ps_log("remove-process", owner, pid, process[owner][pid]);
    x16_ps(owner, 'x');
    process[owner][pid] = PROCESS_INVALID; // killed
}

void process_dump()
{
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

/*
   Return WARRIOR_INVALID if there is more than one warrior left alive;
   Otherwise, return the owner ID of the surviving warrior.
 */
unsigned char process_lastWarrior()
{
    unsigned char lastWarrior = WARRIOR_INVALID;

    for(x=0; x<WARRIORS_MAX; ++x)
       for(y=0; y<WARRIOR_PROCESSES_MAX; ++y)
          if (process[x][y] > -1)
          {
              if (lastWarrior == WARRIOR_INVALID)
              {
                lastWarrior = x; // owner number
                break; // exit inner (process) loop
              }
              else // uh oh, there's two alive
              {
                  return WARRIOR_INVALID;
              }
          }

    return lastWarrior;
}

unsigned char process_countWarriors()
{
    liveWarriors = 0;
    for(owner=0; owner<WARRIORS_MAX; ++owner)
    {
       alive = 0;
       // scan to next valid process
       for(i = currentProcess[owner]; i < (currentProcess[owner] + WARRIOR_PROCESSES_MAX); ++i)
       {
           pid = i % WARRIOR_PROCESSES_MAX;
           if (process[owner][pid] == PROCESS_INVALID)
              continue;

            alive = 1;
            break; // next warrior           
       }
       liveWarriors += alive;
    }
    return liveWarriors;
}

//
// run for 1 epoch
// return the number of live warriors
//
unsigned char process_runCorewar() 
{
    liveWarriors = 0;
    for(owner=0; owner<WARRIORS_MAX; ++owner)
    {
       alive = 0;
       // scan to next valid process
       for(i = currentProcess[owner]; i < (currentProcess[owner] + WARRIOR_PROCESSES_MAX); ++i)
       {
           pid = i % WARRIOR_PROCESSES_MAX;
           if (process[owner][pid] == PROCESS_INVALID)
              continue;

            address = process[owner][pid];
            process[owner][pid] = vm_execute(); // owner, pid, process[owner][pid]);
            //
            // Housekeeping
            // 
            currentProcess[owner] = pid + 1;
            alive = 1;
            break; // next warrior           
       }
       liveWarriors += alive;
    }
    return liveWarriors;
}

/*
    Update 'owner', 'pid', and 'address' to the next live process to run.
 */
unsigned char process_prepareNextToRun()
{
    //cprintf("current: owner[%u] pid[%u] address[%d]\r\n", owner, pid, address);
    //cputcxy(70+owner,1,' '); // erase
    for(j=0; j<WARRIORS_MAX; ++j)
    {
       owner = (owner + 1) % WARRIORS_MAX;
       alive = 0;
       // scan to next valid process
       for(i = currentProcess[owner]; i < (currentProcess[owner] + WARRIOR_PROCESSES_MAX); ++i)
       {
           pid = i % WARRIOR_PROCESSES_MAX;
           //printf("checking pid [%u]\n", pid);
           if (process[owner][pid] == PROCESS_INVALID)
              continue;
           alive = 1;
           address = process[owner][pid]; // here's the instruction pointer
           break; // next warrior           
       }
       if (alive) 
          break;   // found next valid PID
    }
    // 'alive' could be used to see if there's anyone left
    // 'owner' was set in the outer loop
    // 'pid' and 'address' were set in the inner loop
    currentProcess[owner] = pid + 1; // prep for next iteration

    //cprintf("next: owner[%u] pid[%u] address[%d]\r\n", owner, pid, address);
    return alive;
}

/*
   Update the ip of the most recently run PID
 */
void process_postExecute(int ip)
{
   process[owner][pid] = ip;
   ++epoch;
}

unsigned char process_continue()
{
    return epoch < MAXIMUM_EPOCHS;
}
