#include "SymTable.h"
#include <string.h>

int hash(char* key)
{
	int temp = 0;
	int i = 0;
	while (key[i] != '\0')
	{
		temp = (temp << SYMDIFF);
		temp += key[i];
		temp %= SYMSIZE;
		//temp = ((temp << SYMDIFF) + key[i]) % SYMSIZE;
		++i;
	}
	return temp;
}

BOOL insert_SymTable(SYMTABLE_CONTROL* symtable, AST_TREE* ast, int index)
{
	if ((!ast->nodes[index].isvalid) || (ast->nodes[index].type != TOKEN_TYPE_ID))
		return FALSE;

	int h = hash(ast->nodes[index].strtoken);
	SYMTABLE_DATA* sym = symtable->symtable[h];
	while ((sym != NULL) && (strcmp(ast->nodes[index].strtoken, sym->name) != 0))
		sym = sym->next;

	if (sym == NULL)
	{
		sym = malloc(sizeof(SYMTABLE_DATA));
		if (sym == NULL)
			return ReturnFalse("malloc symtable failed");
		memset(sym, 0, sizeof(SYMTABLE_DATA));
		sym->name = ast->nodes[index].strtoken;

		sym->lines = malloc(sizeof(LINE_COL));
		if (sym->lines == NULL)
			return ReturnFalse("malloc line_col failed");
		sym->lines->lineno = ast->nodes[index].line_no;
		sym->lines->colno = ast->nodes[index].colum_no;
		sym->lines->next = NULL;
		sym->mem_loc = symtable->gl_memloc++;

		symtable->symtable[h] = sym;
	}
	else
	{
		LINE_COL* tmp = sym->lines;
		//如果该变量标示符已经存在，就将当前节点对应的行列号记录到行列链表里。
		while (tmp->next != NULL)
			tmp = tmp->next;

		tmp->next = malloc(sizeof(LINE_COL));
		if (tmp->next == NULL)
			return ReturnFalse("malloc line_col failed");

		tmp->next->lineno = ast->nodes[index].line_no;
		tmp->next->colno = ast->nodes[index].colum_no;
		tmp->next->next = NULL;
	}

	return TRUE;
}

BOOL buildSymTable(SYMTABLE_CONTROL* symtable, AST_TREE* ast)
{
	if ((symtable == NULL) || (ast == NULL))
		return ReturnFalse("symtable or ast is NULL when build symtable");

	for (int i = 0; i < ast->count; i++)
	{
		if (ISTOKENTYPEX(ast, i, TOKEN_TYPE_ID))
		{
			if (insert_SymTable(symtable, ast, i) == FALSE)
			{
				return ReturnFalse("build symtable failed");
			}
		}
	}
	return TRUE;
}
