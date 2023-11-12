#include "Token.h"
#include "ASTBuild.h"
#include "SymTable.h"

void PrintNode(NODE_DATA* node, int index)
{
	printf("Node Index:%d\nNode Row Ans Colnum:%d,%d\n", index, node[index].line_no, node[index].colum_no);
	printf("Node str: %s\n", node[index].strtoken);
	printf("Node child num: %d\n", node[index].childs.count);
	for (int i = 0; i < node[index].childs.count; i++)
	{
		printf("Node %d child : %d\n", i, node[index].childs.childnum[i]);
		printf("Child str:%s\n", node[node[index].childs.childnum[i]].strtoken);
	}
	printf("\n");
}
void PrintSymbal(SYMTABLE_DATA* sym)
{
	SYMTABLE_DATA* psym = sym;
	while (psym != NULL)
	{
		LINE_COL* lines = psym->lines;
		printf("%d %s", psym->mem_loc, psym->name);
		while (lines != NULL)
		{
			printf(" %d:%d", lines->lineno, lines->colno);
			lines = lines->next;
		}
		printf("\n");
		psym = psym->next;
	}
}

int main(int argc, char* argv[])
{
	char* name = "test.txt";

	YU_LANGUAGE_DATA data;
	memset(&data, 0, sizeof(YU_LANGUAGE_DATA));
	InitAstTree(&data.ast);

	data.source.filename = name;
	data.source.needread = TRUE;
	data.source.curline = 1;

	int count = 0;
	if (GetTokenEx(&data.source, &data.token) == FALSE)
		Fault(NULL, "Get Token Failed");

	if(BuildAstTreeEx(&data.token,&data.ast)==FALSE)
		Fault(NULL, "Ast tree Failed");

	for (int i = 0; i < data.ast.count; i++)
	{
		PrintNode(data.ast.nodes, i);
	}

	if (buildSymTable(&data.symtable, &data.ast) == FALSE)
		Fault(NULL, "Sym Table Failed");

	for (int i = 0; i < SYMSIZE; i++)
	{
		PrintSymbal(data.symtable.symtable[i]);
	}

	system("pause");
	return 0;
}