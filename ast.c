ast *ast_create_leaf (char *value, ast_type type) {
  ast *a;
  if ( ( malloc(sizeof(ast)) ) == NULL ) 
    yyerror("out of memory");

  a->leaf_type = type;
 
  if ( type == "STRINGLITERAL" || type == "string" ) {
    a->x.string = value;
  }
  printf("created node with value: %s\n", value);
  return a;
}

ast *ast_add_internal_node (struct ast_list *children, ast_node_type type) {
  ast *a;
  if ( ( malloc(sizeof(ast)) ) == NULL )
    yyerror("out of memory");

  a->node_type = type;

  if ( type == "ASSIGN" ) {
    a->x.binary.oper = "=";
    a->x.binary.left = children;
    a->x.binary.right = children->next;
    printf("created node with values: %s, %s, %s\n", a->x.binary.oper);
  }
  return a;
}