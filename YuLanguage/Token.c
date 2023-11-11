#include "Token.h"

const char* reserves[] = { "", "print","if","then","elif","else","endif",NULL };

BOOL GetNextChar(SOURCE_DATA* source, char* mch)
{
	char ch;
start:
	if (source->file == NULL)
	{
		fopen_s(&source->file , source->filename, "r");
		if (source->file == NULL)
			return ReturnFalse("can't open file");
	}
	if (source->needread == TRUE || source->cur >= BUF_SIZE)
	{
		if (fgets(source->buf, BUF_SIZE, source->file) == NULL)
		{
			if (feof(source->file))
			{
				*mch = EOF;
				return TRUE;
			}
			else
				return ReturnFalse("can't fgets to read file content.");
		}
		source->needread = FALSE;
		source->cur = 0;
	}
	if ((ch = source->buf[source->cur++]) == STRNULL)
	{
		source->needread = TRUE;
		goto start;
	}
	/*if(feof(source->file))
		source->is_end = TRUE;*/
	source->curcolum++;
	*mch = ch;
	return TRUE;
}

BOOL makeTokenStr(char ch, TOKEN_DATA* outData)
{
	if (outData->str == NULL)
	{
		outData->size += Default_Token_Size;
		outData->str = malloc(sizeof(char) * outData->size);
	}
	else if (outData->size - outData->count >= 1)
	{
		outData->size += Default_Token_Size;
		char* Buf = NULL;
		Buf = realloc(outData->str , sizeof(char) * outData->size);
		if (Buf == NULL)
			return ReturnFalse("makeTokenStr alloc failure.");
		outData->str = Buf;
	}
	if (outData->str == NULL)
		return ReturnFalse("makeTokenStr alloc failure.");

	outData->str[outData->count++] = ch;
	outData->str[outData->count] = STRNULL;
	return TRUE;
}

void ungetchar(SOURCE_DATA* source)
{
	source->cur > 0 ? source->cur-- : source->cur;
	source->curcolum > 0 ? source->curcolum-- : source->curcolum;
}

BOOL lookupReserve(TOKEN_DATA* outData)
{
	int len = strlen(outData->str);
	int i = 0;
	while (reserves[i] != NULL)
	{
		int len_reserve = strlen(reserves[i]);
		if (len == len_reserve &&
			reserves[i][0] == outData->str[0] &&
			!strncmp(reserves[i], outData->str, len))
		{
			outData->type = TOKEN_TYPE_RESERVE;
			outData->reserve = i;  //设置关键词的枚举值
			break;
		}
		i++;
	}
	return TRUE;
}

BOOL GetToken(SOURCE_DATA* source, TOKEN_DATA* outData)
{
	PROCESS_STATES state = STATES_START;
	char startStr_ch;
	char ch;

	while (state != STATES_DOWN)
	{
		if (GetNextChar(source, &ch) == FALSE)
			return ReturnFalse("get char failed");
		switch (state)
		{
		case STATES_START:
			//开始阶段记录token起始行列
			outData->curline = source->curline;
			outData->curcolum = source->curcolum;
			if (ch == ' ' || ch == '\t' || ch == '\n') //如果是换行回车之类的就跳过。
			{
				if (ch == '\n')//换行文件读取地址记录
				{
					source->curline++;
					source->curcolum = 0;
				}
				continue;
			}
			else if (isalpha(ch)) //判断读取的字符是否是英文字母。
			{
				state = STATES_INID; //如果是字母，我们就将state状态机设置为INID 。
				makeTokenStr(ch, outData); //然后将读取出来的ch字符通过函数makeTokenStr加入到TokenString动态字符串里
			}
			else if (isdigit(ch)) //判断读取的字符是否是数字
			{
				state = STATES_INNUM; //如果是数字，我们就将state状态机设置为INNUM 。
				makeTokenStr(ch, outData); //然后将读取出来的ch字符通过函数makeTokenStr加入到TokenString动态字符串里
			}
			else if (ch == '\'' || ch == '"') //判断是否是字符串
			{
				state = STATES_INSTR;
				startStr_ch = ch;
//				outData->str[0] = STRNULL;//未申请内存，野指针
			}
			else
			{
				switch (ch)
				{
				case '+':
					state = STATES_DOWN; //如果字符是‘+’号就将state状态机设为DOWN,这样就可以结束循环，并把token设为PLUS枚举值，表示找到加号运算符。
					outData->type = TOKEN_TYPE_PLUS;
					break;
				case '-':
					state = STATES_DOWN; //和上面同理
					outData->type = TOKEN_TYPE_MINIS;
					break;
				case '*':
					state = STATES_DOWN; //和上面同理
					outData->type = TOKEN_TYPE_TIMES;
					break;
				case '/':
					state = STATES_DOWN;
					outData->type = TOKEN_TYPE_DIVIDE;
					break;
				case '=':
					state = STATES_INASSIGN;
					break;
				case '>':
					state = STATES_INGREAT;  //大于字符进入INGREAT状态
					break;
				case '<':
					state = STATES_INLESS;  //小于字符进入INLESS状态
					break;
				case '!':
					state = STATES_INNOT_EQ;  //不等于或取反符号进入INNOT_EQ状态
					break;
				case '&':
					state = STATES_INAND;  //逻辑且
					break;
				case '|':
					state = STATES_INOR;  //逻辑或
					break;
				case '(':
					state = STATES_DOWN;
					outData->type = TOKEN_TYPE_LBRACKET;
					break;
				case ')':
					state = STATES_DOWN;
					outData->type = TOKEN_TYPE_RBRACKET;
					break;
				case EOF:
					state = STATES_DOWN; //EOF字符表示读取到了文件的结尾，则返回ENDFILE的token，在外层main函数的主循环就会结束扫描。
					outData->type = TOKEN_TYPE_ENDFILE;
					break;
				case ';':
					state = STATES_DOWN; //分号表示单条语句结束
					outData->type = TOKEN_TYPE_SEMI;
					break;
				default:
					state = STATES_DOWN; //其他情况下表示读取到了未定义的token，那么就返回ERROR。
					outData->type = TOKEN_TYPE_ERROR;
					break;
				}//switch(ch)
				makeTokenStr(ch, outData);
			}
			break;
		case STATES_INID:
			if (isalnum(ch) || ch == '_') //在INID状态下，一直读取字符，直到该字符不是字母为止，并将读取的字母通过makeTokenStr构成完整的标示符。标示符可以包含下划线！
				makeTokenStr(ch, outData);
			else
			{
				state = STATES_DOWN;
				outData->type = TOKEN_TYPE_ID; //将token设为ID表示读取到了一个标示符。ID即identifier 英文缩写。
				ungetchar(source); //因为多读取了一个非字母的字符，所以用ungetchar函数来回退一个字符，供下一次扫描使用。
			}
			break;
		case STATES_INNUM:
			if (isdigit(ch))
				makeTokenStr(ch, outData);
			else if (ch == '.')  //如果是小数点，就说明是浮点数，进入INFLOAT状态
			{
				makeTokenStr(ch, outData);
				state = STATES_INFLOAT;
			}
			else
			{
				state = STATES_DOWN;
				outData->type = TOKEN_TYPE_NUM; //将token设为NUM表示读取到了一个数字。
				ungetchar(source);
			}
			break;
		case STATES_INFLOAT:  //在INFLOAT状态下，一直读取字符，直到该字符不是数字为止，这样就可以得到一个浮点数。
			//由于前面读取过小数点了，故此处仅允许数字
			if (isdigit(ch))
				makeTokenStr(ch, outData);
			else
			{
				state = STATES_DOWN;
				outData->type = TOKEN_TYPE_FLOAT;
				ungetchar(source);
			}
			break;
		case STATES_INSTR:
			if (ch != startStr_ch) //如果再次遇到startStr_ch即字符串开始的单引号或双引号，就表示一个字符串结束
				makeTokenStr(ch, outData); //将双引号或单引号内的字符加入到TokenString中。
			else
			{
				state = STATES_DOWN;
				outData->type = TOKEN_TYPE_STR; //将token设为STR表示读取到了一个字符串
			}
			break;
		case STATES_INASSIGN:  //如果是'=='就是EQUAL等于运算符，如果只有一个'='就是ASSIGN赋值运算符
			state = STATES_DOWN;
			if (ch == '=')
			{
				makeTokenStr(ch, outData);
				outData->type = TOKEN_TYPE_EQUAL;
			}
			else
			{
				outData->type = TOKEN_TYPE_ASSIGN;
				ungetchar(source);
			}
			break;
		case STATES_INGREAT:  //如果是'>='，token就是GREAT_EQ大于等于运算符，否则就是GREAT大于运算符
			state = STATES_DOWN;
			if (ch == '=')
			{
				makeTokenStr(ch, outData);
				outData->type = TOKEN_TYPE_GREAT_EQ;
			}
			else
			{
				outData->type = TOKEN_TYPE_GREAT;
				ungetchar(source);
			}
			break;
		case STATES_INLESS:  //如果后面还接了等号即'<='，那么token就是LESS_EQ小于等于运算符，否则就是LESS小于运算符
			state = STATES_DOWN;
			if (ch == '=')
			{
				makeTokenStr(ch, outData);
				outData->type = TOKEN_TYPE_LESS_EQ;
			}
			else
			{
				outData->type = TOKEN_TYPE_LESS;
				ungetchar(source);
			}
			break;
		case STATES_INNOT_EQ:  //!后面接了'='即'!='则表示token是NOT_EQ不等于运算符，否则就是'!'即REVERSE取反运算符
			state = STATES_DOWN;
			if (ch == '=')
			{
				makeTokenStr(ch, outData);
				outData->type = TOKEN_TYPE_NOT_EQ;
			}
			else
			{
				outData->type = TOKEN_TYPE_REVERSE;
				ungetchar(source);
			}
			break;
		case STATES_INAND:  //两个&在一起即'&&'表示token是AND逻辑且运算符。
			if (ch == '&')
			{
				makeTokenStr(ch, outData);
				state = STATES_DOWN;
				outData->type = TOKEN_TYPE_AND;
			}
			else
				return ReturnFalse("\n word parse error, '&' must with '&' now: %d:%d (词法扫描时发生错误：&符号后面必须再接一个&才能构成逻辑且，否则是无效符号)",
					source->curline, source->curcolum);
			break;
		case STATES_INOR:  //两个|在一起即'||'表示token是逻辑或运算符
			if (ch == '|')
			{
				makeTokenStr(ch, outData);
				state = STATES_DOWN;
				outData->type = TOKEN_TYPE_OR;
			}
			else
				return ReturnFalse("\n word parse error, '|' must with '|' now: %d:%d (词法扫描时发生错误：'|'符号后面必须再接一个'|'才能构成逻辑或，否则是无效符号)",
					source->curline, source->curcolum);
			break;
		}
	}
	if (outData->type == TOKEN_TYPE_ID)
	{
		if (lookupReserve(outData) == FALSE)
			return ReturnFalse("look up reserve failed");
	}
		
	return TRUE;
}


BOOL ControlTokenArray(TOKEN_ARRAY* arr)
{
	if (arr->token == NULL)
	{
		arr->size = Default_TokenArray_Size;
		arr->token = malloc(arr->size * sizeof(TOKEN_DATA));
		if (arr->token == NULL)
			return ReturnFalse("Malloc token array failed");
		memset(arr->token, 0, arr->size * sizeof(TOKEN_DATA));
	}
	else if (arr->count >= arr->size)
	{
		arr->size += Default_TokenArray_Size;
		TOKEN_DATA* newBuf = realloc(arr->token, arr->size * sizeof(TOKEN_DATA));
		if (newBuf == NULL)
			return ReturnFalse("Realloc token array failed");
		arr->token = newBuf;
		memset(arr->token + (arr->count), 0, (arr->size - arr->count) * sizeof(TOKEN_DATA));
	}
	return TRUE;
}
BOOL GetTokenEx(SOURCE_DATA* source, TOKEN_ARRAY* arr)
{
	while (TRUE)
	{
		if (ControlTokenArray(arr) == FALSE)
			return FALSE;
		if (GetToken(source, &arr->token[arr->count]) == FALSE)
			return FALSE;

		if (arr->token[arr->count].type == TOKEN_TYPE_ENDFILE)
			return TRUE;

		arr->count++;
	}
}
