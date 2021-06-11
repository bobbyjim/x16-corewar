#ifndef _COMMON_H_
#define _COMMON_H_

typedef unsigned char byte;
typedef unsigned int  word;

#define     BLACK   0
#define     WHITE   1
#define     CYAN    4
#define     GREEN   5
#define     BLUE    6
#define     YELLOW  7
#define     AMBER   8
#define     LTRED   10
#define     LTGREEN 13
#define     LTBLUE  14
#define     LTGREY  15

#define     DEFAULT_COLOR   LTGREY

#define     EQ(a,b)         (!strcmp(a,b))

#endif
