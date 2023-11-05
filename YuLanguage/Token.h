#ifndef TOKEN_H
#define TOKEN_H
#include "GlobalStruct.h"

BOOL GetNextChar(SOURCE_DATA* source, char* mch);

BOOL makeTokenStr(char ch, TOKEN_DATA* outData);

void ungetchar(SOURCE_DATA* source);

BOOL lookupReserve(TOKEN_DATA* outData);

BOOL GetToken(SOURCE_DATA* source, TOKEN_DATA* outData);

#endif