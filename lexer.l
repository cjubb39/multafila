
/*
 *	Lexer for multafila, a language focusing on making multithreading easy 
 *	and intuitive.
 *	
 *	<authors>
 *	
 */

%{

	#include <stdio.h>
	#include <string.h>
	#include "include/global_config.h"
	#include "include/y.tab.h"
	#include "include/symtab.h"

	#define YY_NO_INPUT

	extern symtab *st;
	extern scope *cur_scope;

%}

%option nounput
%option yylineno

delim 			[ \t\n]
ws    			{delim}+
letter			[A-Za-z]
non-zero		[1-9]
digit			[0-9]

id 				{letter}({letter}|{digit}|[_])*
number			[-]?{non-zero}{digit}*|[0]
float 			[-]?{non-zero}{digit}*\.{digit}+|[-]?[0\.{digit}+]


%%

{ws}			{  }

"/*"((("*"[^/])?)|[^*])*"*/" { } /* comments */
"//"[^\n]*\n 	{  }

"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"for"			{ return FOR; }
"pfor"			{ return PFOR; }
"spawn"			{ return SPAWN; }
"barrier"		{ return BARRIER; }
"return"		{ return RETURN; }
"lock"			{ return LOCK; }

"int"			{ return INT; }
"double"		{ return DOUBLE; }
"char"			{ return CHAR; }
"boolean"		{ return BOOLEAN; }
"string"		{ return STRING; }
"thread"		{ return THREAD; }
"void"			{ return VOID; }

"true"			{ return TRUE; }
"false"			{ return FALSE; }

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

"{"             { 
									cur_scope = symtab_open_scope(st, SCOPE_NORMAL);
					 				return LBRACE;
					 			}

"}"             { 
									cur_scope = symtab_close_scope(st);
					 				return RBRACE;
					 			}
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
