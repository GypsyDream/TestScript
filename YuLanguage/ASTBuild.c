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
		return FALSE;

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
		return ReturnFalse("语法分析错误：语法树当前节点cur_index超出语法树范围");
	}
	return TRUE;
}

//堆栈操作函数：压栈、去除最后一个栈数据、出栈
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
		//最低优先级
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
	{
		//堆栈中莫得东西，返回最低的优先级不会干扰到其他优先级判断
		data->token_level =data->token_category = -1;
		return TRUE;
	}

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


//实际在语法树中，优先级越低，在语法树中越靠近根节点，运行时先计算优先级高的各叶子节点直接的操作符，最后运行优先级低的操作符
int express_level(AST_TREE* ast)
{
	//堆栈的作用
	//语法树规则：优先级高的先执行，优先级低的后执行，在语法树中，优先级高的操作符为优先级低的操作符的子节点
	//判断优先级时，若前一个优先级高于后一个，那么有两种情况，堆栈中若存在数据，后一个优先级与前一个优先级低的那个才是现在这个节点的parent
	//例如a = b + c * d + e;
	//当执行到b + c，b成为+的子节点，后发现后面的*优先级高于+
	//此时*应当为+的子节点，于是将+压栈，先生成*的语法树，后将*加为+的子节点即可，c成为*的子节点
	//但之后发现*后面的+，优先级比*低，这时*有两种情况，成为第一个+的子节点还是第二个+的子节点，于是与堆栈比较，两个+根据从左到右的顺序栈中的+优先级高，所以*应当成为栈中+的子节点，先执行
	//于是将d变为*的第二个子节点后，跳出了循环，返回了*的为位置
	//之后按照顺序，后一个节点为+，依然根据从左到右的原则，第一个+成为后一个+的子节点
	//函数中有的地方压入了-1，由于-1为最低的优先级，所以不管后面如何跟栈中优先级比较，也是-1优先级低(栈中优先级低)，永远不会由于后面指令优先级更低而跳出递归，例如括号内无论如何优先级比括号外高的情况

	NODE_DATA* nodes = ast->nodes;
	PROCESS_STATES state = STATES_START;
	unsigned int p = -1;//父节点
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
				//token类别为操作符，父节点为当前节点，进入输入ID模式
				p = ast->cur_index;
				state = STATES_INID;
			}
			else if (ISTOKENTYPE(ast, TOKEN_TYPE_LBRACKET))
			{
				//token类型为左括号，则优先将括号内语句生成为AST语法树
				if (ISTOKENTYPEX6(ast, ast->cur_index + 1, TOKEN_TYPE_ID, TOKEN_TYPE_NUM, TOKEN_TYPE_FLOAT, TOKEN_TYPE_STR, TOKEN_TYPE_LBRACKET, TOKEN_TYPE_REVERSE))
				{
					//左括号右边为提示符、数字、左括号、取反，则可正常递归生成括号内语法树
					//在递归之前，先将当前栈的栈顶记录下来，因为最后一个元素就是下一次要弹出的元素，即为堆栈顶部，\
					所以只需将stacklist的count保存下来即可。递归结束后，直接恢复stacklist的count值即可恢复递归前的堆栈环境。
					tmpcount = ast->stack.count;
					//记录当前节点
					tmpnode = ast->cur_index;
					//避免括号内进行优先级比较时收到括号外影响，压栈-1（-1优先级为最低）
					push_stack(&ast->stack, ast->nodes, -1);

					p = express_level(ast);//生成括号内语法树
					//此时cur指向的应该是右括号的位置

					ast->stack.count = tmpcount;  //恢复递归之前的堆栈空间。
					ast->cur_index++;  //递归结束后，curnode指向右括号之前的节点，所以将其加一，使其指向右括号用于下面的判断。

					if (!ISTOKENTYPE(ast, TOKEN_TYPE_RBRACKET))
					{
						Fault("syntax error start no right bracket (语法错误：只有左括号，没有右括号！)");
						return -1;
					}
					else if (ISTOKENTYPEX2(ast, tmpnode - 1, TOKEN_TYPE_ASSIGN, TOKEN_TYPE_LBRACKET))
					{
						//tmpnode为之前左括号位置，此处判断的为左括号左边的节点
						//左括号左边是赋值或还是左括号,意味着递归未结束，对上一个括号来说括号内没有全部生成结束
						//左括号左边为等于，例如a = b = (c + d) * e;则需要继续生成后面的*e最后作为第二个=的子节点
						//例如 a = ((a+b)*5) 在生成a+b的语法树后因为state设为了INID就会继续和*5生成语法树，否则就只会生成a+b而丢失乘法节点和5的节点。
						state = STATES_INID;
					}
					else if (ISTOKENTYPEX(ast, ast->cur_index + 1, TOKEN_TYPE_ASSIGN))
					{
						//括号右侧存在赋值符认为是错误语法
						Fault("syntax error ')' can't with '=' (语法错误：括号后面不能接赋值运算符！)");
						return -1;
					}
					else
					{
						//结束括号语法树生成
						state = STATES_DOWN;
					}
				}
				else
				{
					Fault("syntax error left bracket (语法错误：左括号右边的节点不是允许的节点！)");
					return -1;
				}
			}
			else if (ISTOKENTYPE(ast, TOKEN_TYPE_REVERSE))
			{
				//取反符,parent为取反符，进入INOP状态
				p = ast->cur_index;
				state = STATES_INOP;
			}
			else
			{
				Fault("syntax error start (语法错误：语法解析在start状态时遇到无效的节点！)");
				return -1;
			}
			break;
		case STATES_INID:
			//输入ID模式
			if (ISTOKCATEOP(ast))
			{
				//INID状态遇到表达式操作符如加减乘除，大于等于等，就将之前的父节点加入当前节点的子节点，并将当前节点设为新的父节点，并进入INOP状态。
				AddChildNode(ast, ast->cur_index, p);
				p = ast->cur_index;
				state = STATES_INOP;//states_in_operate
			}
			else if (ISTOKENTYPE2(ast, TOKEN_TYPE_SEMI, TOKEN_TYPE_RBRACKET) && 
				(ISTOKCATEX(ast, ast->cur_index - 1, TOKEN_CATEGORY_FACTOR)) || 
				(ISTOKENTYPEX(ast, ast->cur_index - 1, TOKEN_TYPE_RBRACKET)))//上一个为ID，这一个为分号或右括号，同时判断之前一个为操作因子或与右括号
			{
				//如果遇到分号或右括号，并且前一个节点是允许的节点比如标示符，数字之类的操作因子或右括号等，就表示当前express2函数解析完毕，可以返回。
				ast->cur_index--;
				state = STATES_DOWN;
			}
			else
			{
				Fault("syntax error inid (语法错误：在express2函数的inid状态遇到无效的节点！)");
				return -1;
			}
			break;
		case STATES_INOP://操作符输入模式
			if (ISTOKCATE(ast, TOKEN_CATEGORY_FACTOR) && (ISTOKCATEXOP(ast, ast->cur_index - 1)))//这一个token为操作因子，且确认上一个为操作符
			{
				if (ISTOKCATEXOP(ast, ast->cur_index + 1))
				{
					//此情况为当前节点为节点，前后均为操作符
					//下一个token也为操作符，则将上一个token优先级与下一个token优先级进行比较，目的为了判断这个因子作为哪一个操作符的子节点
					if (ast->nodes[p].level == ast->nodes[ast->cur_index + 1].level)
					{
						//前后优先级相同应遵循从左到右的原则，则左边的操作符优先级高，但有一种特殊情况，两个都是=则后边的优先级高
						//例如a = b = c = d;对c来说肯定是先把d的值付给c，再把c的值付给b
						if (ISTOKENTYPEX(ast, ast->cur_index+1, TOKEN_TYPE_ASSIGN))
						{
							//如果操作因子之后的是一个赋值运算符的话，因为表达式除了第一个赋值符优先级低外，其他的赋值符都有最高的优先级，所以递归express2先生成赋值符的表达式。
							tmpcount = ast->stack.count;
							push_stack(&ast->stack,ast->nodes, -1);
							//因为在express2递归时一开始就会进行addcurnode操作，所以这里减一，使其递归时从操作因子开始。
							ast->cur_index--;
							AddChildNode(ast, p, express_level(ast));
							//恢复栈状态
							ast->stack.count = tmpcount;
						}
						else
						{
							//上一个与下一个token优先级相同,当前token加为上一操作符子节点，上衣操作符作为下一操作符子节点
							//p优先级高，当前节点作为p的子节点，p作为下一个操作符的子节点
							AddChildNode(ast, p, ast->cur_index);
							AddChildNode(ast, ast->cur_index + 1, p);
							ast->cur_index++;
							p = ast->cur_index;
						}
					}
					else if (ast->nodes[p].level > ast->nodes[ast->cur_index + 1].level)
					{
						//上一个操作符优先级大于这一个操作符
						//与上面相同的特殊情况，不管前一个是啥，后一个若为等号也是等号优先级高
						if (ISTOKENTYPEX(ast, ast->cur_index + 1, TOKEN_TYPE_ASSIGN))
						{
							//如果操作因子之后的是一个赋值运算符的话，因为表达式除了第一个赋值符优先级低外，其他的赋值符都有最高的优先级，所以递归express2先生成赋值符的表达式。
							tmpcount = ast->stack.count;
							push_stack(&ast->stack, ast->nodes, -1);
							//因为在express2递归时一开始就会进行addcurnode操作，所以这里减一，使其递归时从操作因子开始。
							ast->cur_index--;
							AddChildNode(ast, p, express_level(ast));
							//恢复栈状态
							ast->stack.count = tmpcount;
						}
						else
						{
							//可以肯定的时当前token一定是高优先级（前一个操作符）的子节点，但前一个节点是否为后一个节点的子节点不能确定
							//这是因为堆栈中可能存在比后一个优先级更高的节点，那么上一个操作符就应该是堆栈中操作符的子节点
							AddChildNode(ast, p, ast->cur_index);
							//获取上一次堆栈信息
							STACK_DATA last;
							get_last_stack(&ast->stack, &last);
							if (last.token_category == ast->nodes[ast->cur_index + 1].category ||
								last.token_level > ast->nodes[ast->cur_index + 1].level)
							{
								//堆栈中优先级高，跳出后返回的p就是上一个操作符，他会成为堆栈中操作符的子节点
								//栈中类别与下一个操作符相同或栈中优先级高于下一个操作符则返回，优先处理栈中语法树生成
								state = STATES_DOWN;
							}
							else if (last.token_level < ast->nodes[ast->cur_index + 1].level)
							{
								//下一个操作符优先级高，那么上一个节点就应当变成下一个操作符的子节点
								//栈中优先级低，则继续当前操作
								AddChildNode(ast, ast->cur_index + 1, p);
								ast->cur_index++;
								p = ast->cur_index;
							}
							else
							{
								//堆栈中的优先级竟然比下一个操作符优先级还高
								//按道理堆栈中的高优先级操作符至少因该成为下一个操作符的子节点（也可能是上一个），出错
								Fault("syntax error inop 1 (语法错误：优先级判定失败！位置:express2函数 inop 1)");
								return -1;
							}
						}
					}
					else
					{
						//上一个操作符优先级小于下一个操作符，压栈当前操作符，优先生成后面高优先级操作符
						tmpcount = ast->stack.count;
						push_stack(&ast->stack, ast->nodes, p);
						ast->cur_index--;
						//优先级高AST语句添加为当前操作符子节点
						AddChildNode(ast, p, express_level(ast));
						ast->stack.count = tmpcount;
					}
				}
				else
				{
					//操作因子后不是操作符，直接将这个操作因子添加到之前操作符子结点中,有可能右括号什么的
					//暂且不管，后边发现问题再报错，当前节点直接加到父节点的子结点中
					AddChildNode(ast, p, ast->cur_index);
				}
			}
			else if (ISTOKCATEOP(ast) &&
				(ISTOKCATEX(ast, ast->cur_index - 1, TOKEN_CATEGORY_FACTOR)) || ISTOKENTYPEX(ast, ast->cur_index - 1, TOKEN_TYPE_RBRACKET))
			{
				//此节点为操作符，上一节点为因子或右括号,上一次加入的还是一个操作符
				//将父节点优先级与当前节点比较
				if (ast->nodes[p].level == ast->nodes[ast->cur_index].level)
				{
					//优先级相同，父节点为先执行，将父节点加入当前节点子节点中，父节点变为当前节点
					AddChildNode(ast, ast->cur_index, p);
				}
				else if (ast->nodes[p].level > ast->nodes[ast->cur_index].level)
				{
					//父节点高优先级，此时需要比较当前节点操作符与栈中操作符优先级
					//若栈中操作符优先级高
					//若当前节点优先级高于栈中优先级，则优先将父节点加为当前节点子节点
					STACK_DATA last;
					get_last_stack(&ast->stack, &last);
					if (last.token_category == ast->nodes[ast->cur_index].category ||
						last.token_level > ast->nodes[ast->cur_index].level)
					{
						//父节点优先级高于此节点，按正常应为将父节点加为此节点的子节点，
						//但堆栈中的优先级高于此节点，所以父节点子式生成完成，退出子节点
						//例如a = b + c * d + e;
						//此时*为父节点，堆栈中为+，当前节点为+
						//b*c这一小块就返回堆栈中+的子节点
						ast->cur_index--;
						state = STATES_DOWN;
					}
					else if(last.token_level < ast->nodes[ast->cur_index].level)
					{
						AddChildNode(ast, ast->cur_index, p);
						p = ast->cur_index;
					}
					else
					{
						//堆栈中优先级与现状相同但类型不同（不知道啥情况）
						Fault("syntax error inop 2 (语法错误：优先级判定失败！位置:express2函数 inop 2)");
						return -1;
					}
				}
				else
				{
					//堆栈中优先级与现状相同但类型不同（不知道啥情况）
					Fault("syntax error inop 3 (语法错误：优先级判定失败！位置:express2函数 inop 3)");
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
					p = express_level(ast);
					ast->stack.count = tmpcount;
					ast->cur_index++;
					if (ISTOKENTYPE(ast, TOKEN_TYPE_RBRACKET) == FALSE)
					{
						Fault("syntax error start no right bracket (语法错误：没有右括号！)");
						return -1;
					}
					if (ISTOKCATEXOP(ast, ast->cur_index + 1))
					{
						//括号右边是运算符，需要将括号前的运算符和括号后的运算符进行优先级比较。
						if (ISTOKENTYPEX(ast, ast->cur_index + 1, TOKEN_TYPE_ASSIGN))
						{
							Fault("syntax error ')' can't with '=' (语法错误：括号后不能接赋值符！)");
							return -1;
						}
						if (ast->nodes[tmpnode - 1].level == ast->nodes[ast->cur_index + 1].level)
						{
							//括号前的运算符优先级等于括号后的运算符优先级。
							AddChildNode(ast, tmpnode - 1, p);
							p = tmpnode - 1;
							AddChildNode(ast, ++(ast->cur_index), p);
							p = ast->cur_index;
						}
						else if (ast->nodes[tmpnode - 1].level > ast->nodes[ast->cur_index + 1].level)
						{
							AddChildNode(ast, tmpnode - 1, p);
							p = tmpnode - 1;
							STACK_DATA data;
							get_last_stack(&ast->stack, &data);
							if (data.token_category == ast->nodes[ast->cur_index + 1].category ||
								data.token_level > ast->nodes[ast->cur_index + 1].level)
							{
								state = STATES_DOWN;
							}
							else if (data.token_level < ast->nodes[ast->cur_index + 1].level)
							{
								AddChildNode(ast, ++(ast->cur_index), p);
								p = ast->cur_index;
							}
							else
							{
								Fault("syntax error inop 4 (语法错误：优先级判定失败！位置:express2函数 inop 4)");
								return -1;
							}
						}
						else
						{
							//括号前的运算符优先级小于括号后的运算符优先级。
							tmpcount = ast->stack.count;
							push_stack(&ast->stack, ast->nodes, tmpnode - 1);
							AddChildNode(ast, tmpnode - 1, express_level(ast));
							p = tmpnode - 1;
							ast->stack.count = tmpcount;
						}
					}
					else
					{
						//括号右边不是运算符。直接将括号表达式的AST加入之前运算符的子节点。
						AddChildNode(ast, tmpnode - 1, p);
						p = tmpnode - 1;
					}
				}
				else
				{
					Fault("syntax error left bracket (语法错误：左括号右边不是合法的节点！)");
					return -1;
				}
			}
			else if (ISTOKENTYPE(ast, TOKEN_TYPE_REVERSE))
			{
				//操作运算符遇到取反运算符时，因为取反运算符优先级高，所以先生成取反运算符的AST,再将生成的AST作为之前操作运算符的子节点。
				tmpcount = ast->stack.count;
				push_stack(&ast->stack, ast->nodes, p);
				ast->cur_index--;
				AddChildNode(ast, p, express_level(ast));
				ast->stack.count = tmpcount;
			}
			else if (ISTOKENTYPE2(ast, TOKEN_TYPE_SEMI, TOKEN_TYPE_RBRACKET) &&
			(ISTOKCATEX(ast, ast->cur_index - 1, TOKEN_CATEGORY_FACTOR)) || ISTOKENTYPEX(ast, ast->cur_index - 1, TOKEN_TYPE_RBRACKET))
			{
				//遇到分号或右括号时，表达式结束，直接返回
				ast->cur_index--;
				state = STATES_DOWN;
			}
			else
			{
				Fault("syntax error inop (语法错误，INOP状态时遇到无效的节点！)");
				return -1;
			}
			break;
			default:
				Fault("syntax error (语法错误，express2遇到无效的状态机！)");
				return -1;
			break;
		}
	}
	return p;
}

BOOL print_stnt(int p, AST_TREE* ast)
{
	addcurnode(ast);
	if (ISTOKCATE(ast, TOKEN_CATEGORY_FACTOR) || ISTOKENTYPE2(ast, TOKEN_TYPE_LBRACKET, TOKEN_TYPE_REVERSE))
	{

	}
}

BOOL statement(AST_TREE* ast, int* parent)
{
	NODE_DATA* nodes = ast->nodes;

	if (addcurnode(ast) == FALSE)
		return FALSE;
	int p = -1;
	if (ISTOKENTYPE(ast, TOKEN_TYPE_RESERVE))
	{
		switch (ast->nodes[ast->cur_index].reserve_type)
		{
		case RESERVE_TYPE_PRINT:
			ast->p_index = ast->cur_index;

		}
	}
	else
	{
		ast->cur_index--;
		p = express_level(ast);
	}
	ast->cur_index++;
	if (p == -1)
		return FALSE;
	else
		*parent = p;
	return TRUE;
}

BOOL BuildAstTree(AST_TREE* ast)
{
	if (ast == NULL)
		return ReturnFalse("ast point is NULL");
	if(ast->is_builded==TRUE)
		return ReturnFalse("ast is builded");

//	InitAstTree(ast);

	int p = -1;
	while (ast->cur_index < ast->count - 1)
	{
		if (ast->cur_index == -1)//每一个语句的开始
		{
			if (statement(ast, &p))
				ast->rootnode = p;
			else
				return FALSE;
		}
		else
		{
			int m_p;
			if (statement(ast, &m_p))
			{
				ast->nodes[p].nextnode = m_p;
				p = m_p;
			}
			else
				return FALSE;
		}
	}
	return TRUE;
}