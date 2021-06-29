#ifndef _PROCESS_H_
#define _PROCESS_H_

#define     WARRIORS_MAX                8
#define     WARRIOR_PROCESSES_MAX       8
#define     PROCESS_INVALID             -1
#define     WARRIOR_INVALID             255

void process_init();
void process_add(unsigned char owner, unsigned int address);
void process_remove(unsigned char owner, unsigned char pid);
void process_dump();

unsigned char process_runCorewar();
unsigned char process_countWarriors();
unsigned char process_lastWarrior();
unsigned char process_prepareNextToRun();
void process_postExecute(int ip);
unsigned char process_continue();

#endif
