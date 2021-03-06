#ifndef _COMMON_H_
#define _COMMON_H_

/*----------------------------------------------------
"#define X16" to compile for the Commander X16.
"#undef X16" to compile for UNIX or whatever.

Best to include it on the compile command with -DX16.
-----------------------------------------------------*/
//#define        X16
//#undef         X16   

#define     CORESIZE                16000
#define     INVALID_LOCATION        30000
#define     OPERAND_SIZE            4096
#define     OPERAND_HALF_SIZE       2048

#define     MAX_WARRIOR_LINES       256
#define     LINE_BUFFER_SIZE        80

#define     MAXIMUM_EPOCHS          16000

#define     LOGICAL_FILE_NUM        1
#define     IGNORE_LFN              0
#define     SD_CARD                 8
#define     EMULATOR_FILE_SYSTEM    8
#define     HOST_FILE_SYSTEM        1
#define     SA_IGNORE_HEADER        0
#define     LOAD_FLAG               0

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
#define     DKGREY  11
#define     LTGREEN 13
#define     LTBLUE  14
#define     LTGREY  15

#define     DEFAULT_COLOR   LTGREY

#define     EQ(a,b)         (!strcmp(a,b))

#define     CIRCLE_FILLED   209
#define     CIRCLE_EMPTY    215
#define     SQUARE_NW       190
#define     SQUARE_NE       188
#define     SQUARE_SW       187
#define     SQUARE_SE       172

#endif
