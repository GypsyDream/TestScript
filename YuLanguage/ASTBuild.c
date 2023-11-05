#include "ASTBuild.h"

BOOL InitAstTree(AST_TREE* ast)
{
	if (ast == NULL)
		return ReturnFalse("ast point is NULL");
	memset(ast, 0, sizeof(AST_TREE));

	ast->cur_index = -1;
	ast->p_index = -1;

	return TRUE;
}

BOOL AddNode(TOKEN_DATA* token, AST_TREE* ast)
{
	if (token == NULL)
		return FALSE;
	if (token->type == TOKEN_TYPE_ERROR || token->type == TOKEN_TYPE_DEFAULT || token->type == TOKEN_TYPE_ENDFILE)
		return;

	if (ast->nodes == NULL)
	{
		ast->size = Default_Node_Size;
		ast->nodes = malloc(ast->size * sizeof(NODE_DATA));
		if (ast->nodes == NULL)
			return ReturnFalse("malloc ast tree failed");
		memset(ast->nodes, 0, ast->size * sizeof(NODE_DATA));
		ast->rootnode = -1;
	}
	else if (ast->count >= ast->size)
	{
		ast->size += Default_Node_Size;
		NODE_DATA* newBuf = realloc(ast->nodes, ast->size * sizeof(NODE_DATA));
		if (newBuf == NULL)
			return ReturnFalse("realloc ast tree failed");
		ast->nodes = newBuf;
		memset(newBuf + (ast->count), 0, (ast->size - ast->count) * sizeof(NODE_DATA));
	}

	ast->nodes[ast->count].isvalid = TRUE;
	ast->nodes[ast->count].type = token->type;
	ast->nodes[ast->count].reserve_type = token->reserve;
	ast->nodes[ast->count].strtoken = token->str;
	ast->nodes[ast->count].line_no = token->curline;
	ast->nodes[ast->count].colum_no = token->curcolum;
	
	switch (ast->nodes[ast->count].type)
	{
	case TOKEN_TYPE_GREAT:
	case TOKEN_TYPE_LESS:
	case TOKEN_TYPE_EQUAL:
	case TOKEN_TYPE_NOT_EQ:
	case TOKEN_TYPE_GREAT_EQ:
	case TOKEN_TYPE_LESS_EQ:
		ast->nodes[ast->count].category = TOKEN_CATEGORY_RELATION;
		ast->nodes[ast->count].level = TOKEN_LEVEL_RELATION;
		break;
	case TOKEN_TYPE_AND:
	case TOKEN_TYPE_OR:
		ast->nodes[ast->count].category = TOKEN_CATEGORY_LOGIC;
		ast->nodes[ast->count].level = TOKEN_LEVEL_LOGIC;
		break;
	case TOKEN_TYPE_REVERSE:
		ast->nodes[ast->count].category = TOKEN_CATEGORY_LOGIC;
		ast->nodes[ast->count].level = TOKEN_LEVEL_REVERSE;
		break;
	case TOKEN_TYPE_PLUS:
	case TOKEN_TYPE_MINIS:
		ast->nodes[ast->count].category = TOKEN_CATEGORY_PLUS_MINIS;
		ast->nodes[ast->count].level = TOKEN_LEVEL_PLUS_MINIS;
		break;
	case TOKEN_TYPE_TIMES:
	case TOKEN_TYPE_DIVIDE:
		ast->nodes[ast->count].category = TOKEN_CATEGORY_TIM_DIV;
		ast->nodes[ast->count].level = TOKEN_LEVEL_TIM_DIV;
		break;
	case TOKEN_TYPE_ASSIGN:
		ast->nodes[ast->count].category = TOKEN_CATEGORY_ASSIGN;
		ast->nodes[ast->count].level = TOKEN_LEVEL_ASSIGN;
		break;
	case TOKEN_TYPE_ID:
	case TOKEN_TYPE_NUM:
	case TOKEN_TYPE_FLOAT:
	case TOKEN_TYPE_STR:
		ast->nodes[ast->count].category = TOKEN_CATEGORY_FACTOR;
		ast->nodes[ast->count].level = TOKEN_LEVEL_START;
		break;
	default:
		ast->nodes[ast->count].category = TOKEN_CATEGORY_START;
		ast->nodes[ast->count].level = TOKEN_LEVEL_START;
		break;
	}

	ast->count++;

	return TRUE;
}

BOOL addcurnode(AST_TREE* ast)
{
	ast->cur_index++;
	if (ast->cur_index > ast->count - 1)
	{
		ast->cur_index--;
		return ReturnFalse("�﷨���������﷨����ǰ�ڵ�cur_index�����﷨����Χ");
	}
	return TRUE;
}

//��ջ����������ѹջ��ȥ�����һ��ջ���ݡ���ջ
BOOL push_stack(STACKLIST_DATA* stack, NODE_DATA* node, int nodenum)
{
	if (stack == NULL)
		return ReturnFalse("stack point is null, in push stack");
	
	if (stack->stacks == NULL)
	{
		stack->size = Default_Stack_Size;
		stack->stacks = malloc(sizeof(STACK_DATA) * stack->size);
		if (stack->stacks == NULL)
			return ReturnFalse("malloc stack list failed");
		memset(stack->stacks, 0, sizeof(STACK_DATA) * stack->size);
	}
	else if (stack->count >= stack->size)
	{
		stack->size += Default_Stack_Size;
		STACK_DATA* newBuf = realloc(stack->stacks, sizeof(STACK_DATA) * stack->size);
		if (newBuf == NULL)
			return ReturnFalse("realloc stack list failed");
		stack->stacks = newBuf;
		memset(stack->stacks + stack->count, 0, (stack->size - stack->count) * sizeof(STACK_DATA));
	}

	if (nodenum < 0)
	{
		//������ȼ�
		stack->stacks[stack->count].token_category = TOKEN_CATEGORY_START;
		stack->stacks[stack->count].token_level = TOKEN_LEVEL_START;
	}
	else
	{
		if (node == NULL)
			return ReturnFalse("node point is null,  in push stack");
		stack->stacks[stack->count].token_category = node[nodenum].category;
		stack->stacks[stack->count].token_level = node[nodenum].level;
	}
	
	stack->count++;

	return TRUE;
}
BOOL get_last_stack(STACKLIST_DATA* stack, STACK_DATA* data)
{
	if (stack == NULL)
		return ReturnFalse("stack is null in get last stack");
	if (stack->count <= 0)
		return ReturnFalse("stack list null");

	memcpy(data, &stack->stacks[stack->count - 1], sizeof(STACK_DATA));
	return TRUE;
}
BOOL pop_stack(STACKLIST_DATA* stack)
{
	if (stack == NULL)
		return ReturnFalse("stack is null");
	if (stack->count <= 0)
		return ReturnFalse("stack list null");

	memset(&stack->stacks[stack->count - 1], 0, sizeof(STACK_DATA));

	stack->count--;
	return TRUE;
}

BOOL AddChildNode(AST_TREE* ast, unsigned int parent, unsigned int child)
{
	NODE_CHILD* childs = &ast->nodes[parent].childs;
	if (childs->size == 0)
	{
		childs->size = Default_Child_Size;
		childs->childnum = malloc(childs->size * sizeof(unsigned int));
		if (childs->childnum == NULL)
			return ReturnFalse("malloc childs failed");
		memset(childs->childnum, 0, childs->size * sizeof(unsigned int));
	}
	else if (childs->count >= childs->size)
	{
		childs->size += Default_Child_Size;
		unsigned int* childnum = realloc(childs->childnum, childs->size * sizeof(unsigned int));
		if (childnum == NULL)
			return ReturnFalse("realloc childs failed");
		childs->childnum = childnum;
		memset(childs->childnum + childs->count, 0, (childs->size - childs->count) * sizeof(unsigned int));
	}

	childs->childnum[childs->count++] = child;

	return TRUE;
}
BOOL RemoveLastChild(NODE_DATA* node, unsigned int* tmp)
{
	if (node->childs.count <= 0)
		return ReturnFalse("child count less than zero");

	tmp = node->childs.childnum[node->childs.count--];

	return TRUE;
}


//ʵ�����﷨���У����ȼ�Խ�ͣ����﷨����Խ�������ڵ㣬����ʱ�ȼ������ȼ��ߵĸ�Ҷ�ӽڵ�ֱ�ӵĲ�����������������ȼ��͵Ĳ�����
int express_level(AST_TREE* ast)
{
	NODE_DATA* nodes = ast->nodes;
	PROCESS_STATES state = STATES_START;
	unsigned int p = -1;//���ڵ�
	int tmpcount = 0;
	int tmpnode = -1;

	while (state != STATES_DOWN)
	{
		addcurnode(ast);
		switch (state)
		{
		case STATES_START:
			if (ISTOKCATE(ast, TOKEN_CATEGORY_FACTOR))
			{
				//token���Ϊ�����������ڵ�Ϊ��ǰ�ڵ㣬��������IDģʽ
				p = ast->cur_index;
				state = STATES_INID;
			}
			else if (ISTOKENTYPE(ast, TOKEN_TYPE_LBRACKET))
			{
				//token����Ϊ�����ţ������Ƚ��������������ΪAST�﷨��
				if (ISTOKENTYPEX6(ast, ast->cur_index + 1, TOKEN_TYPE_ID, TOKEN_TYPE_NUM, TOKEN_TYPE_FLOAT, TOKEN_TYPE_STR, TOKEN_TYPE_LBRACKET, TOKEN_TYPE_REVERSE))
				{
					//�������ұ�Ϊ��ʾ�������֡������š�ȡ������������ݹ������������﷨��
					//�ڵݹ�֮ǰ���Ƚ���ǰջ��ջ����¼��������Ϊ���һ��Ԫ�ؾ�����һ��Ҫ������Ԫ�أ���Ϊ��ջ������\
					����ֻ�轫stacklist��count�����������ɡ��ݹ������ֱ�ӻָ�stacklist��countֵ���ɻָ��ݹ�ǰ�Ķ�ջ������
					tmpcount = ast->stack.count;
					//��¼��ǰ�ڵ�
					tmpnode = ast->cur_index;
					//���������ڽ������ȼ��Ƚ�ʱ�յ�������Ӱ�죬ѹջ-1��-1���ȼ�Ϊ��ͣ�
					push_stack(&ast->stack, ast->nodes, -1);

					p = express_level(ast);//�����������﷨��

					ast->stack.count = tmpcount;  //�ָ��ݹ�֮ǰ�Ķ�ջ�ռ䡣
					ast->cur_index++;  //�ݹ������curnodeָ��������֮ǰ�Ľڵ㣬���Խ����һ��ʹ��ָ������������������жϡ�

					if (!ISTOKENTYPE(ast, TOKEN_TYPE_RBRACKET))
					{
						Fault("syntax error start no right bracket (�﷨����ֻ�������ţ�û�������ţ�)");
						return -1;
					}
					else if (ISTOKENTYPEX2(ast, tmpnode - 1, TOKEN_TYPE_ASSIGN, TOKEN_TYPE_LBRACKET))
					{
						//����������Ǹ�ֵ���������ţ���������ɱ����﷨����
						//���� a = ((a+b)*5) ������a+b���﷨������Ϊstate��Ϊ��INID�ͻ������*5�����﷨���������ֻ������a+b����ʧ�˷��ڵ��5�Ľڵ㡣
						state = STATES_INID;
					}
					else if (ISTOKENTYPEX(ast, ast->cur_index + 1, TOKEN_TYPE_ASSIGN))
					{
						//�����Ҳ���ڸ�ֵ����Ϊ�Ǵ����﷨
						Fault("syntax error ')' can't with '=' (�﷨�������ź��治�ܽӸ�ֵ�������)");
						return -1;
					}
					else
					{
						//���������﷨������
						state = STATES_DOWN;
					}
				}
				else
				{
					Fault("syntax error left bracket (�﷨�����������ұߵĽڵ㲻������Ľڵ㣡)");
					return -1;
				}
			}
			else if (ISTOKENTYPE(ast, TOKEN_TYPE_REVERSE))
			{
				//ȡ����,parentΪȡ����������INOP״̬
				p = ast->cur_index;
				state = STATES_INOP;
			}
			else
			{
				Fault("syntax error start (�﷨�����﷨������start״̬ʱ������Ч�Ľڵ㣡)");
				return -1;
			}
			break;
		case STATES_INID:
			//����IDģʽ
			if (ISTOKCATEOP(ast))//��һ��ΪID����һ��Ϊ�����������
			{
				//INID״̬�������ʽ��������Ӽ��˳������ڵ��ڵȣ��ͽ�֮ǰ�ĸ��ڵ���뵱ǰ�ڵ���ӽڵ㣬������ǰ�ڵ���Ϊ�µĸ��ڵ㣬������INOP״̬��
				AddChildNode(ast, ast->cur_index, p);
				p = ast->cur_index;
				state = STATES_INOP;//states_in_operate
			}
			else if (ISTOKENTYPE2(ast, TOKEN_TYPE_SEMI, TOKEN_TYPE_RBRACKET) && 
				(ISTOKCATEX(ast, ast->cur_index - 1, TOKEN_CATEGORY_FACTOR)) || 
				(ISTOKENTYPEX(ast, ast->cur_index - 1, TOKEN_TYPE_RBRACKET)))//��һ��ΪID����һ��Ϊ�ֺŻ������ţ�ͬʱ�ж�֮ǰһ��Ϊ�������ӻ���������
			{
				//��������ֺŻ������ţ�����ǰһ���ڵ�������Ľڵ�����ʾ��������֮��Ĳ������ӻ������ŵȣ��ͱ�ʾ��ǰexpress2����������ϣ����Է��ء�
				ast->cur_index--;
				state = STATES_DOWN;
			}
			else
			{
				Fault("syntax error inid (�﷨������express2������inid״̬������Ч�Ľڵ㣡)");
				return -1;
			}
			break;
		case STATES_INOP://��һ��Ϊ������
			if (ISTOKCATE(ast, TOKEN_CATEGORY_FACTOR) && (ISTOKCATEOP(ast)))//��һ��tokenΪ�������ӣ���ȷ����һ��Ϊ������
			{
				if (ISTOKCATEXOP(ast, ast->cur_index + 1))
				{
					//��һ��tokenҲΪ������������һ��token���ȼ�����һ��token���ȼ����бȽ�
					if (ast->nodes[ast->cur_index - 1].level == ast->nodes[ast->cur_index + 1].level)
					{
						if (ISTOKENTYPEX(ast, ast->cur_index+1, TOKEN_TYPE_ASSIGN))
						{
							//�����������֮�����һ����ֵ������Ļ�����Ϊ���ʽ���˵�һ����ֵ�����ȼ����⣬�����ĸ�ֵ��������ߵ����ȼ������Եݹ�express2�����ɸ�ֵ���ı��ʽ��
							tmpcount = ast->stack.count;
							push_stack(&ast->stack,ast->nodes, -1);
							//��Ϊ��express2�ݹ�ʱһ��ʼ�ͻ����addcurnode���������������һ��ʹ��ݹ�ʱ�Ӳ������ӿ�ʼ��
							ast->cur_index--;
							AddChildNode(ast, p, express_level(ast));
							//�ָ�ջ״̬
							ast->stack.count = tmpcount;
						}
						else
						{
							//��һ������һ��token���ȼ���ͬ,��ǰtoken��Ϊ��һ�������ӽڵ㣬���²�������Ϊ��һ�������ӽڵ�
							AddChildNode(ast, p, ast->cur_index);
							AddChildNode(ast, ast->cur_index, p);
							ast->cur_index++;
							p = ast->cur_index;
						}
					}
					else if (ast->nodes[ast->cur_index - 1].level > ast->nodes[ast->cur_index + 1].level)
					{
						//��һ�����������ȼ�������һ��������
						if (ISTOKENTYPEX(ast, ast->cur_index + 1, TOKEN_TYPE_ASSIGN))
						{
							//�����������֮�����һ����ֵ������Ļ�����Ϊ���ʽ���˵�һ����ֵ�����ȼ����⣬�����ĸ�ֵ��������ߵ����ȼ������Եݹ�express2�����ɸ�ֵ���ı��ʽ��
							tmpcount = ast->stack.count;
							push_stack(&ast->stack, ast->nodes, -1);
							//��Ϊ��express2�ݹ�ʱһ��ʼ�ͻ����addcurnode���������������һ��ʹ��ݹ�ʱ�Ӳ������ӿ�ʼ��
							ast->cur_index--;
							AddChildNode(ast, p, express_level(ast));
							//�ָ�ջ״̬
							ast->stack.count = tmpcount;
						}
						else
						{
							//��ǰ�ڵ����֮ǰ���ڵ���
							AddChildNode(ast, p, ast->cur_index);
							//��ȡ��һ�ζ�ջ��Ϣ
							STACK_DATA last;
							get_last_stack(&ast->stack, &last);
							if (last.token_category == ast->nodes[ast->cur_index + 1].category ||
								last.token_level > ast->nodes[ast->cur_index + 1].level)
							{
								//ջ���������һ����������ͬ��ջ�����ȼ�������һ���������򷵻أ����ȴ���ջ���﷨������
								state = STATES_DOWN;
							}
							else if (last.token_level < ast->nodes[ast->cur_index + 1].level)
							{
								//ջ�����ȼ��ͣ��������ǰ����
								AddChildNode(ast, ast->cur_index, p);
								ast->cur_index++;
								p = ast->cur_index;
							}
							else
							{
								Fault("syntax error inop 1 (�﷨�������ȼ��ж�ʧ�ܣ�λ��:express2���� inop 1)");
								return -1;
							}
						}
					}
					else
					{
						//��һ�����������ȼ�С����һ����������ѹջ��ǰ���������������ɺ�������ȼ�������
						tmpcount = ast->stack.count;
						push_stack(&ast->stack, ast->nodes, p);
						ast->cur_index--;
						//���ȼ���AST������Ϊ��ǰ�������ӽڵ�
						AddChildNode(ast, p, express_level(ast));
						ast->stack.count = tmpcount;
					}
				}
				else
				{
					//�������Ӻ��ǲ�������ֱ�ӽ��������������ӵ�֮ǰ�������ӽ����
					AddChildNode(ast, p, ast->cur_index);
				}
			}
			else if (ISTOKCATEOP(ast) &&
				(ISTOKCATEX(ast, ast->cur_index - 1, TOKEN_CATEGORY_FACTOR)) || ISTOKENTYPEX(ast, ast->cur_index - 1, TOKEN_TYPE_RBRACKET))
			{
				//�˽ڵ�Ϊ����������һ�ڵ�Ϊ���ӻ�������,��һ�μ���Ļ���һ��������
				//�����ڵ����ȼ��뵱ǰ�ڵ�Ƚ�
				if (ast->nodes[p].level == ast->nodes[ast->cur_index].level)
				{
					//���ȼ���ͬ�����ڵ�Ϊ��ִ�У������ڵ���뵱ǰ�ڵ��ӽڵ��У����ڵ��Ϊ��ǰ�ڵ�
					AddChildNode(ast, ast->cur_index, p);
				}
				else if (ast->nodes[p].level > ast->nodes[ast->cur_index].level)
				{
					//���ڵ�����ȼ�����ʱ��Ҫ�Ƚϵ�ǰ�ڵ��������ջ�в��������ȼ�
					//��ջ�в��������ȼ���
					//����ǰ�ڵ����ȼ�����ջ�����ȼ��������Ƚ����ڵ��Ϊ��ǰ�ڵ��ӽڵ�
					STACK_DATA last;
					get_last_stack(&ast->stack, &last);
					if (last.token_category == ast->nodes[ast->cur_index].category ||
						last.token_level > ast->nodes[ast->cur_index].level)
					{
						//���ڵ����ȼ����ڴ˽ڵ㣬������ӦΪ�����ڵ��Ϊ�˽ڵ���ӽڵ㣬
						//����ջ�е����ȼ����ڴ˽ڵ㣬���Ը��ڵ���ʽ������ɣ��˳��ӽڵ�
						//����a = b + c * d + e;
						//��ʱ*Ϊ���ڵ㣬��ջ��Ϊ+����ǰ�ڵ�Ϊ+
						//b*c��һС��ͷ��ض�ջ��+���ӽڵ�
						ast->cur_index--;
						state = STATES_DOWN;
					}
					else if(last.token_level < ast->nodes[ast->cur_index].level)
					{
						AddNodeChild(ast->cur_index, p);
						p = ast->cur_index;
					}
					else
					{
						//��ջ�����ȼ�����״��ͬ�����Ͳ�ͬ����֪��ɶ�����
						Fault("syntax error inop 2 (�﷨�������ȼ��ж�ʧ�ܣ�λ��:express2���� inop 2)");
						return -1;
					}
				}
				else
				{
					//��ջ�����ȼ�����״��ͬ�����Ͳ�ͬ����֪��ɶ�����
					Fault("syntax error inop 3 (�﷨�������ȼ��ж�ʧ�ܣ�λ��:express2���� inop 3)");
					return -1;
				}
			}
			else if (ISTOKENTYPE(ast, TOKEN_TYPE_LBRACKET))
			{
				if (ISTOKENTYPEX6(ast, ast->cur_index + 1, TOKEN_TYPE_ID, TOKEN_TYPE_NUM, TOKEN_TYPE_FLOAT, TOKEN_TYPE_STR, TOKEN_TYPE_LBRACKET, TOKEN_TYPE_REVERSE))
				{
					tmpcount = ast->stack.count;
					tmpnode = ast->cur_index;
					push_stack(&ast->stack, ast->nodes, -1);
					p = express2();
					ast->stack.count = tmpcount;
					ast->cur_index++;
					if (ISTOKENTYPE(ast, TOKEN_TYPE_RBRACKET) == FALSE)
					{
						Fault("syntax error start no right bracket (�﷨����û�������ţ�)");
						return -1;
					}
				}
			}
			break;
		}
	}
}

BOOL print_stnt(int p, AST_TREE* ast)
{
	addcurnode(ast);
	if (ISTOKCATE(ast, TOKEN_CATEGORY_FACTOR) || ISTOKENTYPE2(ast, TOKEN_TYPE_LBRACKET, TOKEN_TYPE_REVERSE))
	{

	}
}

BOOL statement(AST_TREE* ast)
{
	NODE_DATA* nodes = ast->nodes;

	if (addcurnode(ast) == FALSE)
		return FALSE;

	if (ISTOKENTYPE(ast, TOKEN_TYPE_RESERVE))
	{
		switch (ast->nodes[ast->cur_index].reserve_type)
		{
		case RESERVE_TYPE_PRINT:
			ast->p_index = ast->cur_index;

		}
	}
}

BOOL BuildAstTree(AST_TREE* ast)
{
	if (ast == NULL)
		return ReturnFalse("ast point is NULL");
	if(ast->is_builded==TRUE)
		return ReturnFalse("ast is builded");

	InitAstTree(ast);

	while (ast->cur_index < ast->count - 1)
	{
		if (ast->cur_index == -1)//ÿһ�����Ŀ�ʼ
		{

		}
		else
		{

		}
	}

}