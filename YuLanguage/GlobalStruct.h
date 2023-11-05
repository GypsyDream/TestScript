#ifndef _GLOBAL_STRUCT_H
#define _GLOBAL_STRUCT_H
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdarg.h>

extern const char* reserves[];

#define STRNULL '\0'
#define BUF_SIZE 1024

typedef unsigned char BOOL;
#define TRUE 1
#define FALSE 0

//判断ast中当前token类型是否与输入相同
#define ISTOKENTYPE(ast, toketype)\
(ast->nodes[ast->cur_index].isvalid == TRUE && ast->nodes[ast->cur_index].type == toketype)
#define ISTOKENTYPE2(ast, toketype1, toketype2)\
(ast->nodes[ast->cur_index].isvalid == TRUE && ast->nodes[ast->cur_index].type == toketype1 \
|| ast->nodes[ast->cur_index].type == toketype2)
#define ISTOKENTYPE3(ast, toketype1, toketype2, toketype3)\
(ast->nodes[ast->cur_index].isvalid == TRUE && ast->nodes[ast->cur_index].type == toketype1 \
|| ast->nodes[ast->cur_index].type == toketype2 || ast->nodes[ast->cur_index].type == toketype3)
#define ISTOKENTYPE4(ast, toketype1, toketype2, toketype3, toketype4)\
(ast->nodes[ast->cur_index].isvalid == TRUE && ast->nodes[ast->cur_index].type == toketype1 \
|| ast->nodes[ast->cur_index].type == toketype2 || ast->nodes[ast->cur_index].type == toketype3\
|| ast->nodes[ast->cur_index].type == toketype4)
#define ISTOKENTYPE5(ast, toketype1, toketype2, toketype3, toketype4, toketype5)\
(ast->nodes[ast->cur_index].isvalid == TRUE && ast->nodes[ast->cur_index].type == toketype1 \
|| ast->nodes[ast->cur_index].type == toketype2 || ast->nodes[ast->cur_index].type == toketype3\
|| ast->nodes[ast->cur_index].type == toketype4 || ast->nodes[ast->cur_index].type == toketype5)
#define ISTOKENTYPE6(ast, toketype1, toketype2, toketype3, toketype4, toketype5, toketype6)\
(ast->nodes[ast->cur_index].isvalid == TRUE && ast->nodes[ast->cur_index].type == toketype1 \
|| ast->nodes[ast->cur_index].type == toketype2 || ast->nodes[ast->cur_index].type == toketype3\
|| ast->nodes[ast->cur_index].type == toketype4 || ast->nodes[ast->cur_index].type == toketype5\
|| ast->nodes[ast->cur_index].type == toketype6)
//自定义节点号的判断token类型
#define ISTOKENTYPEX(ast, nodenum, toketype)\
(ast->nodes[nodenum].isvalid == TRUE && ast->nodes[nodenum].type == toketype)
#define ISTOKENTYPEX2(ast, nodenum, toketype1, toketype2)\
(ast->nodes[nodenum].isvalid == TRUE && ast->nodes[nodenum].type == toketype1 \
|| ast->nodes[nodenum].type == toketype2)
#define ISTOKENTYPEX3(ast, nodenum, toketype1, toketype2, toketype3)\
(ast->nodes[nodenum].isvalid == TRUE && ast->nodes[nodenum].type == toketype1 \
|| ast->nodes[nodenum].type == toketype2 || ast->nodes[nodenum].type == toketype3)
#define ISTOKENTYPEX4(ast, nodenum, toketype1, toketype2, toketype3, toketype4)\
(ast->nodes[nodenum].isvalid == TRUE && ast->nodes[nodenum].type == toketype1 \
|| ast->nodes[nodenum].type == toketype2 || ast->nodes[nodenum].type == toketype3\
|| ast->nodes[nodenum].type == toketype4)
#define ISTOKENTYPEX5(ast, nodenum, toketype1, toketype2, toketype3, toketype4, toketype5)\
(ast->nodes[nodenum].isvalid == TRUE && ast->nodes[nodenum].type == toketype1 \
|| ast->nodes[nodenum].type == toketype2 || ast->nodes[nodenum].type == toketype3\
|| ast->nodes[nodenum].type == toketype4 || ast->nodes[nodenum].type == toketype5)
#define ISTOKENTYPEX6(ast, nodenum, toketype1, toketype2, toketype3, toketype4, toketype5, toketype6)\
(ast->nodes[nodenum].isvalid == TRUE && ast->nodes[nodenum].type == toketype1 \
|| ast->nodes[nodenum].type == toketype2 || ast->nodes[nodenum].type == toketype3\
|| ast->nodes[nodenum].type == toketype4 || ast->nodes[nodenum].type == toketype5\
|| ast->nodes[nodenum].type == toketype6)
//判断token类别
#define ISTOKCATE(ast, toketype)\
(ast->nodes[ast->cur_index].isvalid == TRUE && ast->nodes[ast->cur_index].category == toketype)
#define ISTOKCATE2(ast, toketype1, toketype2)\
(ast->nodes[ast->cur_index].isvalid == TRUE && ast->nodes[ast->cur_index].category == toketype1 \
|| ast->nodes[ast->cur_index].category == toketype2)
#define ISTOKCATE3(ast, toketype1, toketype2, toketype3)\
(ast->nodes[ast->cur_index].isvalid == TRUE && ast->nodes[ast->cur_index].category == toketype1 \
|| ast->nodes[ast->cur_index].category == toketype2 || ast->nodes[ast->cur_index].category == toketype3)
#define ISTOKCATE4(ast, toketype1, toketype2, toketype3, toketype4)\
(ast->nodes[ast->cur_index].isvalid == TRUE && ast->nodes[ast->cur_index].category == toketype1 \
|| ast->nodes[ast->cur_index].category == toketype2 || ast->nodes[ast->cur_index].category == toketype3\
|| ast->nodes[ast->cur_index].category == toketype4)
#define ISTOKCATE5(ast, toketype1, toketype2, toketype3, toketype4, toketype5)\
(ast->nodes[ast->cur_index].isvalid == TRUE && ast->nodes[ast->cur_index].category == toketype1 \
|| ast->nodes[ast->cur_index].category == toketype2 || ast->nodes[ast->cur_index].category == toketype3\
|| ast->nodes[ast->cur_index].category == toketype4 || ast->nodes[ast->cur_index].category == toketype5)
#define ISTOKCATE6(ast, toketype1, toketype2, toketype3, toketype4, toketype5, toketype6)\
(ast->nodes[ast->cur_index].isvalid == TRUE && ast->nodes[ast->cur_index].category == toketype1 \
|| ast->nodes[ast->cur_index].category == toketype2 || ast->nodes[ast->cur_index].category == toketype3\
|| ast->nodes[ast->cur_index].category == toketype4 || ast->nodes[ast->cur_index].category == toketype5\
|| ast->nodes[ast->cur_index].category == toketype6)
//判断token类别
#define ISTOKCATEX(ast, nodenum, toketype)\
(ast->nodes[nodenum].isvalid == TRUE && ast->nodes[nodenum].category == toketype)
#define ISTOKCATEX2(ast, nodenum, toketype1, toketype2)\
(ast->nodes[nodenum].isvalid == TRUE && ast->nodes[nodenum].category == toketype1 \
|| ast->nodes[nodenum].category == toketype2)
#define ISTOKCATEX3(ast, nodenum, toketype1, toketype2, toketype3)\
(ast->nodes[nodenum].isvalid == TRUE && ast->nodes[nodenum].category == toketype1 \
|| ast->nodes[nodenum].category == toketype2 || ast->nodes[nodenum].category == toketype3)
#define ISTOKCATEX4(ast, nodenum, toketype1, toketype2, toketype3, toketype4)\
(ast->nodes[nodenum].isvalid == TRUE && ast->nodes[nodenum].category == toketype1 \
|| ast->nodes[nodenum].category == toketype2 || ast->nodes[nodenum].category == toketype3\
|| ast->nodes[nodenum].category == toketype4)
#define ISTOKCATEX5(ast, nodenum, toketype1, toketype2, toketype3, toketype4, toketype5)\
(ast->nodes[nodenum].isvalid == TRUE && ast->nodes[nodenum].category == toketype1 \
|| ast->nodes[nodenum].category == toketype2 || ast->nodes[nodenum].category == toketype3\
|| ast->nodes[nodenum].category == toketype4 || ast->nodes[nodenum].category == toketype5)
#define ISTOKCATEX6(ast, nodenum, toketype1, toketype2, toketype3, toketype4, toketype5, toketype6)\
(ast->nodes[nodenum].isvalid == TRUE && ast->nodes[nodenum].category == toketype1 \
|| ast->nodes[nodenum].category == toketype2 || ast->nodes[nodenum].category == toketype3\
|| ast->nodes[nodenum].category == toketype4 || ast->nodes[nodenum].category == toketype5\
|| ast->nodes[nodenum].category == toketype6)
//判断节点是否为运算符
#define ISTOKEXPRESS(ast, nodenum) (ast->nodes[nodenum].isvalid == TRUE  &&  \
								(ast->nodes[nodenum].type == TOKEN_TYPE_ASSIGN || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_PLUS || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_MINIS || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_TIMES || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_DIVIDE || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_GREAT || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_LESS || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_EQUAL || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_NOT_EQ || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_GREAT_EQ || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_LESS_EQ || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_AND || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_OR || \
							    ast->nodes[nodenum].type == TOKEN_TYPE_REVERSE))
//判断节点类型是否为表达式
#define ISTOKCATEOP(ast) ISTOKCATE5(ast, TOKEN_CATEGORY_ASSIGN,TOKEN_CATEGORY_PLUS_MINIS,TOKEN_CATEGORY_TIM_DIV,TOKEN_CATEGORY_RELATION,TOKEN_CATEGORY_LOGIC)
#define ISTOKCATEXOP(ast, nodenum) ISTOKCATE5(ast, nodenum, TOKEN_CATEGORY_ASSIGN,TOKEN_CATEGORY_PLUS_MINIS,TOKEN_CATEGORY_TIM_DIV,TOKEN_CATEGORY_RELATION,TOKEN_CATEGORY_LOGIC)

typedef enum
{
	TOKEN_TYPE_DEFAULT,				//默认类型
	TOKEN_TYPE_ID,					//对象
	TOKEN_TYPE_RESERVE,				//关键字
	TOKEN_TYPE_NUM,					//整形
	TOKEN_TYPE_FLOAT,				//浮点数
	TOKEN_TYPE_STR,					//字符串
	TOKEN_TYPE_PLUS,				//加
	TOKEN_TYPE_MINIS,				//减
	TOKEN_TYPE_TIMES,				//乘
	TOKEN_TYPE_DIVIDE,				//除
	TOKEN_TYPE_ASSIGN,				//等于
	TOKEN_TYPE_LBRACKET,			//左括号
	TOKEN_TYPE_RBRACKET,			//右括号
	TOKEN_TYPE_SEMI,				//分号
	TOKEN_TYPE_GREAT,				//大于
	TOKEN_TYPE_LESS,				//小于
	TOKEN_TYPE_EQUAL,				//等于
	TOKEN_TYPE_NOT_EQ,				//不等
	TOKEN_TYPE_GREAT_EQ,			//大于等于
	TOKEN_TYPE_LESS_EQ,				//小于等于
	TOKEN_TYPE_AND,					//与
	TOKEN_TYPE_OR,					//或
	TOKEN_TYPE_REVERSE,				//取反
	TOKEN_TYPE_ENDFILE,				//结束
	TOKEN_TYPE_ERROR				//错误
}ENUM_TOKEN_TYPE;

/*token类别*/
typedef enum
{
	TOKEN_CATEGORY_START, 
	TOKEN_CATEGORY_PLUS_MINIS, 
	TOKEN_CATEGORY_ASSIGN, 
	TOKEN_CATEGORY_TIM_DIV, 
	TOKEN_CATEGORY_RELATION, 
	TOKEN_CATEGORY_LOGIC, 
	TOKEN_CATEGORY_FACTOR
}TOKEN_CATEGORY;

/*token优先级,枚举值越大优先级越高*/
typedef enum
{
	TOKEN_LEVEL_START, 
	TOKEN_LEVEL_ASSIGN,
	TOKEN_LEVEL_LOGIC, 
	TOKEN_LEVEL_RELATION, 
	TOKEN_LEVEL_PLUS_MINIS, 
	TOKEN_LEVEL_TIM_DIV,
	TOKEN_LEVEL_REVERSE
}TOKEN_LEVEL;

/*关键字枚举*/
typedef enum
{
	RESERVE_TYPE_DEFAULT,
	RESERVE_TYPE_PRINT, 
	RESERVE_TYPE_IF, 
	RESERVE_TYPE_THEN, 
	RESERVE_TYPE_ELIF, 
	RESERVE_TYPE_ELSE, 
	RESERVE_TYPE_ENDIF
}RESERVE_TYPE;

typedef struct
{
	unsigned int count;
	unsigned int size;
	char* str;
	ENUM_TOKEN_TYPE type;
	RESERVE_TYPE reserve;

	unsigned int curline;
	unsigned int curcolum;
}TOKEN_DATA;

#define Default_Token_Size 24

typedef struct
{
	unsigned int count;
	unsigned int size;
	unsigned int* childnum;
}NODE_CHILD;

#define Default_Child_Size 3

typedef struct
{
	BOOL isvalid;
	ENUM_TOKEN_TYPE type;
	RESERVE_TYPE reserve_type;
	TOKEN_CATEGORY category;
	TOKEN_LEVEL level;

	char* strtoken;

	NODE_CHILD childs;
	unsigned int nextnode;

	unsigned int line_no;
	unsigned int colum_no;

	BOOL isprinted;

}NODE_DATA;

//生成ast语法树堆栈数据结构
typedef struct
{
	TOKEN_CATEGORY token_category;//栈节点token分类
	TOKEN_LEVEL token_level;//栈节点优先级
}STACK_DATA;

typedef struct
{
	unsigned int size;
	unsigned int count;
	STACK_DATA* stacks;
}STACKLIST_DATA;
#define Default_Stack_Size 5		//默认堆栈大小

typedef struct
{
	unsigned int size;
	unsigned int count;

	NODE_DATA* nodes;

	unsigned int rootnode;

	//算法用到的变量
	BOOL is_builded;				//构建完成标志
	int p_index;			//当前指向的父节点
	int cur_index;			//当前指向的节点

	STACKLIST_DATA stack;	//栈数据，生成语法树过程使用

}AST_TREE;

#define Default_Node_Size 64



typedef enum
{
	STATES_START, 
	STATES_INID, 
	STATES_INNUM, 
	STATES_INFLOAT, 
	STATES_INSTR, 
	STATES_DOWN, 
	STATES_INASSIGN, 
	STATES_INPLUS_MINIS, 
	STATES_INTIME_DIVIDE,
	STATES_INGREAT, 
	STATES_INLESS, 
	STATES_INNOT_EQ, 
	STATES_INAND, 
	STATES_INOR, 
	STATES_INAND_OR, 
	STATES_INREVERSE, 
	STATES_INPRINT, 
	STATES_INRELATION, 
	STATES_INOP
}PROCESS_STATES;

typedef struct
{
	char* filename;
	FILE* file;
	char buf[BUF_SIZE];
	int cur;
	BOOL needread;

	unsigned int curline;
	unsigned int curcolum;
}SOURCE_DATA;


typedef struct
{
	unsigned int count;
	unsigned int size;
	void* point;
}POINTER_TYPE;

BOOL MallocPoint(POINTER_TYPE* point, unsigned int each_size, unsigned int buf_size);
BOOL ReturnFalse(char* err, ...);
void Fault(char* err, ...);

#endif