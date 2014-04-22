%{

	#define YYSTYPE char *
	#include <stdio.h>
	#include <string.h>
	#include "y.tab.h"

%}

delim 			[ \t\n]
ws    			{delim}+
letter			[A-Za-z]
non-zero		[1-9]
digit			[0-9]

id 				{letter}({letter}|{digit}|[_])*
number			[-]?{non-zero}{digit}*|[0]
float 			[-]?{non-zero}{digit}*\.{digit}+|[-]?[0\.{digit}+]


%x C_COMMENT

%%

{ws}			{  }
"/*"            { BEGIN(C_COMMENT); }
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>.    { }
"//"[^\n]\n 	{  }

"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"for"			{ return FOR; }
"pfor"			{ return PFOR; }
"spawn"			{ return SPAWN; }
"barrier"		{ return BARRIER; }
"true"			{ return TRUE; }
"false"			{ return FALSE; }
"return"		{ return RETURN; }
"lock"			{ return LOCK; }
"printOut"		{ return PRINT; }
"getLineIn"		{ return READ; }

"int"			{ return INT; }
"double"		{ return DOUBLE; }
"char"			{ return CHAR; }
"boolean"		{ return BOOLEAN; }
"string"		{ return STRING; }
"thread"		{ return THREAD; }
"void"			{ return VOID; }

"="             { return ASSIGN; }
"+="			{ return PLUSASSIGN; }
"-="			{ return MINUSASSIGN; }

"!="            { return NE; }
"=="            { return EQ; }
"<"             { return LT; }
"<="            { return LE; }
">"             { return GT; }
">="            { return GE; }
"&&"            { return AND; }
"||"            { return OR; }
"++"			{ return INC; }
"--"			{ return DEC; }
"!"				{ return BOOLEANOP; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return OVER; }
"%"				{ return MOD; }

"{"             { return LBRACE; }
"}"             { return RBRACE; }
"["             { return LBRACK; }
"]"             { return RBRACK; }
"("             { return LPAREN; }
")"             { return RPAREN; }
";"             { return SEMI; }
","             { return COMMA; }

{id}			{ yylval = strdup(yytext); return IDENTIFIER; }
{number} 		{ yylval = strdup(yytext); return INTEGER; }
{float}			{ yylval = strdup(yytext); return FLOAT; }
"'"{letter}"'"	{ yylval = strdup(yytext); return CHARLITERAL; }
"\""([^\n\"\\]*(\\[.\n])*)*"\"" 	{ yylval = strdup(yytext); return STRINGLITERAL; }



%%

/*
int installID() {
	yylex();
	yylval = malloc(sizeof(yytext));
	strncpy(yylval,yytext,yyleng);   // copy string to symbol table
	return ID;		// return token name
}

int installInt() {
	yylex();
	yylval = malloc(sizeof(yytext));
	yylval = atoi(yytext);	// assign int value to symbol table
	return INTEGER;								// return token name
}
*/



