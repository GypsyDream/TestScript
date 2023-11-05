#include "GlobalStruct.h"

void Fault(char* err, ...)
{
	if (err != NULL)
	{
		va_list arg;
		va_start(arg, err);
		vprintf(err, arg);
		va_end(arg);
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

BOOL MallocPoint(POINTER_TYPE* point, unsigned int each_size, unsigned int buf_size)
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


