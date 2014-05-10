%{


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

#include "include/y.tab.h"

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
char *exe_out_name;

int yylex(void);
int yyerror(char *);

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
      $$ = (void *) ast_add_internal_node( NULL, current_function, AST_NODE_FUNCTION_LIST, st, cur_scope );
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
      $$ = (void *) ast_add_internal_node( NULL, current_function, AST_NODE_FUNCTION_LIST, st, cur_scope );
    }
  ;

function_def
  : type IDENTIFIER LPAREN arg_list RPAREN statement_list 
    {
      #ifdef PARSER_DEBUG
      fprintf(stderr, "FUNCTION DEF IDENT: %s\n", (char *)$2);
      #endif

      ast_list *body, *arguments;
      heap_list_malloc(hList, body);
      heap_list_malloc(hList, arguments);

      arguments = (ast_list *) $4;
      body->data = (ast *) $6;
      body->next = arguments;
      /*arguments->data = (ast *) $4;
      arguments->next = NULL;*/
      ast_type t = (ast_type) $1;
      symtab_insert(st, $2, t, ST_STATIC_DEC);
      $$ = (void *) ast_add_internal_node( $2, body, AST_NODE_FUNCTION_DEF, st, cur_scope );

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
      symtab_insert(st, $2, t, ST_STATIC_DEC);
      $$ = (void *) ast_add_internal_node( $2, body, AST_NODE_FUNCTION_DEF, st, cur_scope );

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
      $$ = (void *) ast_add_internal_node( $1, children, AST_NODE_FUNCTION_CALL, st, cur_scope );
    }
  | IDENTIFIER LPAREN RPAREN
    {
      fprintf(stderr, "FUNC_CALL 2: NOT YET IMPLEMENTED\n");
    }
  ;

arg_list
  : type IDENTIFIER
    {
      ast_type t = (ast_type) $1;
      symtab_insert(st, $2, t, ST_NONSTATIC_DEC);
      ast *leaf = ast_create_leaf($2, t, st, cur_scope);
      leaf->flag = 1;

      ast_list *arg;
      heap_list_malloc(hList, arg);
      arg->data = leaf;
      arg->next = NULL;

      $$ = (void *) arg;
    }
  | type IDENTIFIER COMMA arg_list
    {
      ast_type t = (ast_type) $1;
      symtab_insert(st, $2, t, ST_NONSTATIC_DEC);
      ast *leaf = ast_create_leaf($2, t, st, cur_scope);
      leaf->flag = 1;

      ast_list *arg;
      heap_list_malloc(hList, arg);
      arg->data = leaf;
      arg->next = (ast_list *) $4;

      $$ = (void *) arg;
    }
  ;

param_list
  : IDENTIFIER
    {
      symtab_entry *s = symtab_lookup( st, $1, cur_scope );
      ast_type t = symtab_entry_get_type(s);
      $$ = (void *) ast_create_leaf ( $1, t, st, cur_scope );
    }
  | param_list COMMA param_list
    {
      ast_list *firstparam;
      heap_list_malloc(hList, firstparam);

      firstparam->data = (ast *) $1;
      firstparam->next = (ast_list *) $3;
      $$ = (void *) firstparam;
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
      ast_list *body, *nextstmt;
      heap_list_malloc(hList, body);
      heap_list_malloc(hList, nextstmt);

      body->data = (ast *) $2;
      body->next = nextstmt;
      nextstmt->data = NULL;
      nextstmt->next = NULL;

      $$ = (void *) ast_add_internal_node(BRACED_STATEMENT_MARKER, body, AST_NODE_STATEMENT, st, cur_scope);
    }
  ;/*| statement
    {
      fprintf(stderr, "SL 2: NOT YET IMPLEMENTED\n");
      /*ast_list *stmt;
      heap_list_malloc(hList, stmt);

      stmt->data = (ast *) $1;
      stmt->next = NULL;
      //$$ = stmt;
      $$ = ast_add_internal_node("statementSL", stmt, AST_NODE_STATEMENT, st, cur_scope);
    }
  ;*/

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

      $$ = (void *) ast_add_internal_node("statementS-SLI", firststmt, AST_NODE_STATEMENT, st, cur_scope);

      #ifdef PARSER_DEBUG
      fprintf(stderr, "STMT-SLI DATA: %p %p %p\n", $1, $2, $$);
      #endif
    }
  | statement
    {
      ast_list *stmt, *nextstmt;
      heap_list_malloc(hList, stmt);
      heap_list_malloc(hList, nextstmt);

      stmt->data = (ast *) $1;
      stmt->next = nextstmt;
      nextstmt->data = NULL;
      nextstmt->next = NULL;

      $$ = (void *) ast_add_internal_node("statements", stmt, AST_NODE_STATEMENT, st, cur_scope);
      
      #ifdef PARSER_DEBUG
      fprintf(stderr, "STMT-S DATA: %p %p %p\n", $1, NULL, $$);
      #endif
    }
  ;

statement
  : loop_statement
    {
      fprintf(stderr, "STATEMENT 1: NOT YET IMPLEMENTED\n");
    }
  | thread_statement
    {
      $$ = $1;
    }
  | conditional_statement
    {
      fprintf(stderr, "STATEMENT 3: NOT YET IMPLEMENTED\n");
    }
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
    {
      fprintf(stderr, "STATEMENT 4: NOT YET IMPLEMENTED\n");
    }
  | SEMI
    {
      fprintf(stderr, "STATEMENT 5: NOT YET IMPLEMENTED\n");
    }
  | statement_list
    {
      $$ = $1;
    }
  ;

return_statement
 : RETURN IDENTIFIER
    {
      ast_list *children;
      heap_list_malloc(hList, children);

      children->data = (ast *) $2;
      $$ = (void *) ast_add_internal_node( "return", children, AST_NODE_FUNCTION_CALL, st, cur_scope );
    }
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
    {
      fprintf(stderr, "LOOP STATEMENT 1: NOT YET IMPLEMENTED\n");
    }
  | for_statement
    {
      fprintf(stderr, "LOOP STATEMENT 2: NOT YET IMPLEMENTED\n");
    }
  | pfor_statement
    {
      fprintf(stderr, "LOOP STATEMENT 3: NOT YET IMPLEMENTED\n");
    }
  ;

thread_statement
  : spawn_statement
    {
      $$ = $1;
    }
  | lock_statement
    {
      fprintf(stderr, "THREAD STATEMENT 1: NOT YET IMPLEMENTED\n");
    }
  | barrier_statement
    {
      $$ = $1;
    }
  ;

conditional_statement
  : IF LPAREN bool_expr RPAREN statement_list
    {
      ast_list *condstmt, *ifbody, *elsebody;
      heap_list_malloc(hList, condstmt);
      heap_list_malloc(hList, ifbody);
      heap_list_malloc(hList, elsebody);

      condstmt->data = (ast *) $3;
      condstmt->next = ifbody;
      ifbody->data = (ast *) $5,
      ifbody->next = NULL;
      elsebody->data = NULL;
      elsebody->next = NULL;

      $$ = (void *) ast_add_internal_node( $1, condstmt, AST_NODE_CONDITIONAL, st, cur_scope );
    }
  | IF LPAREN bool_expr RPAREN statement_list ELSE statement_list
    {
      ast_list *condstmt, *ifbody, *elsebody;
      heap_list_malloc(hList, condstmt);
      heap_list_malloc(hList, ifbody);
      heap_list_malloc(hList, elsebody);

      condstmt->data = (ast *) $3;
      condstmt->next = ifbody;
      ifbody->data = (ast *) $5,
      ifbody->next = elsebody;
      elsebody->data = (ast *) $7;
      elsebody->next = NULL;

      $$ = (void *) ast_add_internal_node( $1, condstmt, AST_NODE_CONDITIONAL, st, cur_scope );
    }
  ;

while_statement
  : WHILE LPAREN bool_expr RPAREN statement_list
    {
      ast_list *condstmt, *whilebody;
      heap_list_malloc(hList, condstmt);
      heap_list_malloc(hList, whilebody);

      condstmt->data = (ast *) $3;
      condstmt->next = whilebody;
      whilebody->data = (ast *) $5,
      whilebody->next = NULL;

      $$ = (void *) ast_add_internal_node( $1, condstmt, AST_NODE_WHILE, st, cur_scope );
    }
  ;

for_statement
  : FOR LPAREN assignment SEMI rel_expr SEMI unary_math RPAREN statement_list
    {
      fprintf(stderr, "FOR STATEMENT 1: NOT YET IMPLEMENTED\n");
    }
  ;

pfor_statement
  : PFOR LPAREN IDENTIFIER COMMA IDENTIFIER COMMA INTEGER RPAREN statement_list
    {
      fprintf(stderr, "PFOR STATEMENT 1: NOT YET IMPLEMENTED\n");
    }
  ;

spawn_statement
  : SPAWN LPAREN IDENTIFIER RPAREN statement_list
    {
      ast_type t = symtab_entry_get_type(symtab_lookup(st, $3, cur_scope));

      ast_list *body;
      ast_list *args;
      heap_list_malloc(hList, body);
      heap_list_malloc(hList, args);

      body->data = (ast *) $5;
      body->next = args;
      args->data = ast_create_leaf( $3, t, st, cur_scope );
      args->next = NULL;

      $$ = (void *) ast_add_internal_node(NULL, body, AST_NODE_SPAWN, st, cur_scope);
    }
  ;

lock_statement
  : LOCK LPAREN param_list RPAREN statement_list
    {
      fprintf(stderr, "LOCK STATEMENT 1: NOT YET IMPLEMENTED\n");
    }
  ;

barrier_statement
  : BARRIER SEMI
    {
      $$ = (void *) ast_add_internal_node(NULL, NULL, AST_NODE_BARRIER, st, cur_scope);
    }
  ;

declaration
  : type IDENTIFIER
    {
      /* add to symtab and then create node */
      ast_type t = (ast_type) $1;
      symtab_insert(st, $2, t, ST_NONSTATIC_DEC);

      /* add to threadtab if necessary */
      if ((ast_type) $$ == AST_THREAD){
        #ifdef PARSER_DEBUG
        fprintf(stderr, "thread declaration\n");
        #endif
        threadtab_insert(tb, create_thread_data($2, 1));
      }

      ast* leaf = ast_create_leaf($2, t, st, cur_scope);

      ast_list *ident;
      heap_list_malloc(hList, ident);
      ident->data = leaf;
      ident->next = NULL;

      $$ = (void *) ast_add_internal_node("declaration", ident, AST_NODE_DECLARATION, st, cur_scope);
    }
  | type IDENTIFIER LBRACK INTEGER RBRACK
    {
      ast_type t;
      int size;
      if ( (ast_type) $1 == AST_CHAR ) {
        t = AST_CHARARRAY;
        size = sizeof(char) * (int) atoi($4);
       } else if ( (ast_type) $1 == AST_INT ) {
        t = AST_INTARRAY;
        size = sizeof(int) * (int) atoi($4);
      }

      symtab_insert(st, $2, t, ST_NONSTATIC_DEC);
      ast* leaf = ast_create_array_leaf($2, size, t, st, cur_scope);

      ast_list *ident;
      heap_list_malloc(hList, ident);
      ident->data = leaf;
      ident->next = NULL;

      $$ = (void *) ast_add_internal_node("declaration", ident, AST_NODE_DECLARATION, st, cur_scope);

    }
  | type IDENTIFIER LBRACK IDENTIFIER LBRACK
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
      $$ = (void *) ast_add_internal_node( $2, lh, AST_NODE_BINARY, st, cur_scope ) ;
      
      #ifdef PARSER_DEBUG
      fprintf(stderr, "BIN NODE DATA: %p %s %p %p\n", $1, (char *)$2, $3, $$);
      #endif
    }
  ;

lvalue 
  : type IDENTIFIER  
    { 
      ast_type t = (ast_type) $1;
      symtab_insert( st, $2, t, ST_NONSTATIC_DEC );
      $$ = (void *) ast_create_leaf( $2, t, st, cur_scope );
      free($2);
    }
  | IDENTIFIER
    {
      ast_type t = symtab_entry_get_type(symtab_lookup(st, $1, cur_scope));
      $$ = (void *) ast_create_leaf( $1, t, st, cur_scope );
    }
  | type array
    {
      fprintf(stderr, "LVALUE 3: NOT YET IMPLEMENTED\n");
    }
  ;

rvalue
  : expr
    {
      $$ = $1;
    }
  ;

expr
  : bool_expr
    {
      fprintf(stderr, "EXPR 1: NOT YET IMPLEMENTED\n");
    }
  | math_expr
    {
      fprintf(stderr, "EXPR 2: NOT YET IMPLEMENTED\n");
    }
  | unary_math
    {
      fprintf(stderr, "EXPR 3: NOT YET IMPLEMENTED\n");
    }
  | func_call
  | braced_expr
    {
      fprintf(stderr, "EXPR 5: NOT YET IMPLEMENTED\n");
    }
  | literal
  | array
    {
      fprintf(stderr, "EXPR 7: NOT YET IMPLEMENTED\n");
    }
  | LPAREN expr RPAREN 
    {
      fprintf(stderr, "EXPR 8: NOT YET IMPLEMENTED\n");
    }
  ;

braced_expr
  : LBRACE literal_list RBRACE
    {
      fprintf(stderr, "BRACED_EXPR 1: NOT YET IMPLEMENTED\n");
    }
  ;

bool_expr
  : rel_expr
    {
      fprintf(stderr, "BOOL EXPR 1: NOT YET IMPLEMENTED\n");
    }
  | IDENTIFIER
   {
      ast_type t = symtab_entry_get_type(symtab_lookup(st, $1, cur_scope));
      $$ = (void *) ast_create_leaf( $1, t, st, cur_scope );
    }
  | BOOLEANOP IDENTIFIER
    {
      fprintf(stderr, "BOOL EXPR 3: NOT YET IMPLEMENTED\n");
    }
  ;

rel_expr
  : expr relop expr
    { 
      ast_list *lh;
      ast_list *rh;
      heap_list_malloc(hList, lh);
      heap_list_malloc(hList, rh);

      lh->data = (ast *) $1;
      lh->next = rh;
      rh->data = (ast *) $3;
      rh->next = NULL;
      $$ = (void *) ast_add_internal_node( $2, lh, AST_NODE_BINARY, st, cur_scope ) ;
    }
  ;

math_expr
  : expr mathop expr
    { 
      ast_list *lh;
      ast_list *rh;
      heap_list_malloc(hList, lh);
      heap_list_malloc(hList, rh);

      lh->data = (ast *) $1;
      lh->next = rh;
      rh->data = (ast *) $3;
      rh->next = NULL;
      $$ = (void *) ast_add_internal_node( $2, lh, AST_NODE_BINARY, st, cur_scope ) ;
    }
  ;

unary_math
  : IDENTIFIER unary_math_op
    {
      ast_list *operand;
      heap_list_malloc(hList, operand);

      operand->data = (ast *) $1;
      operand->next = NULL;

      $$ = (void *) ast_add_internal_node( $1, operand, AST_NODE_UNARY, st, cur_scope );
    }
  ;

assignop
  : ASSIGN { $$ = "="; }
  | PLUSASSIGN { $$ = "+="; }
  | MINUSASSIGN { $$ = "-="; }
  ;

relop
  : EQ { $$ = "=="; }
  | NE { $$ = "!="; }
  | LT { $$ = "<"; }
  | LE { $$ = "<="; }
  | GT { $$ = ">"; }
  | GE { $$ = ">="; }
  ;

mathop
  : PLUS { $$ = "+"; }
  | MINUS { $$ = "-"; }
  | TIMES { $$ = "*"; }
  | OVER { $$ = "/"; }
  | MOD { $$ = "%"; }
  ;

unary_math_op
  : INC { $$ = "++"; }
  | DEC { $$ = "--"; }
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
      $$ = (void *) ast_create_leaf( $1, AST_STRINGLITERAL, st, cur_scope ); 
      free($1);
    }
  ;

number
  : INTEGER
    {
      $$ = (void *) ast_create_leaf( $1, AST_INTLITERAL, st, cur_scope );
      free($1);
    }
  | FLOAT
  ;

array
  : IDENTIFIER LBRACK INTEGER RBRACK
    {

    }
  | IDENTIFIER LBRACK IDENTIFIER RBRACK
  | IDENTIFIER LBRACK INTEGER RBRACK LBRACK INTEGER RBRACK
  | IDENTIFIER LBRACK IDENTIFIER RBRACK LBRACK IDENTIFIER RBRACK
  ;

type
  : INT { $$ = (void *) AST_INT; }
  | DOUBLE
  | CHAR { $$ = (void *) AST_CHAR; }
  | BOOLEAN
  | STRING { $$ = (void *) AST_STRING; }
  | THREAD { $$ = (void *) AST_THREAD; }
  ;

%%


int main( int argc, char *argv[] )
{
  malloc_checked(hList);
  heap_list_init(hList);
  st = symtab_init();
  tb = threadtab_init();
  symtab_set_threadtab(st, tb);

  /* pre-install printOut */
  symtab_insert( st, "printOut", AST_STRING, ST_STATIC_DEC);

  cur_scope = symtab_open_scope(st, SCOPE_FUNCTION);

  strcpy(errmsg,"type error\n");

  if (argc < 2) {
    printf("Usage: ./small-parser <source filename>\n");
    exit(0);
  }

  /* get output file name */
  exe_out_name = malloc_checked_string(64);
  strncpy(exe_out_name, argv[1], sizeof exe_out_name);
  char *extension_ptr = strstr(exe_out_name, ".mf");

  if (extension_ptr == NULL){
    strncpy(exe_out_name, "a.out", 6);
  } else {
    *extension_ptr = '\0';
  }

  /* open input file name */
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

  free(exe_out_name);

  fclose(yyin);

  threadtab_destroy(tb);
  symtab_destroy(st);
  heap_list_purge(hList);
  free(hList);

  return flag;

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
