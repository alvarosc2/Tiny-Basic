#include "stdafx.h"
#include "global.h"
#include "paser.h"

int line_number;

//Variable array
const int variable_number = 26;
int variables[variable_number] = {0};
int varp;  //variable array pointer

void match(TOKEN_CODE tcode);
void line(void);
void statement(void);
void expr_list(void);
int expression(void);
int term(void);
int factor(void);
int relop(int exp1, TOKEN tok, int exp2);
void var_list(void);
int var(void);
int number(void);

void match(TOKEN_CODE t_code)
{
	if (token.token_code == t_code)
		lex();
	else
		error_code("Syntax error");
}

void parser(void)
{
	lex();
	line();
}

void line(void)
{
	if (token.token_code == INTEGER)
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
 	if(token.token_code == PRINT)
	{
		expr_list();
	}
	else if (token.token_code == IF)
	{
		int expression1;  //temporal allocation for expression1
		int expression2;  //temporal allocation for expression2
		TOKEN temp; //temporal allocation for relational operators
		
		//expression1 relop expression2

		lex();
		expression1 = expression();
		
		strcpy(temp.string, token.string);
		temp.token_code =  token.token_code;
		
		lex();
		expression2 = expression();

		if (relop(expression1, temp, expression2))
		{
			if (token.token_code == THEN)
			{
				 lex();
				 statement();
			}
			else
				error_code("Syntax error");
		}
		else
			return;
	}
	else if (token.token_code == GOTO)
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
	else if (token.token_code == INPUT)
	{
		var_list();
	}
	else if (token.token_code == LET)
	{
		int varp2 = 0; //pointer to the variables array
		//get the index for the variable of the left side of the assignment
		lex();
		varp2 = token.string[0] - 65;
		//get assignment operator
		lex();
		//Evaluate the expression
		lex();
		//Assign the result of the expression() to the variable
		variables[varp2] = expression();
	}
	else if (token.token_code == GOSUB)
	{
		int expression1;
		Node *next;  //the next instruction after the gosub jump we need to store that address

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
			strcpy(source_line, index->source_line);
			psource = source_line;
			index = index->Sig;

			parser();    
		}

		index = next; //restore the original instruction pointer
		psource = source_line;
	}
	else if (token.token_code == RETURN)
	{
		return;
	}
	else if (token.token_code == CLEAR)
	{
		system("cls"); //Non portable feature to clear the screen
	}
	else if (token.token_code == LIST)
	{
		print_nodes();
	}
	else if (token.token_code == RUN)
	{
		index = root;

		while (index)
		{
			strcpy(source_line, index->source_line);
			psource = source_line;
			index = index->Sig;

			parser();
		}
	}
	else if (token.token_code == END)
	{
		return;
	}
	else
		error_code("Syntax error\n");

}

void expr_list(void)
{
	lex();

	if (token.token_code == STRING)
	{
		printf("%-14s", token.string);
		lex();

		while (token.token_code == COMMA)
		{
			lex();
		
			if (token.token_code == STRING)
			{
				printf("%-14s", token.string);
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

		while (token.token_code == COMMA)
		{
			lex();
		
			if (token.token_code == STRING)
			{
				printf("%-14s", token.string);
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
	
	if(token.token_code != NO_TOKEN)
	{
		while ((token.token_code == PLUS) || (token.token_code == MINUS))
		{
			if (token.token_code == PLUS)
			{
				lex();
				expression1 += term();
			}
			else if (token.token_code == MINUS)
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

	if(token.token_code != NO_TOKEN)
	{
		while ((token.token_code == TIMES) || (token.token_code == DIV))
		{
			if (token.token_code == TIMES)
			{
				lex();
				term1 *= factor();
				lex();
			}
			else if (token.token_code == DIV)
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

	if (token.token_code == IDENTIFIER)
	{
		factor1 = var();
	}
	else if (token.token_code == INTEGER)
	{
		factor1 = number();
	}
	else if (token.token_code == LPARENT)
	{
		lex();
		factor1 = expression();
		if (token.token_code != RPARENT)
			error_code("Syntax error");
	}

	return factor1;
}

int relop(int exp1, TOKEN tok, int exp2)
{
	switch (tok.token_code)
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
		error_code("Syntax error");
	}
}

void var_list(void)
{
	lex();
	var();
	printf("?");
	scanf("%d", &variables[varp]);
	lex();

	if(token.token_code != NO_TOKEN)
	{
		while (token.token_code == COMMA)
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
	varp = token.string[0] - 65;
	return variables[varp];
}

int number(void)
{		
	return number_value;
}
