#include "stdafx.h"
#include "merror.h"

//it is possible that the function should return and integer representing and error code

int error_code(char *error_code)
{
	printf(">> %s", error_code);
	return -1; //for now this fuction will only return -1  
}