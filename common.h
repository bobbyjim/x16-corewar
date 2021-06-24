#ifndef _COMMON_H_
#define _COMMON_H_


/*--------------------------------------------
"#define X16" to compile for the Commander X16.
"#undef X16" to compile for UNIX or whatever.
--------------------------------------------*/
//#define        X16
#undef         X16   


#define LOGICAL_FILE_NUM        1
#define IGNORE_LFN              0
#define SD_CARD                 8
#define EMULATOR_FILE_SYSTEM    8
#define HOST_FILE_SYSTEM        1
#define SA_IGNORE_HEADER        0
#define LOAD_FLAG               0

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

void setVerbosity(unsigned char v);
void bumpVerbosity();
unsigned char isVerbose();

#endif
