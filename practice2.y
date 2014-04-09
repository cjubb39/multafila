%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

#define DEBUG 0
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

%token INT DOUBLE THREAD VOID CHAR BOOLEAN STRING
%token INTEGER FLOAT CHARLITERAL STRINGLITERAL
%token IDENTIFIER 
%token BARRIER SPAWN WHILE FOR PFOR IF ELSE LOCK TRUE FALSE RETURN PRINT READ
%token LBRACE RBRACE LBRACK RBRACK LPAREN RPAREN SEMI COMMA 
%right ASSIGN PLUSASSIGN MINUSASSIGN
%left AND OR
%left NE EQ LT LE GT GE
%left BOOLEANOP
%left PLUS MINUS
%left TIMES OVER MOD
%right INC DEC

%start function_list

%%

function_list
  : function_def function_list 
  | function_def
  ;

function_def
  : type IDENTIFIER LPAREN arg_list RPAREN statement_list {
      struct ast_list *child_list;
      child_list->data = $4;
      child_list->data->next = $5;
      ast_add_internal_node ( child_list, $2 );
    }
  | type IDENTIFIER LPAREN RPAREN statement_list {
      struct ast_list *child_list;
      child_list->data = $5;
      ast_add_internal_node ( child_list, $2 );
    }
  ;

func_call
  : IDENTIFIER LPAREN param_list RPAREN
  | IDENTIFIER LPAREN RPAREN
  | PRINT LPAREN param_list RPAREN { 
      struct ast_list *child_list;
      child_list->data = $3;
      ast_add_internal_node ( child_list, $1 );
    }
  | READ LPAREN RPAREN
  ;

arg_list
  : type IDENTIFIER
  | type IDENTIFIER COMMA arg_list
  ;

param_list
  : IDENTIFIER
  | param_list COMMA param_list
  | array
  | literal
  ;

statement_list
  : LBRACE statement_list_internal RBRACE 
  | statement
  ;

statement_list_internal
  : statement statement_list_internal
  | statement
  ;

statement
  : loop_statement
  | thread_statement
  | conditional_statement
  | declaration SEMI
  | assignment SEMI
  | expr SEMI
  | return_statement SEMI
  | SEMI
  ;

return_statement
 : RETURN IDENTIFIER
 | RETURN literal { 
      struct ast_list* child_list;
      child_list->data = $2;
      $$ = ast_add_internal_node( child_list, $1 ) 
    }
 ;

loop_statement
  : while_statement
  | for_statement
  | pfor_statement
  ;

thread_statement
  : spawn_statement
  | lock_statement
  | barrier_statement
  ;

conditional_statement
  : IF LPAREN bool_expr RPAREN statement_list
  | IF LPAREN bool_expr RPAREN statement_list ELSE statement_list
  ;

while_statement
  : WHILE LPAREN bool_expr RPAREN statement_list
  ;

for_statement
  : FOR LPAREN assignment SEMI rel_expr SEMI unary_math RPAREN statement_list
  ;

pfor_statement
  : PFOR LPAREN IDENTIFIER COMMA IDENTIFIER COMMA INTEGER RPAREN statement_list
  ;

spawn_statement
  : SPAWN LPAREN IDENTIFIER RPAREN statement_list
  ;

lock_statement
  : LOCK LPAREN param_list RPAREN statement_list
  ;

barrier_statement
  : BARRIER SEMI
  ;

declaration
  : type IDENTIFIER 
  | type array
  ;

assignment
  : lvalue assignop rvalue { 
      struct ast_list *child_list;
      child_list->data = $1;
      child_list->next->data = $3;
      $$ = ast_add_internal_node( child_list, $2 ) ;
    }
  ;

lvalue
  : type IDENTIFIER  { $$ = ast_create_leaf( $2, $1 ); }
  | IDENTIFIER
  | type array
  ;

rvalue
  : expr
  ;

expr
  : bool_expr
  | math_expr
  | unary_math
  | func_call
  | braced_expr
  | literal
  | IDENTIFIER
  | array
  | LPAREN expr RPAREN 
  ;

braced_expr
  : LBRACE literal_list RBRACE
  ;

bool_expr
  : rel_expr;
  | IDENTIFIER;
  | BOOLEANOP IDENTIFIER;
  ;

rel_expr
  : expr relop expr
  ;

math_expr
  : expr mathop expr;
  ;

unary_math
  : IDENTIFIER unary_math_op
  ;

assignop
  : ASSIGN
  | PLUSASSIGN
  | MINUSASSIGN
  ;

relop
  : EQ
  | NE
  | LT
  | LE
  | GT
  | GE
  ;

mathop
  : PLUS
  | MINUS
  | TIMES
  | OVER
  | MOD
  ;

unary_math_op
  : INC
  | DEC
  ;

literal_list
  : literal
  | literal COMMA literal_list
  ;

literal
  : CHARLITERAL
  | number
  | TRUE
  | FALSE
  | STRINGLITERAL { $$ = ast_create_leaf( $1, "STRINGLITERAL" ); }
  ;

number
  : INTEGER ( $$ = ast_create_leaf ( $1, "INTEGERLITERAL" ); )
  | FLOAT
  ;

array
  : IDENTIFIER LBRACK INTEGER RBRACK
  | IDENTIFIER LBRACK IDENTIFIER RBRACK
  | IDENTIFIER LBRACK INTEGER RBRACK LBRACK INTEGER RBRACK
  | IDENTIFIER LBRACK IDENTIFIER RBRACK LBRACK IDENTIFIER RBRACK
  ;

type
  : INT
  | DOUBLE 
  | CHAR
  | BOOLEAN
  | STRING
  | THREAD
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
