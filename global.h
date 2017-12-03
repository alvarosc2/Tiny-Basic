#include "stdafx.h"
#define SOURCE_LINE_LENGTH 255

//Linked list to store each source line of the source file
//in an individual node
typedef struct Registro
{
	int line_number;
	char *source_line;
	struct Registro *Sig;
}Node;

extern char source_line[];
extern char *psource;
extern char source_line_without_line_number[];