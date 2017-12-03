// Tiny Basic Draft 7.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "global.h"
#include "paser.h"

char source_line[SOURCE_LINE_LENGTH];
char *psource = source_line;
char source_line_without_line_number[SOURCE_LINE_LENGTH];

Node *root = NULL; //the root of the list
Node *tail = NULL; //the tail of the list
Node *index;      //pointer to an individual node

FILE *source_file;

void insert_tail(int source_line_number, char *e);

int _tmain(int argc, _TCHAR* argv[])
{
	//Open a source file
	if (!(source_file = _tfsopen(argv[1], L"rt", _SH_DENYNO)))
	{
		printf("\nThe file could not be opened\n");
		return 0;
	}
	else
	{
		//load the source file into a linked list structure where each node
		//contains one line of source code
		while(!feof(source_file))
		{	
			fgets(source_line, SOURCE_LINE_LENGTH, source_file);

			//Only move to the linked list the lines of the source code
			//no carriage returns or empty strings
			if(lex().token_code != NO_TOKEN)
			{
				strcpy(source_line_without_line_number, psource);
				insert_tail(number_value, source_line_without_line_number);
			}
			psource = source_line;
		}

		//print the source program on the screen
		print_nodes();
	}


	
	//Command line
	while(true){
	printf("* ");
	fgets(source_line, SOURCE_LINE_LENGTH, stdin);
	
	parser();
	psource = source_line;
	fflush(stdin);
	}
	
	//Close a source file
	fclose(source_file);
	
	return 0;
}

void print_nodes(void)
{
	Node *pIndex = root;
	
	while(pIndex)
	{
		printf("* %d%s", pIndex->line_number, pIndex->source_line);
		pIndex = pIndex->Sig;
	}

	printf("\n");
}

//insert a node at the end of the list
void insert_tail(int source_line_number, char *e)
{
	Node *new_node;

	//allocates space for the node structure
	new_node = (Node *) malloc(sizeof(Node));
	//allocates space for the source line string
	new_node -> source_line = (char *) malloc(sizeof(char) * strlen(e) + 1);
	//copy the source line in the memory just allocated for it
	strcpy(new_node -> source_line, e);
	new_node->line_number = source_line_number;
	
	new_node->Sig = NULL;
	
	if (root == NULL)
	{
		root = new_node;
		tail = new_node;
	}
	else
	{
		tail->Sig = new_node;
		tail = new_node;
	}
}