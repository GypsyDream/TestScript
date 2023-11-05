#include "Token.h"
#include "ASTBuild.h"

int main(int argc, char* argv[])
{
	char* name = "E:\\zengl\\v0.0.8\\zengl_lang_v0.0.8_forXP\\zengl_lang_v0.0.8\\zengl_lang_v0.0.8\\test.zl";
	SOURCE_DATA source;
	memset(&source, 0, sizeof(SOURCE_DATA));

	POINTER_TYPE pointer;
	memset(&pointer, 0, sizeof(POINTER_TYPE));

	TOKEN_DATA data[100];
	memset(data, 0, sizeof(TOKEN_DATA) * 100);

	AST_TREE Ast;
	memset(&Ast, 0, sizeof(AST_TREE));

	source.filename = name;
	source.needread = TRUE;
	source.curline = 1;

	int count = 0;
	while (1)
	{
		if (GetToken(&source, &data[count]) == FALSE)
			break;

		if (AddNode(&data[count], &Ast) == FALSE)
			break;

		if (data[count++].type == TOKEN_TYPE_ENDFILE)
			break;
	}

	for (int i = 0; i < count; i++)
	{
		if (data[i].type == TOKEN_TYPE_STR)
			printf("'%s' ", data[i].str);
		else
			printf("%s ", data[i].str);
		if (data[i].str[0] == ';')
			printf("\n");
	}

	system("pause");
	return 0;
}