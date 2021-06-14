#ifndef _TEST_H_
#define _TEST_H_

#include "arena.h"

#define TEST_PASS           0x00
#define TEST_FAIL_OPCODE    0x01
#define TEST_FAIL_A_MODE    0x02
#define TEST_FAIL_A_OPERAND 0x04
#define TEST_FAIL_B_MODE    0x08
#define TEST_FAIL_B_OPERAND 0x10
#define TEST_FAIL_IP        0x20

#define RUN_TEST(testname,startLocation, startString,resultLocation,resultString)  loadCell(startString, startLocation); \
    dumpArena(startLocation,resultLocation); \
    setIp(startLocation); \
    execute(); \
    buildTempCell(resultString); \
    dumpArena(startLocation,resultLocation); \
    testCell(testname, resultLocation); 

#define RUN_IP_TEST(testname,location,command,resultIp) loadCell(command,location); \
    setIp(location); \
    execute(); \
    testIp(testname, location,resultIp);

void setTestVerbosity(unsigned char v);
unsigned char testCell(char* testname, int location);
void testIp(char *testname, int location, int resultIp);

unsigned char getLastTestResult();

#endif