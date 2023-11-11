#ifndef ASTBUILD_H
#define ASTBUILD_H
#include "GlobalStruct.h"

BOOL InitAstTree(AST_TREE* ast);

BOOL AddNode(TOKEN_DATA* token, AST_TREE* ast);



BOOL BuildAstTree(AST_TREE* ast);


BOOL BuildAstTreeEx(TOKEN_ARRAY* token, AST_TREE* ast);
#endif
