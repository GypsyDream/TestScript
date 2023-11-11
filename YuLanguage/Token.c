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
			outData->reserve = i;  //���ùؼ��ʵ�ö��ֵ
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
			//��ʼ�׶μ�¼token��ʼ����
			outData->curline = source->curline;
			outData->curcolum = source->curcolum;
			if (ch == ' ' || ch == '\t' || ch == '\n') //����ǻ��лس�֮��ľ�������
			{
				if (ch == '\n')//�����ļ���ȡ��ַ��¼
				{
					source->curline++;
					source->curcolum = 0;
				}
				continue;
			}
			else if (isalpha(ch)) //�ж϶�ȡ���ַ��Ƿ���Ӣ����ĸ��
			{
				state = STATES_INID; //�������ĸ�����Ǿͽ�state״̬������ΪINID ��
				makeTokenStr(ch, outData); //Ȼ�󽫶�ȡ������ch�ַ�ͨ������makeTokenStr���뵽TokenString��̬�ַ�����
			}
			else if (isdigit(ch)) //�ж϶�ȡ���ַ��Ƿ�������
			{
				state = STATES_INNUM; //��������֣����Ǿͽ�state״̬������ΪINNUM ��
				makeTokenStr(ch, outData); //Ȼ�󽫶�ȡ������ch�ַ�ͨ������makeTokenStr���뵽TokenString��̬�ַ�����
			}
			else if (ch == '\'' || ch == '"') //�ж��Ƿ����ַ���
			{
				state = STATES_INSTR;
				startStr_ch = ch;
//				outData->str[0] = STRNULL;//δ�����ڴ棬Ұָ��
			}
			else
			{
				switch (ch)
				{
				case '+':
					state = STATES_DOWN; //����ַ��ǡ�+���žͽ�state״̬����ΪDOWN,�����Ϳ��Խ���ѭ��������token��ΪPLUSö��ֵ����ʾ�ҵ��Ӻ��������
					outData->type = TOKEN_TYPE_PLUS;
					break;
				case '-':
					state = STATES_DOWN; //������ͬ��
					outData->type = TOKEN_TYPE_MINIS;
					break;
				case '*':
					state = STATES_DOWN; //������ͬ��
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
					state = STATES_INGREAT;  //�����ַ�����INGREAT״̬
					break;
				case '<':
					state = STATES_INLESS;  //С���ַ�����INLESS״̬
					break;
				case '!':
					state = STATES_INNOT_EQ;  //�����ڻ�ȡ�����Ž���INNOT_EQ״̬
					break;
				case '&':
					state = STATES_INAND;  //�߼���
					break;
				case '|':
					state = STATES_INOR;  //�߼���
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
					state = STATES_DOWN; //EOF�ַ���ʾ��ȡ�����ļ��Ľ�β���򷵻�ENDFILE��token�������main��������ѭ���ͻ����ɨ�衣
					outData->type = TOKEN_TYPE_ENDFILE;
					break;
				case ';':
					state = STATES_DOWN; //�ֺű�ʾ����������
					outData->type = TOKEN_TYPE_SEMI;
					break;
				default:
					state = STATES_DOWN; //��������±�ʾ��ȡ����δ�����token����ô�ͷ���ERROR��
					outData->type = TOKEN_TYPE_ERROR;
					break;
				}//switch(ch)
				makeTokenStr(ch, outData);
			}
			break;
		case STATES_INID:
			if (isalnum(ch) || ch == '_') //��INID״̬�£�һֱ��ȡ�ַ���ֱ�����ַ�������ĸΪֹ��������ȡ����ĸͨ��makeTokenStr���������ı�ʾ������ʾ�����԰����»��ߣ�
				makeTokenStr(ch, outData);
			else
			{
				state = STATES_DOWN;
				outData->type = TOKEN_TYPE_ID; //��token��ΪID��ʾ��ȡ����һ����ʾ����ID��identifier Ӣ����д��
				ungetchar(source); //��Ϊ���ȡ��һ������ĸ���ַ���������ungetchar����������һ���ַ�������һ��ɨ��ʹ�á�
			}
			break;
		case STATES_INNUM:
			if (isdigit(ch))
				makeTokenStr(ch, outData);
			else if (ch == '.')  //�����С���㣬��˵���Ǹ�����������INFLOAT״̬
			{
				makeTokenStr(ch, outData);
				state = STATES_INFLOAT;
			}
			else
			{
				state = STATES_DOWN;
				outData->type = TOKEN_TYPE_NUM; //��token��ΪNUM��ʾ��ȡ����һ�����֡�
				ungetchar(source);
			}
			break;
		case STATES_INFLOAT:  //��INFLOAT״̬�£�һֱ��ȡ�ַ���ֱ�����ַ���������Ϊֹ�������Ϳ��Եõ�һ����������
			//����ǰ���ȡ��С�����ˣ��ʴ˴�����������
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
			if (ch != startStr_ch) //����ٴ�����startStr_ch���ַ�����ʼ�ĵ����Ż�˫���ţ��ͱ�ʾһ���ַ�������
				makeTokenStr(ch, outData); //��˫���Ż������ڵ��ַ����뵽TokenString�С�
			else
			{
				state = STATES_DOWN;
				outData->type = TOKEN_TYPE_STR; //��token��ΪSTR��ʾ��ȡ����һ���ַ���
			}
			break;
		case STATES_INASSIGN:  //�����'=='����EQUAL��������������ֻ��һ��'='����ASSIGN��ֵ�����
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
		case STATES_INGREAT:  //�����'>='��token����GREAT_EQ���ڵ�����������������GREAT���������
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
		case STATES_INLESS:  //������滹���˵Ⱥż�'<='����ôtoken����LESS_EQС�ڵ�����������������LESSС�������
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
		case STATES_INNOT_EQ:  //!�������'='��'!='���ʾtoken��NOT_EQ��������������������'!'��REVERSEȡ�������
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
		case STATES_INAND:  //����&��һ��'&&'��ʾtoken��AND�߼����������
			if (ch == '&')
			{
				makeTokenStr(ch, outData);
				state = STATES_DOWN;
				outData->type = TOKEN_TYPE_AND;
			}
			else
				return ReturnFalse("\n word parse error, '&' must with '&' now: %d:%d (�ʷ�ɨ��ʱ��������&���ź�������ٽ�һ��&���ܹ����߼��ң���������Ч����)",
					source->curline, source->curcolum);
			break;
		case STATES_INOR:  //����|��һ��'||'��ʾtoken���߼��������
			if (ch == '|')
			{
				makeTokenStr(ch, outData);
				state = STATES_DOWN;
				outData->type = TOKEN_TYPE_OR;
			}
			else
				return ReturnFalse("\n word parse error, '|' must with '|' now: %d:%d (�ʷ�ɨ��ʱ��������'|'���ź�������ٽ�һ��'|'���ܹ����߼��򣬷�������Ч����)",
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
