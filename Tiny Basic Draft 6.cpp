// Tiny Basic Draft 5.cpp : Defines the entry point for the console application.
// implements a finite automata that recognises words, numbers and relational operators
// for the tiny basic language

#include "stdafx.h"
#define SOURCE_LINE_LENGTH 255

FILE *source_file;

//Stack structure to store line numbers for the GOSUB
//subroutines
typedef struct StackNode
{
	int line_number;
	struct StackNode *Sig;
}Stack;

Stack *stack_bottom;

//Linked list to store each source line of the source file
typedef struct Registro
{
	int line_number;
	char e[SOURCE_LINE_LENGTH];
	struct Registro *Sig;
}Nodo;

Nodo *root = NULL;
Nodo *tail = NULL;
Nodo *index;

int t = 0;
int machine_state = 0;
int start = 0;

//Numeric value of a number
int number_value = 0;
int line_number;

//Expression result
int expression_result;

//buffer, associated pointer and length of the buffer 
char tokenb[255];
int tokenp = 0;
int tokenb_length = 0;
char *ptoken = tokenb;

//token type. IDENTIFIER, REAL, INTEGER,REL OP
char token_type[255];

//  Token codes

typedef enum {
    NO_TOKEN, IDENTIFIER, INTEGER, STRING,
    PRINT, IF, THEN, GOTO, INPUT, LET,
	GOSUB, RETURN, CLEAR, LIST, RUN, END,
	GT, GE, LT, LE, EQUAL, NE, PLUS, MINUS,
	TIMES, DIV, LPARENT, RPARENT, COMMA,
	QUOTE,
} TOKEN_CODE;

//Reserved word table
typedef struct {
    char       string[255];
    TOKEN_CODE token_code;
} SYM_TAB_NODE;

SYM_TAB_NODE stn;

const int sym_tab_length = 12;

SYM_TAB_NODE sym_tab[sym_tab_length] = {
    {"PRINT", PRINT}, {"IF", IF}, {"THEN", THEN}, {"GOTO", GOTO}, {"INPUT", INPUT},
    {"LET", LET}, {"GOSUB", GOSUB}, {"RETURN", RETURN}, {"CLEAR", CLEAR}, {"LIST", LIST},
	{"RUN", RUN}, {"END", END},
};

//Variable array
const int variable_number = 26;
int variables[variable_number] = {0};
int varp;  //variable array pointer

char source_line[SOURCE_LINE_LENGTH];
char *psource = source_line;

int get_char(void);
void print_nodes(void);
void print_token(void);
void ungetchar(void);
int failed(void);
SYM_TAB_NODE lex(void);
int error_code(void);

void parser(void);
void line(void);
void statement(void);
void expr_list(void);
int expression(void);
int term(void);
int factor(void);
int relop(int exp1, SYM_TAB_NODE sim, int exp2);
void var_list(void);
int var(void);
int number(void);

void AnadePL(char e[]);

int _tmain(int argc, _TCHAR* argv[])
{
	//Open a source file
	if (!(source_file = _tfsopen(argv[1], L"rt", _SH_DENYNO)))
	{
		printf("\nEl archivo no pudo abrirse\n");
		return 0;
	}
	
	//load the source file into a linked list structure where each node
	//contains one line of source code
	//There is a bug here: if there is white space after the last line of code
	//the lexer does not make a good treatment of the EOF code
	while(!feof(source_file))
	{
		AnadePL(fgets(source_line, SOURCE_LINE_LENGTH, source_file));
		index = tail;
		//Get the line number strings and convert them to their numeric value
		lex();
		number();
		index->line_number = number_value;
		psource = source_line;
	}

	//Close a source file
	fclose(source_file);
	
	print_nodes();

	//Command line
	while(true){
	printf("* ");
	fgets(source_line, SOURCE_LINE_LENGTH, stdin);
	
	parser();
	psource = source_line;
	fflush(stdin);
	}
	return 0;
}

void print_nodes(void)
{
	Nodo *pIndex = root;
	
	while(pIndex != NULL)
	{
		printf("* %s", pIndex->e);
		pIndex = pIndex->Sig;
	}

	printf("\n");
}

int get_char(void)
{
	return *psource++;
}

void print_token(void)
{
	printf("<%s> %s\n", token_type, tokenb);
}

void ungetchar(void)
{
	*psource--;
}

int failed(void)
{
	ungetchar();
	
	switch (start)
	{
	case 0:   start = 13;   break; //After testing white space and relational operators test identifiers
	case 13:  start = 16;   break; //After testing IDENTIFIERS test INTEGERS
	case 16:  start = 19;   break; //After testing INTERGERS test STRINGS
	case 19:  error_code(); break; 
	default:  error_code(); break;
	}

	return start;
}

//implementation of a sequence of transition diagrams
SYM_TAB_NODE lex(void)
{
	start = 0;
	machine_state = 0;

	int j = 0;
	bool is_reserved_word = false;
	
	while(1)
	{
		switch (machine_state)
		{
		case 0:
			t = get_char();
			//t = getchar(); //t is the lookahead character
			
			if (t == ' ' || t == '\t' || t == '\n')
			{
				if (t == '\n')
				{
					strcpy(stn.string, "");
					stn.token_code = NO_TOKEN;
					return stn;
					//index = index->Sig;
					//strcpy(source_line, index->e);
					//psource = source_line;
				}
				machine_state = 0;
			}
			else if (t == '<') 
			{
				machine_state = 1;
			}
			else if (t == '=')
			{
				machine_state = 5;
			}
			else if (t == '>')
			{
				machine_state = 6;
			}
			else if (t == '+')
			{
				machine_state = 9;
			}
			else if (t == '-')
			{
				machine_state = 10;
			}
			else if (t == '*')
			{
				machine_state = 11;
			}
			else if (t == '/')
			{
				machine_state = 12;
			}
			else if (t == '(')
			{
				machine_state = 30;
			}
			else if (t == ')')
			{
				machine_state = 31;
			}
			else if (t == ',')
			{
				machine_state = 32;
			}
			else
				machine_state = failed();
			
			break;

		case 1:
			t = get_char();
			//t = getchar();

			if (t == '=')
				machine_state = 2;
			else if (t == '>')
				machine_state = 3;
			else
				machine_state = 4;

			break;

		case 2:
			//devuelve(oprel, MEI)
			strcpy(stn.string, "<=");
			stn.token_code = LE;

			return stn;

		case 3:
			//devuelve(oprel, DIF)
			strcpy(stn.string, "<>");
			stn.token_code = NE;

			return stn;

		case 4:
			//devuelve(oprel, MEN)
			strcpy(stn.string, "<");
			stn.token_code = LT;

			return stn;

		case 5:
			//devuelve(oprel, IGU)
			strcpy(stn.string, "=");
			stn.token_code = EQUAL;

			return stn;

		case 6:
			t = get_char();
			//t = getchar();

			if (t == '=')
				machine_state = 7;
			else
				machine_state = 8;

			break;

		case 7:
			//devuelve(oprel, MAI)
			strcpy(stn.string, ">=");
			stn.token_code = GE;

			return stn;

		case 8:
			ungetchar();
			//devuelve(oprel, MAY)
			strcpy(stn.string, ">");
			stn.token_code = GT;

			return stn;
		
		case 9:
			//devuelve(op, PLUS)
			strcpy(stn.string, "+");
			stn.token_code = PLUS;

			return stn;

		case 10:
			//devuelve(op, MINUS)
			strcpy(stn.string, "-");
			stn.token_code = MINUS;

			return stn;

		case 11:
			//devuelve(op, TIMES)
			strcpy(stn.string, "*");
			stn.token_code = TIMES;

			return stn;

		case 12:
			//devuelve(op, DIV)
			strcpy(stn.string, "/");
			stn.token_code = DIV;

			return stn;

		//states to determine an IDENTIFIER
		case 13:
			//t = getchar();
			t = get_char();

			if (isalpha(t))
			{
				machine_state = 14;
				*ptoken = t;
				ptoken++;
			}
			else
				machine_state = failed();

			break;

		case 14:
			//t = getchar();
			t = get_char();

			if (isalnum(t))
			{
				machine_state = 14;
				*ptoken = t;
				ptoken++;
			}
			else
				machine_state = 15;

			break;
		
		//Acceptance state for identifiers
		case 15:
			ungetchar();
			*ptoken = '\0'; //end buffer with null character
			ptoken = tokenb; //reset buffer pointer
			
			//convert the identifier from lowercase to uppercase
			while (*ptoken)
			{
				*ptoken = toupper(*ptoken);
				ptoken++;
			}
			
			//test if the identifier is a reserved word
			ptoken = tokenb; //reset buffer pointer
			j = 0;
			while(j < sym_tab_length)
			{
				if (!strcmp(tokenb, sym_tab[j].string))
				{
					//compare a table of reserved words agains a the word just captured
					strcpy(stn.string, sym_tab[j].string);
					stn.token_code = sym_tab[j].token_code;
					is_reserved_word = true;

					break;
				}

				j++;
			}
			
			//It is NOT a reserved word, then it is an identifier
			if (!is_reserved_word)
			{
				strcpy(stn.string, tokenb);
				stn.token_code = IDENTIFIER;
			}
			return stn;
		
		//states to determine an INTEGER
		//input state is 16
		case 16:
			//t = getchar();
			t = get_char();

			if (isdigit(t))
			{
				machine_state = 17;
				*ptoken = t;
				ptoken++;
			}
			else
				machine_state = failed();

			break;

		case 17:
			//t = getchar();
			t = get_char();

			if (isdigit(t))
			{
				machine_state = 17;
				*ptoken = t;
				ptoken++;			
			}
			else
				machine_state = 18;

			break;

		case 18:
			ungetchar();
			*ptoken = '\0'; //end buffer with null character
			ptoken = tokenb; //reset buffer pointer
			strcpy(stn.string, tokenb);
			stn.token_code = INTEGER;
			
			return stn;
		//End of states to determine an INTEGER
		
		//states to determine a STRING
		case 19: 
			//t = getchar();
			t = get_char();

			if (t == '"')
			{
				//Do not print the opening quotation marks
				machine_state = 20;
			}
			else
				failed();
			
			break;
		
		case 20:
			//t = getchar();
			t = get_char();

			if (t != '"')
			{
				machine_state = 20;
				*ptoken = t;
				ptoken++;			
			}
			else
			{
				//do not print the closing quotation marks
				machine_state = 21;			
			}

			break;
		
		//Acceptance state for STRINGs
		case 21:
			//ungetc(t, stdin);
			*ptoken = '\0'; //end buffer with null character
			ptoken = tokenb; //reset buffer pointer

			strcpy(stn.string, tokenb);
			stn.token_code = STRING;
	
			return stn;
			//End of states to determine a STRING

		case 30:
			//devuelve(op, LPARENT)
			strcpy(stn.string, "(");
			stn.token_code = LPARENT;

			return stn;

		case 31:
			//devuelve(op, RPARENT)
			strcpy(stn.string, ")");
			stn.token_code = RPARENT;

			return stn;

		case 32:
			//devuelve(op, COMMA)
			strcpy(stn.string, ",");
			stn.token_code = COMMA;

			return stn;
		}
	}
}

//it is possible that the function should return and integer representing and error code
int error_code(void)
{
	printf(">> Error!");
	exit(-1); //for now this fuction will only exit with code -1  
}


void parser(void)
{
	lex();
	line();
}

void line(void)
{
	if (stn.token_code == INTEGER)
	{
		number();
		lex();
		statement();
	}
	else
	{
		statement();
	}
}

void statement(void)
{
 	if(stn.token_code == PRINT)
	{
		expr_list();
	}
	else if (stn.token_code == IF)
	{
		int expression1;  //temporal allocation for expression1
		int expression2;  //temporal allocation for expression2
		SYM_TAB_NODE temp; //temporal allocation for relational operators
		
		//expression1 relop expression2

		lex();
		expression1 = expression();
		
		strcpy(temp.string, stn.string);
		temp.token_code =  stn.token_code;
		
		lex();
		expression2 = expression();

		if (relop(expression1, temp, expression2))
		{
			if (stn.token_code == THEN)
			{
				 lex();
				 statement();
			}
			else
				error_code();
		}
		else
			return;
	}
	else if (stn.token_code == GOTO)
	{
		int expression1;

		lex();
		expression1 = expression();
		
		index = root;
		while (index)
		{
			if (index->line_number == expression1)
				break;

			index = index->Sig;
		}
	}
	else if (stn.token_code == INPUT)
	{
		var_list();
	}
	else if (stn.token_code == LET)
	{
		int varp2 = 0; //pointer to the variables array
		//get the index for the variable of the left side of the assignment
		lex();
		varp2 = stn.string[0] - 65;
		//get assignment operator
		lex();
		//Evaluate the expression
		lex();
		//Assign the result of the expression() to the variable
		variables[varp2] = expression();
	}
	else if (stn.token_code == GOSUB) //This will require to implement a simple stack
	{
		int expression1;
		Nodo *next;  //the next instruction after the gosub jump we need to store that address

		lex();
		expression1 = expression(); //The line number to jump to

		//we need the next line of source code after the gosub
		next = index;

		//This piece looks for source line corresponding to the line number to jump
		index = root;
		while (index)
		{
			if (index->line_number == expression1)
				break;

			index = index->Sig;
		}

		//This code will execute all the instructions in the subroutine
		while (index)
		{
			strcpy(source_line, index->e);
			psource = source_line;
			index = index->Sig;

			parser();    
		}

		index = next; //restore the original instruction pointer
		psource = source_line;
	}
	else if (stn.token_code == RETURN)
	{
		return;
	}
	else if (stn.token_code == CLEAR)
	{
		system("cls"); //Non portable feature to clear the screen
	}
	else if (stn.token_code == LIST)
	{
		print_nodes();
	}
	else if (stn.token_code == RUN)
	{
		index = root;

		while (index)
		{
			strcpy(source_line, index->e);
			psource = source_line;
			index = index->Sig;

			parser();
		    
		}
	}
	else if (stn.token_code == END)
	{
		return;
	}
	else
		error_code();

}

void expr_list(void)
{
	lex();

	if (stn.token_code == STRING)
	{
		printf("%-14s", stn.string);
		lex();

		while (stn.token_code == COMMA)
		{
			lex();
		
			if (stn.token_code == STRING)
			{
				printf("%-14s", stn.string);
			}
			else
			{
				printf("%-14d", expression());
			}
			
			lex();
		}
		printf("\n");
	}
	else
	{
		printf("%-14d", expression());

		while (stn.token_code == COMMA)
		{
			lex();
		
			if (stn.token_code == STRING)
			{
				printf("%-14s", stn.string);
			}
			else
			{
				printf("%-14d", expression());
			}
		}
		printf("\n");
	}
}

int expression(void)
{
	int expression1;

	expression1 = term();
	
	if(stn.token_code != NO_TOKEN)
	{
		while ((stn.token_code == PLUS) || (stn.token_code == MINUS))
		{
			if (stn.token_code == PLUS)
			{
				lex();
				expression1 += term();
			}
			else if (stn.token_code == MINUS)
			{
				lex();
				expression1 -= term();
			}
		}
	}

	return expression1;
}

int term(void)
{
	int term1;
	
	term1 = factor();
	lex();

	if(stn.token_code != NO_TOKEN)
	{
		while ((stn.token_code == TIMES) || (stn.token_code == DIV))
		{
			if (stn.token_code == TIMES)
			{
				lex();
				term1 *= factor();
				lex();//fails here
			}
			else if (stn.token_code == DIV)
			{
				lex();
				term1 /= factor();
				lex();
			}

		}
	}
	return term1;
}

int factor(void)
{
	int factor1;

	if (stn.token_code == IDENTIFIER)
	{
		factor1 = var();
	}
	else if (stn.token_code == INTEGER)
	{
		factor1 = number();
	}
	else if (stn.token_code == LPARENT)
	{
		lex();
		factor1 = expression();
		if (stn.token_code != RPARENT)
			error_code();
	}

	return factor1;
}

int relop(int exp1, SYM_TAB_NODE sim, int exp2)
{
	switch (sim.token_code)
	{
	case LT:
		return  exp1 < exp2;

	case LE:
		return  exp1 <= exp2;

	case GT:
		return  exp1 > exp2;

	case GE:
		return  exp1 >= exp2;

	case EQUAL:
		return  exp1 == exp2;

	case NE:
		return  exp1 != exp2;

	default:
		error_code();
	}
}

void var_list(void)
{
	lex();
	var();
	printf("?");
	scanf("%d", &variables[varp]);
	lex();

	if(stn.token_code != NO_TOKEN)
	{
		while (stn.token_code == COMMA)
		{
			//Get alternatively variables followed by COMMAS
			lex();
			var();
			printf("?");
			scanf("%d", &variables[varp]);
			lex();
		}
	}
}

int var(void)
{
	varp = stn.string[0] - 65;

	return variables[varp];
}

int number(void)
{		
	int i = 0;
	number_value = 0;

	//This should be part of a scanner
	while (stn.string[i])
	{
		number_value = number_value * 10 + stn.string[i] - '0';
		i++;
	}
	//End of the code that should be part of a scanner

	return number_value;
}

//insert a node at the end of the list
void AnadePL(char e[])
{
	Nodo *Nuevo;
	Nuevo = (Nodo *) malloc(sizeof(Nodo));
	strcpy(Nuevo -> e, e);
	Nuevo->Sig = NULL;
	
	if (root == NULL)
	{
		root = Nuevo;
		tail = Nuevo;
	}
	else
	{
		tail->Sig = Nuevo;
		tail = Nuevo;
	}
}

//insert a node at the begining of the list
//this node contains the line number of the
//line of code after the gosub which calls this
void push(int line_no)
{
	Stack *Nuevo;
	Nuevo = (Stack *) malloc(sizeof(Stack));
	Nuevo -> line_number = line_no;
	Nuevo -> Sig = stack_bottom;
	stack_bottom = Nuevo;
}

int pop(void)
{
	int tmp; //temporal variable

	Stack *Ptr;
	Ptr = stack_bottom;

	if (Ptr == NULL)
	{
		printf("The stack is empty");
		return -1; //line numbers are positive integers
	}
	tmp = Ptr->line_number;
	stack_bottom = Ptr->Sig;
	free(Ptr);

	return tmp;
}