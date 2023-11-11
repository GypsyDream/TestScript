#include "GlobalStruct.h"

void Fault(NODE_DATA* node, char* err, ...)
{
	if (node != NULL)
		printf("err node str:%s\node position:%d,%d\n", node->strtoken, node->line_no, node->colum_no);
	if (err != NULL)
	{
		va_list arg;
		va_start(arg, err);
		vprintf(err, arg);
		va_end(arg);
		system("pause");
		exit(0);
	}
}

BOOL ReturnFalse(char* err, ...)
{
	if (err != NULL)
	{
		va_list arg;
		va_start(arg, err);
		vprintf(err, arg);
		va_end(arg);
	}
	return FALSE;
}

BOOL MallocPoint(POINTER_TYPE* point, int each_size, int buf_size)
{
	if (point == NULL)
		return FALSE;

	if (point->point == NULL)
	{
		point->size = buf_size;
		point->point = malloc(each_size * buf_size);
		if (point->point == NULL)
			return FALSE;
	}
	else if (point->size <= point->count)
	{

	}
	return TRUE;
}


