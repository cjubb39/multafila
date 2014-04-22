%{

#define YYSTYPE char *

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global_config.h"
#include "error_handling.h"
#include "ast.h"
#include "symtab.h"

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
symtab *st;
scope *cur_scope;

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
  : type IDENTIFIER LPAREN arg_list RPAREN statement_list 
    {

    }
  | type IDENTIFIER LPAREN RPAREN statement_list 
    {
      ast_list children;
      children.data = $5;
      ast_type t = (int) $1;
      symtab_insert(st, $2, t);
      $$ = ast_add_internal_node( $2, &children, AST_NODE_FUNCTION_DEF, st, cur_scope );

    }
  ;

func_call
  : IDENTIFIER LPAREN param_list RPAREN
  | IDENTIFIER LPAREN RPAREN
  | PRINT LPAREN param_list RPAREN 
    { 
      ast_list children;
      children.data = $3;
      $$ = ast_add_internal_node( "printOut", &children, AST_NODE_FUNCTION_CALL, st, cur_scope );
    }
  | READ LPAREN RPAREN
  ;

arg_list
  : type IDENTIFIER
  | type IDENTIFIER COMMA arg_list
  ;

param_list
  : IDENTIFIER
    {
      symtab_entry *s = symtab_lookup( st, $1, cur_scope );
      ast_type t = symtab_entry_get_type(s);
      $$ = ast_create_leaf ( $1, t, st, cur_scope );
    }
  | param_list COMMA param_list
    {
      ast_list firstparam;
      firstparam.data = $1;
      firstparam.next = $3;
    }
  | array
  | literal
  ;

statement_list
  : LBRACE statement_list_internal RBRACE 
  | statement
  ;

statement_list_internal
  : statement statement_list_internal
    {
      ast_list firststmt;
      firststmt.data = $1;
      firststmt.next = $2;
    }
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
 | RETURN literal 
    { 
      
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
    { 
      symtab_insert(st, $2, (int) $1);
      $$ = ast_create_leaf( $2, (int) $1, st, cur_scope );
    }
  | type array
  ;

assignment
  : lvalue assignop rvalue 
    { 
      ast_list lh;
      ast_list rh;
      lh.data = $1;
      lh.next = &rh;
      rh.data = $3;
      rh.next = NULL;
      $$ = ast_add_internal_node( $2, &lh, AST_NODE_BINARY, st, cur_scope ) ;
    }
  ;

lvalue
  : type IDENTIFIER  
    { 
      symtab_insert( st, $2, (int) $1 );
      $$ = ast_create_leaf( $2, (int) $1, st, cur_scope );
      free($2);
    }
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
  : ASSIGN { $$ = '='; }
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
  | STRINGLITERAL 
    { 
      $$ = ast_create_leaf( $1, AST_STRINGLITERAL, st, cur_scope ); 
      free($1);
    }
  ;

number
  : INTEGER
  | FLOAT
  ;

array
  : IDENTIFIER LBRACK INTEGER RBRACK
  | IDENTIFIER LBRACK IDENTIFIER RBRACK
  | IDENTIFIER LBRACK INTEGER RBRACK LBRACK INTEGER RBRACK
  | IDENTIFIER LBRACK IDENTIFIER RBRACK LBRACK IDENTIFIER RBRACK
  ;

type
  : INT { $$ = AST_INT; }
  | DOUBLE 
  | CHAR
  | BOOLEAN
  | STRING { $$ = AST_STRING; }
  | THREAD
  ;



%%


int main( int argc, char *argv[] )
{
  st = symtab_init();

  /* pre-install printOut */
  symtab_insert( st, "printOut", AST_STRING);

  cur_scope = symtab_open_scope(st, SCOPE_FUNCTION);

  strcpy(errmsg,"type error\n");
  int i;

  if (argc < 2) {
    printf("Usage: ./small-parser <source filename>\n");
    exit(0);
  }

  FILE *fp = fopen(argv[1],"r");
  if (!fp) {
    printf("Unable to open file for reading\n");
    exit(0);
  }

  yyin = fp;

  fp = fopen("dump.symtab","w");
  if (!fp) {
    printf("Unable to open file for writing\n");
    exit(0);
  }

  int flag = yyparse();

  fclose(fp);
  fclose(yyin);

  return flag;

}




yywrap()
{
   return(1);
}

int yyerror(char * msg)
{ 
  fprintf(stderr,"%s \n",msg);
  return 0;
}
