#ifndef _x16_h_
#define _x16_h_

#include "cell.h"

void x16_init();
void x16_help();
void x16_prompt(int ip);
void x16_top();

void x16_execFail(char *reason);
void x16_printCell(Cell *cell, char* postfix);
void x16_loadfile(char *filename, unsigned int location);

void x16_arena_draw();
void x16_arena_dump(int start, int end);
void x16_arena_ps(unsigned char owner, unsigned char pid, char* op);
void x16_ps_log(char *msg, unsigned char owner, unsigned char pid, int addr);
void x16_pos(unsigned char x, unsigned char y);
void x16_putc(char c);
void x16_putcxy(unsigned char x, unsigned char y, char c);
void x16_puts(char* s);
void x16_putsxy(unsigned char x, unsigned char y, char* s);

#endif
