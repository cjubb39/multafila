%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0
#define TRUE 1
#define FALSE 0
#define MAX_MSG_LEN 50
#define YYDEBUG 1

int errcnt = 0;
char errmsg[40];
extern char *yytext;
extern FILE *yyin;
extern FILE *yyout;
extern int yyparse();
extern int lineno;
int yydebug = 1;
int t;
%}


%token IF THEN ELSE WHILE FOR PFOR LOCK 
%token ASSIGN NE EQ LT LE GT GE AND OR 
%token PLUS MINUS TIMES OVER MOD
%token LBRACE RBRACE LBRACK RBRACK LPAREN RPAREN SEMI COMMA 
%token ID NUMBER INTEGER POINTER

%start function_list

%%
type
  : INTEGER
  | POINTER
  ;

identifier
  : ID
  ;

relop
  : EQ
  | NE
  | LT
  | LE
  | GE
  | GT
;

logical_op
  : AND
  | OR
;

para
  : type identifier
  | identifier
  ;

para_list
  : para
  | para COMMA para_list
;

entry
  : NUMBER
  | NUMBER COMMA entry
;

entry_list
  : LBRACE RBRACE
  | LBRACE entry RBRACE
;

expression
  : NUMBER
  | identifier
  | NUMBER PLUS expression
  | identifier PLUS expression
  ;

assignment
  : identifier ASSIGN expression SEMI
  | identifier LBRACK expression RBRACK ASSIGN expression SEMI
  | identifier ASSIGN identifier LPAREN para_list RPAREN SEMI
;

declaration
  : type identifier SEMI
  | type identifier ASSIGN expression SEMI
  | type identifier LBRACK expression RBRACK SEMI
  | type identifier LBRACK expression RBRACK ASSIGN entry_list SEMI
  | type identifier ASSIGN identifier LPAREN para_list RPAREN SEMI
  ;

cond_exp
  : type identifier relop expression
  | expression relop expression
;

cond_explist
  : cond_exp
  | cond_exp logical_op cond_exp
;

cond_stmt
  : IF LPAREN cond_explist RPAREN LBRACE stmt_list RBRACE
  | IF LPAREN cond_explist RPAREN LBRACE stmt_list RBRACE ELSE LBRACE stmt_list RBRACE
  ;

loop_stmt
  : WHILE LPAREN cond_explist RPAREN LBRACE stmt_list RBRACE
  | FOR LPAREN declaration SEMI cond_explist SEMI assignment RPAREN LBRACE stmt_list RBRACE 
  | FOR LPAREN assignment SEMI cond_explist SEMI assignment RPAREN LBRACE stmt_list RBRACE 
  | PFOR LPAREN identifier COMMA identifier COMMA expression RPAREN LBRACE stmt_list RBRACE
;

lock_stmt
  : LOCK LPAREN identifier RPAREN LBRACE stmt_list RBRACE
  ;

stmt
  : declaration
  | assignment
  | cond_stmt
  | loop_stmt
  | lock_stmt
  ;

stmt_list
  : stmt
  | stmt stmt_list
  ;

function
  : type identifier LPAREN para_list RPAREN LBRACE stmt_list RBRACE
  | type identifier LPAREN RPAREN LBRACE stmt_list RBRACE
;

function_list
  : function
  | function function_list
  ;

%%

int main( int argc,char *argv[] )
{


   strcpy(errmsg,"type error\n");
   int i;
   if(argc < 2) {
      printf("Usage: ./cfc <source filename>\n");
      exit(0);
   }
   FILE *fp = fopen(argv[1],"r");
   if(!fp) {
     printf("Unable to open file for reading\n");
     exit(0);
   }
   yyin = fp;

   fp = fopen("dump.symtab","w");
   if(!fp) {
     printf("Unable to open file for writing\n");
     exit(0);
   }

   int flag = yyparse();

   /* 
   symtab_dump(fp);  
   lineno--; 
   printf("\nsemantic error cnt: %d \tlines of code: %d\n",errcnt,lineno);
   */

   /* cleanup */
   fclose(fp);
   fclose(yyin);

   return flag;
}


yywrap()
{
   return(1);
}

int yyerror(char * msg)
{ fprintf(stderr,"%s \n",msg);
  return 0;
}