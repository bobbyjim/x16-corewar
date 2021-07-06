#ifndef _BANK_H_
#define _BANK_H_

#define     REDCODE_BANK                1
#define     HELP_BANK                   2
#define     CELLS_PER_BANK              1500

void switchToHelpBank();
void setBank(unsigned char bank);
void setCoreBank(int position);


#endif
