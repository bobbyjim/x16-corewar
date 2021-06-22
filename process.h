#ifndef _PROCESS_H_
#define _PROCESS_H_

#define     WARRIORS_MAX                8
#define     WARRIOR_PROCESSES_MAX       8
#define     PROCESS_INVALID             -1

void process_init();
void process_add(unsigned char owner, unsigned int address);
void process_remove(unsigned char owner, unsigned int address);

unsigned char process_runCorewar();

#endif
