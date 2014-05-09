%{

#define YYSTYPE char *

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/global_config.h"
#include "include/error_handling.h"
#include "include/mem_manage.h"
#include "include/ast.h"
#include "include/symtab.h"
#include "include/threadtab.h"
#include "include/gen_code.h"
#include "include/yacc_compatability.h"

#define MAX_MSG_LEN 50
#define YYDEBUG 0

//#define PARSER_DEBUG

int errcnt = 0;
char errmsg[40];
extern char *yytext;
extern FILE *yyin;
extern FILE *yyout;
extern int yyparse();
extern int lineno;
int t;
symtab *st;
threadtab *tb;
scope *cur_scope;
heap_list_head *hList;


%}

%token INT DOUBLE THREAD VOID CHAR BOOLEAN STRING
%token INTEGER FLOAT CHARLITERAL STRINGLITERAL
%token IDENTIFIER 
%token BARRIER SPAWN WHILE FOR PFOR IF ELSE LOCK TRUE FALSE RETURN
%token LBRACE RBRACE LBRACK RBRACK LPAREN RPAREN SEMI COMMA 
%right ASSIGN PLUSASSIGN MINUSASSIGN
%left AND OR
%left NE EQ LT LE GT GE
%left BOOLEANOP
%left PLUS MINUS
%left TIMES OVER MOD
%right INC DEC

%start start_point

%%

start_point
  : function_list
    {
      ast *root = (ast *) $1;
			gen_code( root, st, tb );
      ast_destroy(root);
    }
  ;

function_list
  : function_def function_list 
    {
      ast_list *current_function, *next_function;
      heap_list_malloc(hList, current_function);
      heap_list_malloc(hList, next_function);

      current_function->data = (ast *) $1;
      current_function->next = next_function;
      next_function->data = (ast *) $2;
      next_function->next = NULL;
      $$ = ast_add_internal_node( NULL, current_function, AST_NODE_FUNCTION_LIST, st, cur_scope );
    }
  | function_def
    {
      ast_list *current_function, *next_function;
      heap_list_malloc(hList, current_function);
      heap_list_malloc(hList, next_function);

      current_function->data = (ast *) $1;
      current_function->next = next_function;
      next_function->data = NULL;
      next_function->next = NULL;
      $$ = ast_add_internal_node( NULL, current_function, AST_NODE_FUNCTION_LIST, st, cur_scope );
    }
  ;

function_def
  : type IDENTIFIER LPAREN arg_list RPAREN statement_list 
    {
      fprintf(stderr, "FUNC_DEF 1: NOT YET IMPLEMENTED\n");
    }
  | type IDENTIFIER LPAREN RPAREN statement_list 
    {
      #ifdef PARSER_DEBUG
      fprintf(stderr, "FUNCTION DEF IDENT: %s\n", (char *)$2);
      #endif
      
      ast_list *body, *arguments;
      heap_list_malloc(hList, body);
      heap_list_malloc(hList, arguments);

      body->data = (ast *) $5;
      body->next = arguments;
      arguments->data = NULL;
      arguments->next = NULL;
      ast_type t = (ast_type) $1;
      symtab_insert(st, $2, t);
      $$ = ast_add_internal_node( $2, body, AST_NODE_FUNCTION_DEF, st, cur_scope );

    }
  ;

func_call
  : IDENTIFIER LPAREN param_list RPAREN
    { 
      #ifdef PARSER_DEBUG
      fprintf(stderr, "FUNCTION CALL IDENT: %s\n", (char *)$1);
      #endif

      ast_list *children;
      heap_list_malloc(hList, children);

      children->data = (ast *) $3;
      children->next = NULL;
      $$ = ast_add_internal_node( $1, children, AST_NODE_FUNCTION_CALL, st, cur_scope );
    }
  | IDENTIFIER LPAREN RPAREN
    {
      fprintf(stderr, "FUNC_CALL 2: NOT YET IMPLEMENTED\n");
    }
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
      fprintf(stderr, "PARAM_LIST 2: NOT YET IMPLEMENTED\n");
      /*ast_list *firstparam;
      heap_list_malloc(hList, firstparam);

      firstparam->data = (ast *) $1;
      firstparam->next = (ast *) $3;
      $$ = firstparam;*/
    }
  | array
    {
      fprintf(stderr, "PARAM_LIST 3: NOT YET IMPLEMENTED\n");
    }
  | literal
    {
      fprintf(stderr, "PARAM_LIST 4: NOT YET IMPLEMENTED\n");
    }
  ;

statement_list
  : LBRACE statement_list_internal RBRACE 
    {
      $$ = $2;
    }
  | statement
    {
      fprintf(stderr, "SL 2: NOT YET IMPLEMENTED\n");
      /*ast_list *stmt;
      heap_list_malloc(hList, stmt);

      stmt->data = (ast *) $1;
      stmt->next = NULL;
      //$$ = stmt;
      $$ = ast_add_internal_node("statementSL", stmt, AST_NODE_STATEMENT, st, cur_scope);*/
    }
  ;

statement_list_internal
  : statement statement_list_internal
    {
      ast_list *firststmt, *nextstmt;
      heap_list_malloc(hList, firststmt);
      heap_list_malloc(hList, nextstmt);

      firststmt->data = (ast *) $1;
      firststmt->next = nextstmt;
      nextstmt->data = (ast *) $2;
      nextstmt->next = NULL;

      $$ = ast_add_internal_node("statementS-SLI", firststmt, AST_NODE_STATEMENT, st, cur_scope);

      #ifdef PARSER_DEBUG
      fprintf(stderr, "STMT-SLI DATA: %p %p %p\n", $1, $2, $$);
      #endif
    }
  | statement
    {
      //$$ = $1;
      ast_list *stmt, *nextstmt;
      heap_list_malloc(hList, stmt);
      heap_list_malloc(hList, nextstmt);

      stmt->data = (ast *) $1;
      stmt->next = nextstmt;
      nextstmt->data = NULL;
      nextstmt->next = NULL;

      $$ = ast_add_internal_node("statementS", stmt, AST_NODE_STATEMENT, st, cur_scope);
      
      #ifdef PARSER_DEBUG
      fprintf(stderr, "STMT-S DATA: %p %p %p\n", $1, NULL, $$);
      #endif
    }
  ;

statement
  : loop_statement
  | thread_statement
  | conditional_statement
  | declaration SEMI
    {
      $$ = $1;
    }
  | assignment SEMI
    {
      $$ = $1;
    }
  | expr SEMI
    {
      $$ = $1;
    }
  | return_statement SEMI
  | SEMI
  ;

return_statement
 : RETURN IDENTIFIER
 | RETURN literal 
    { 
      fprintf(stderr, "RETURN 2: NOT YET IMPLEMENTED\n");
      /*ast_list *children;
      heap_list_malloc(hList, children);

      children->data = (ast *) $2;
      $$ = ast_add_internal_node( "return", children, AST_NODE_FUNCTION_CALL, st, cur_scope );
      */
    }
 ;

loop_statement
  : while_statement
  | for_statement
  | pfor_statement
  ;

thread_statement
  : spawn_statement
    {
      $$ = $1;
    }
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
    {

    }
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
      /* add to symtab and then create node */
      ast_type t = (ast_type) $1;
      symtab_insert(st, $2, t);

      /* add to threadtab if necessary */
      if ((ast_type) $$ == AST_THREAD){
        threadtab_insert(tb, create_thread_data($2, 1));
      }

      ast* leaf = ast_create_leaf($2, t, st, cur_scope);

      ast_list *ident;
      heap_list_malloc(hList, ident);
      ident->data = leaf;
      ident->next = NULL;

      $$ = ast_add_internal_node("declaration", ident, AST_NODE_DECLARATION, st, cur_scope);
    }
  | type array
  ; 

assignment
  : lvalue assignop rvalue 
    { 
      ast_list *lh;
      ast_list *rh;
      heap_list_malloc(hList, lh);
      heap_list_malloc(hList, rh);

      lh->data = (ast *) $1;
      lh->next = rh;
      rh->data = (ast *) $3;
      rh->next = NULL;
      $$ = ast_add_internal_node( $2, lh, AST_NODE_BINARY, st, cur_scope ) ;
      
      #ifdef PARSER_DEBUG
      fprintf(stderr, "BIN NODE DATA: %p %s %p %p\n", $1, (char *)$2, $3, $$);
      #endif
    }
  ;

lvalue 
  : type IDENTIFIER  
    { 
      ast_type t = (ast_type) $1;
      symtab_insert( st, $2, t );
      $$ = ast_create_leaf( $2, t, st, cur_scope );
      free($2);
    }
  | IDENTIFIER
    {
      ast_type t = symtab_entry_get_type(symtab_lookup(st, $1, cur_scope));
      $$ = ast_create_leaf( $1, t, st, cur_scope );
    }
  | type array
  ;

rvalue
  : expr
    {
      $$ = $1;
    }
  ;

expr
  : bool_expr
  | math_expr
  | unary_math
  | func_call
    {
      $$ = $1;
    }
  | braced_expr
  | literal
    {
      $$ = $1;
    }
  /*| IDENTIFIER*/
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
  : ASSIGN { $$ = "="; }
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
  malloc_checked(hList);
  heap_list_init(hList);
  st = symtab_init();
  tb = threadtab_init();

  /* pre-install printOut */
  symtab_insert( st, "printOut", AST_STRING);

  cur_scope = symtab_open_scope(st, SCOPE_FUNCTION);

  strcpy(errmsg,"type error\n");

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

  /*fp = fopen("dump.symtab","w");
  if (!fp) {
    printf("Unable to open file for writing\n");
    exit(0);
  }*/

  int flag = yyparse();

  #ifdef PARSER_DEBUG
  fprintf(stderr,"END OF PARSER\n");
  #endif

  fclose(yyin);

  threadtab_destroy(tb);
  symtab_destroy(st);
  heap_list_purge(hList);
  free(hList);

  return flag;

}

void printTree( ast *root )
{
   

}

int yywrap()
{
   return(1);
}

int yyerror(char * msg)
{ 
  fprintf(stderr,"%s \n",msg);
  return 0;
}
