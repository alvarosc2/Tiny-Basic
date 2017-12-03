#include "stdafx.h"
#include "Tokenizer.h"

#define TEST_SPECIAL     0
#define TEST_IDENTIFIER 13
#define TEST_INTEGER    16
#define TEST_STRING     19

TOKEN token;

//Reserved word table
TOKEN rw_table[RW_TABLE_LENGTH] = {
    {"PRINT", PRINT}, {"IF", IF}, {"THEN", THEN}, {"GOTO", GOTO}, {"INPUT", INPUT},
    {"LET", LET}, {"GOSUB", GOSUB}, {"RETURN", RETURN}, {"CLEAR", CLEAR}, {"LIST", LIST},
	{"RUN", RUN}, {"END", END},
};

int t = 0;

int machine_state = 0;
int start = 0;

//buffer and associated pointer to gather tokens
char tokenb[255];
int tokenp = 0;
char *ptoken = tokenb;

//token type. IDENTIFIER, REAL, INTEGER,REL OP
char token_type[255];

//Number in Numeric format
int number_value = 0;

int  get_char(void);
void unget_char(void);
void print_token(void);
int  failed(void);

//implementation of a sequence of transition diagrams
TOKEN lex(void)
{
	int i = 0;
	int j = 0;
	start = 0;
	machine_state = 0;
	bool is_reserved_word = false;
	
	while(1)
	{
		switch (machine_state)
		{
		case 0:
			t = get_char(); //t is the lookahead character
			
			if (t == ' ' || t == '\t')
			{
				machine_state = 0;
			}
			else if (t == '\n')
			{
				strcpy(token.string, "");
				token.token_code = NO_TOKEN;
				return token;
			}
			else if (t == '\0')
			{
				strcpy(token.string, "");
				token.token_code = NO_TOKEN;
				return token;
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
			else if (t == '"')
			{
				machine_state = 19;
			}
			else
				machine_state = failed();
			
			break;

		case 1:
			t = get_char();

			if (t == '=')
				machine_state = 2;
			else if (t == '>')
				machine_state = 3;
			else
				machine_state = 4;

			break;

		case 2:
			//devuelve(oprel, MEI)
			strcpy(token.string, "<=");
			token.token_code = LE;

			return token;

		case 3:
			//devuelve(oprel, DIF)
			strcpy(token.string, "<>");
			token.token_code = NE;

			return token;

		case 4:
			//devuelve(oprel, MEN)
			strcpy(token.string, "<");
			token.token_code = LT;

			return token;

		case 5:
			//devuelve(oprel, IGU)
			strcpy(token.string, "=");
			token.token_code = EQUAL;

			return token;

		case 6:
			t = get_char();

			if (t == '=')
				machine_state = 7;
			else
				machine_state = 8;

			break;

		case 7:
			//devuelve(oprel, MAI)
			strcpy(token.string, ">=");
			token.token_code = GE;

			return token;

		case 8:
			unget_char();
			//devuelve(oprel, MAY)
			strcpy(token.string, ">");
			token.token_code = GT;

			return token;
		
		case 9:
			//devuelve(op, PLUS)
			strcpy(token.string, "+");
			token.token_code = PLUS;

			return token;

		case 10:
			//devuelve(op, MINUS)
			strcpy(token.string, "-");
			token.token_code = MINUS;

			return token;

		case 11:
			//devuelve(op, TIMES)
			strcpy(token.string, "*");
			token.token_code = TIMES;

			return token;

		case 12:
			//devuelve(op, DIV)
			strcpy(token.string, "/");
			token.token_code = DIV;

			return token;

		//states to determine an IDENTIFIER
		case 13:
			t = get_char();

			if (isalpha(t))
			{
				machine_state = 14;
				tokenb[tokenp] = t;
				tokenp++;
			}
			else
				machine_state = failed();

			break;

		case 14:
			t = get_char();

			if (isalpha(t) || isdigit(t))
			{
				machine_state = 14;
				tokenb[tokenp] = t;
				tokenp++;
			}
			else
				machine_state = 15;

			break;
		
		//Acceptance state for identifiers
		case 15:
			unget_char();
			tokenb[tokenp] = '\0'; //end buffer with null character
			tokenp = 0; //reset buffer pointer
			
			//convert the identifier from lowercase to uppercase
			while (tokenb[j])
			{
				tokenb[j] = toupper(tokenb[j]);
				j++;
			}
			
			//test if the identifier is a reserved word
			
			j = 0;
			
			while(j < RW_TABLE_LENGTH)
			{
				if (!strcmp(tokenb, rw_table[j].string))
				{
					//compare a table of reserved words agains a the word just captured
					strcpy(token.string, rw_table[j].string);
					token.token_code = rw_table[j].token_code;
					is_reserved_word = true;

					break;
				}

				j++;
			}
			
			//It is NOT a reserved word, then it is an identifier
			if (!is_reserved_word)
			{
				strcpy(token.string, tokenb);
				token.token_code = IDENTIFIER;
			}
			return token;
		
		//states to determine an INTEGER
		//input state is 16
		case 16:
			t = get_char();

			if (isdigit(t))
			{
				machine_state = 17;
				tokenb[tokenp] = t;
				tokenp++;
			}
			else
				machine_state = failed();

			break;

		case 17:
			t = get_char();

			if (isdigit(t))
			{
				machine_state = 17;
				tokenb[tokenp] = t;
				tokenp++;
			}
			else
				machine_state = 18;

			break;

		case 18:
			unget_char();
			tokenb[tokenp] = '\0'; //end buffer with null character
			tokenp = 0; //reset buffer pointer
			strcpy(token.string, tokenb);
			token.token_code = INTEGER;

			number_value = 0;

			//Get the numeric value
			while (token.string[i])
			{
				number_value = number_value * 10 + token.string[i] - '0';
				i++;
			}
			//End of the code that should be part of a scanner
			
			return token;

		//states to determine a STRING
		case 19:
			t = get_char();

			if (t != '"')
			{
				machine_state = 19;
				tokenb[tokenp] = t;
				tokenp++;
			}
			else
				machine_state = 20;

			break;
		
		//Acceptance state for STRINGs
		case 20:
			//unget_char();
			tokenb[tokenp] = '\0'; //end buffer with null character
			tokenp = 0; //reset buffer pointer

			strcpy(token.string, tokenb);
			token.token_code = STRING;
	
			return token;

		case 30:
			//devuelve(op, LPARENT)
			strcpy(token.string, "(");
			token.token_code = LPARENT;

			return token;

		case 31:
			//devuelve(op, RPARENT)
			strcpy(token.string, ")");
			token.token_code = RPARENT;

			return token;

		case 32:
			//devuelve(op, COMMA)
			strcpy(token.string, ",");
			token.token_code = COMMA;

			return token;
		}
	}
}

int failed(void)
{
	unget_char();
	
	switch (start)
	{
	case TEST_SPECIAL:    start = 13;   break; //After testing white space and relational operators test identifiers
	case TEST_IDENTIFIER: start = 16;   break; //After testing IDENTIFIERS test INTEGERS
	case TEST_INTEGER:    start = 19;   break; //After testing INTERGERS test STRINGS
	case TEST_STRING:     error_code("Syntax error"); break; //If the STRING DFA fails then report an error
	default:         error_code("Syntax error"); break;
	}

	return start;
}

int get_char(void)
{
	return *psource++;
}

void unget_char(void)
{
	psource--;
}

void print_token(void)
{
	printf("<%s> %s\n", token_type, tokenb);
}