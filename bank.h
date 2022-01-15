#ifndef _BANK_H_
#define _BANK_H_

#define     REDCODE_BANK                1
#define     HELP_BANK                   2

//
//   CELLS_PER_BANK used to be 1500.
//
//   If it were 1024, then we could shift instead of divide when 
//   determining bank number... and yes, we'd go from 11 to 16 banks.  
//   So what?
//
#define     CELLS_PER_BANK              1024

void switchToHelpBank();
void setBank(unsigned char bank);
void setCoreBank(int position);


#endif
