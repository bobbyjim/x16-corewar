#ifndef _x16_h_
#define _x16_h_

#include "cell.h"

void x16_init();
void x16_help();
void x16_opcode_help();

void x16_prompt(int ip);
void x16_top();
void x16_clrscr();
int x16_getc();

void x16_printCell(Cell *cell, char* postfix);
void x16_loadfile(char *filename, unsigned int location);

void x16_arena_draw();
void x16_arena_dump(int start, int end);
void x16_arena_ps(unsigned char owner, unsigned char pid, char* op);
void x16_arena_touch(int ip, unsigned char owner);

void x16_ps_log(char *msg, unsigned char owner, unsigned char pid, int addr);
void x16_msg(char* s);
void x16_msg2(char* a, char *b);
void x16_putValue(char* label, unsigned int value);

void x16_ps(unsigned char owner, char state);
#endif
