#ifndef _BANK_H_
#define _BANK_H_

#define     CELLS_PER_BANK             1500

void switchToHelpBank();
void setBank(unsigned char bank);
void setCoreBank(int position);

#endif
