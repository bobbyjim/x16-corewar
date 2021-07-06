/*
 *  This is the intermediate structure for an instruction line,
 *  used while labels are processed.
 *
 *  It's also the "label table".  I know, sue me.
 *
 */
#include "cell.h"
#include "token.h"
#include "x16.h"
#include "common.h"

#include <stdio.h>
#include <string.h>

Token token[256];
char line[80];
unsigned char currentToken = 0;
unsigned char topToken = 0;
char temp_op_mode;
char temp_op_value[8];

Token* getToken(unsigned char i)
{
   if (i >= topToken) 
      return NULL;
   else
      return &token[i];
}

unsigned char getTokenCount()
{
    return topToken;
}

void token_print(unsigned char index)
{
    printf("%-8s   %3s %c%s, %c%s\n", 
            token[index].label, 
            getOpcodeName(token[index].opcode),
            getMode(token[index].aMode),
            token[index].A,
            getMode(token[index].bMode),
            token[index].B
            );
}

void token_wipe(unsigned char i)
{
   sprintf(token[i].label, "%u", i);
   token[i].opcode = INVALID_OPCODE;
   token[i].aMode  = IMMEDIATE;
   strcpy(token[i].A, "");
   token[i].bMode  = IMMEDIATE;
   strcpy(token[i].B, "");
}

void tokenizer_init()
{
   int i;
   for(i=0; i<256; ++i)
      token_wipe(i);

   topToken = 0;
   temp_op_mode = IMMEDIATE;
   strcpy(temp_op_value, "");
}


 /*
  *  Given a line of raw text, 
  *  replace commas and colons with the 
  *  space character.  Heb 12:1
  */
 void sanitize(char *line)
 {
     int i = 0;
     while(i < strlen(line))
     { 
        if (line[i] == ':' || line[i] == ',')
           line[i] = ' ';
        else if (line[i] == ';')
        {
           line[i] = '\0';
           return;
        }
        ++i;
     }
 }

unsigned char thereIsOnlyOneOperand(char *b)
{
    return EQ(b,"");
}

void noLabel()
{
    // NOP
}

void labelIsIn(char* thelabel)
{
    //printf(" - found label [%s]\r\n", thelabel);
    strncpy(token[topToken].label, thelabel, TOKEN_MAX_STRING_LEN);
}

void opcodeIsIn(char* opcode)
{
    token[topToken].opcode = cell_encode_opcode(opcode);
}

/*
 *  Split the operand into mode + value.
 *
 */
void splitOperand(char* operand)
{
    // find mode
    switch(operand[0]) // 1st char
    {
        case '<': 
           temp_op_mode = PREDECREMENT_INDIRECT; 
           ++operand;
           break;
        case '@': 
           temp_op_mode = INDIRECT; 
           ++operand;
           break;
        case '#': 
           temp_op_mode = IMMEDIATE; 
           ++operand;
           break;
        default : 
           temp_op_mode = DIRECT;
           // DO NOT INCREMENT operand 
           break;
    }

    // value is whatever's left
    strncpy(temp_op_value, operand, TOKEN_MAX_OPERAND_LEN);
}

/*
 *   Remove trailing whitespace.
 */
void chomp(char *string)
{
    string = strtok(string, " ");
}

void operandAIsIn(char *operand)
{
    splitOperand(operand);
    token[topToken].aMode = temp_op_mode;
    strncpy(token[topToken].A, temp_op_value, TOKEN_MAX_OPERAND_LEN);
}

void operandBMightBeIn(char *operand)
{
    if EQ(operand,"")
    {
        token[topToken].bMode = IMMEDIATE;
        strcpy(token[topToken].B, "0");
        return;
    }
    // else
    splitOperand(operand);
    token[topToken].bMode = temp_op_mode;
    strncpy(token[topToken].B, temp_op_value, TOKEN_MAX_OPERAND_LEN);
}

unsigned char isHCF()
{
    return token[topToken].opcode == HCF;
}

//
//  Special case.  
//  If DAT and only one operand, then move it to B.
//
void adjustDAT()
{
    strcpy(token[topToken].B, token[topToken].A);
    token[topToken].bMode = token[topToken].aMode;

    strcpy(token[topToken].A, "0");
    token[topToken].aMode = IMMEDIATE;
}

 /*
  *  Given a line of raw text, sort out the potential
  *  instruction contained there.
  */
 unsigned char tokenize(char* input)
 {
     int count = 0;
     char a[40] = "",
          b[8]  = "",
          c[8]  = "",
          d[8]  = "";

     strncpy(line, input, 80);
     token_wipe(topToken);
     sanitize(line);
     //printf("line: [%s]\n", line);

     if (line[0] == '\0') 
        return INVALID_OPCODE;

     count = sscanf(line, " %s %s %s %s ", a, b, c, d);

     //printf(" - tokenizing: [%s] [%s] [%s] [%s]\r\n", a, b, c, d);
     if (count < 2) // nowhere near enough
        return INVALID_OPCODE;

     if (a[0] == ';') 
        return INVALID_OPCODE;
     
     if (cell_encode_opcode(b) != INVALID_OPCODE)
     {
        labelIsIn(a);
        opcodeIsIn(b);
        chomp(c);
        operandAIsIn(c);
        chomp(d);
        operandBMightBeIn(d);
        if (count == 3 && isHCF())
           adjustDAT();
     }
     else if (cell_encode_opcode(a) != INVALID_OPCODE)
     {
        noLabel();
        opcodeIsIn(a);
        chomp(b);
        operandAIsIn(b);
        chomp(c);
        operandBMightBeIn(c);
        if (count == 2 && isHCF())
           adjustDAT();
     }
     else  
     {
        return INVALID_OPCODE;
     }
     ++topToken;
     return token[topToken-1].opcode;
 }