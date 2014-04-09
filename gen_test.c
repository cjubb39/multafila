#include <stdio.h>
#include <assert.h>

#include "global_config.h"
#include "error_handling.h"
#include "ast.h"
#include "symtab.h"

#include "ast_structs.h"
#include "symtab_structs.h"

static FILE *fp = NULL;

void print_ast(ast *a);

void print_header(){
	fprintf(fp, "#include <stdio.h>\nvoid printOut(char *m){printf(\"%%s\\n\", m);}\n\n");
}


void print_ast_type(ast_type at){
	switch(at){
		case AST_STRING:
			fprintf(fp, "char *");
			break;

		case AST_INT:
			fprintf(fp, "int");
			break;

		default:
			break;
	}
}


void print_func_def(ast *a){
	print_ast_type(symtab_entry_get_type(a->data.func_def.func_symtab));
	fprintf(fp, " %s ( ", a->data.func_def.func_symtab->name);

	struct ast_list_s *tmp = a->data.func_def.arguments;
	while (tmp != NULL){
		print_ast(tmp->data);
		tmp = tmp->next;

		if (tmp != NULL)
			fprintf(fp, ", ");
	}

	fprintf(fp, " )\n{\n");
	print_ast(a->data.func_def.body);
	fprintf(fp, "\n}\n");

	print_ast(a->data.func_def.next_function);
}

void print_func_call(ast *a){
	fprintf(fp, "%s( ", a->data.func_call.func_symtab->name);

	struct ast_list_s *tmp = a->data.func_call.arguments;
	while (tmp != NULL){
		print_ast(tmp->data);
		tmp = tmp->next;
		
		if (tmp != NULL)
			fprintf(fp, ", ");
	}
	fprintf(fp, " )");
}

void print_dec(ast *a){
	print_ast_type(a->type);
	fprintf(fp, "  ");
	print_ast(a->data.dec.var);
}

void print_bin(ast *a){
	print_ast(a->data.bin.left);
	fprintf(fp, " %s ", a->data.bin.op);
	print_ast(a->data.bin.right);
}

void print_stmt(ast *a){
	print_ast(a->data.stmt.body);
	fprintf(fp, " ;\n");
	print_ast(a->data.stmt.next);
}

void print_leaf(ast *a){
	switch(a->type){
		case AST_STRINGLITERAL:
			fprintf(fp, "\"%s\"", a->data.string);
			break;

		default:
			fprintf(fp, "%s", a->data.symtab_ptr->name);
			break;
	}
}

void print_ast(ast *a){
	if (a == NULL){
		return;
	}

	switch(ast_get_node_type(a)){
		case AST_NODE_FUNCTION_DEF:
			print_func_def(a);
			break;

		case AST_NODE_FUNCTION_CALL:
			print_func_call(a);
			break;

		case AST_NODE_DECLARATION:
			print_dec(a);
			break;

		case AST_NODE_BINARY:
			print_bin(a);
			break;

		case AST_NODE_STATEMENT:
			print_stmt(a);
			break;

		case AST_NODE_LEAF:
			print_leaf(a);
			break;

		default:
			assert(0 == 1);
			break;
	}
}

void gen_test(ast *a){
	fp = fopen("out.c", "w");
	print_header();

	print_ast(a);

	fclose(fp);
}
