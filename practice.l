%{

	#include <stdio.h>
	#include <string.h>
	#include "y.tab.h"

%}

delim 			[ \t\n]
ws    			{delim}+
letter			[A-Za-z]
digit			[0-9]

id 				{letter}({letter}|{digit})*
number			{digit}+

%%

{ws}			{  }

"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"for"			{ return FOR; }
"pfor"			{ return PFOR; }
"lock" 			{ return LOCK; }

"int"			{ return INTEGER; }
"*"{id}			{ return POINTER; }

"="             { return ASSIGN; }
"!="            { return NE; }
"=="            { return EQ; }
"<"             { return LT; }
"<="            { return LE; }
">"             { return GT; }
">="            { return GE; }
"&&"            { return AND; }
"||"            { return OR; }

"+"             { return PLUS;}
"-"             { return MINUS;}
"*"             { return TIMES;}
"/"             { return OVER;}

"{"             { return LBRACE; }
"}"             { return RBRACE; }
"["             { return LBRACK; }
"]"             { return RBRACK; }
"("             { return LPAREN; }
")"             { return RPAREN; }
";"             { return SEMI; }
","             { return COMMA; }

{id}			{ return(ID); }
{number} 		{ return(NUMBER); }

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




