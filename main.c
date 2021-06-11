
#include <cbm.h>
#include <conio.h>

#include "common.h"
#include "arena.h"
#include "cell.h"
#include "vm.h"

void main() //int argc, char* argv[])
{
    int status;
    int x;

    cbm_k_bsout(0x8E); // revert to primary case

    clearArena();
    bgcolor(BLACK);
    textcolor(GREEN);
    clrscr();

    parseCell("mov 0 1", 1000);
    dumpArena(1000,5);

    setIp(1000);
    drawArena();
    for(x=0; x<CORESIZE; ++x)
    {
       drawCell(getIp());
       status = executeCorewar();
       drawCell(getIp());
    }
    cprintf("\r\n\r\n");
}

    /*
    parseCell("hcl 20 30", 1000);
    parseCell("mov 20 30", 1001);
    parseCell("add 20 30", 1002);
    parseCell("mul 20 30", 1004);
    parseCell("div 20 30", 1005);
    parseCell("mod 20 30", 1006);
    parseCell("jmn 20 30", 1008);
    parseCell("jmz 20 30", 1009);
    parseCell("ske 20 30", 1012);
    parseCell("skn 20 30", 1013);
    parseCell("spl 20 30", 1014);
    */