#ifndef THREADTAB_STRUCTS_H
#define THREADTAB_STRUCTS_H

#include <stdint.h>

#include "global_config.h"
#include "threadtab.h"

struct thread_data {
	char name[MAX_IDENT_LENGTH + 1];
	uint32_t length;
	uint32_t offset;
	struct ast_s *assoc_ast;
	struct thread_data *next;
	unsigned started			:1;
	unsigned completed		:1;
	unsigned							:6;
};

struct threadtab_s{
	struct thread_data *head;
	struct thread_data *tail;
	uint32_t length;
};

#endif
