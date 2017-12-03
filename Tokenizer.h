#include "stdafx.h"
#include "merror.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define RW_TABLE_LENGTH 12
extern int number_value;
extern int t;
extern char source_line[];
extern char *psource;
extern char source_line_without_line_number[];
//  Token codes

typedef enum {
    NO_TOKEN, IDENTIFIER, INTEGER, STRING,
    PRINT, IF, THEN, GOTO, INPUT, LET,
	GOSUB, RETURN, CLEAR, LIST, RUN, END,
	GT, GE, LT, LE, EQUAL, NE, PLUS, MINUS,
	TIMES, DIV, LPARENT, RPARENT, COMMA,
	QUOTE,
} TOKEN_CODE;

typedef struct {
	char       string[255];
    TOKEN_CODE token_code;
} TOKEN;

extern TOKEN token;

//Reserved word table
extern TOKEN rw_table[];

TOKEN lex(void);