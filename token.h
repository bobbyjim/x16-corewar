#ifndef _token_h_
#define _token_h_


#define     TOKEN_MAX_STRING_LEN        8
#define     TOKEN_MAX_OPERAND_LEN       8

 //
 //  An intermediate structure before final encoding.
 //
 typedef struct {
   
   char label[TOKEN_MAX_STRING_LEN+1];

   unsigned int opcode: 5;
   unsigned int aMode : 2;
   unsigned int bMode : 2;

   char A[TOKEN_MAX_OPERAND_LEN+1];
   char B[TOKEN_MAX_OPERAND_LEN+1];

 } Token;

 #endif

void token_print(unsigned char index);
void tokenizer_init();
unsigned char tokenize(char* line);

Token* getToken(unsigned char i);
unsigned char getTokenCount();