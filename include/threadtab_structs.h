#ifndef THREADTAB_STRUCTS_H
#define THREADTAB_STRUCTS_H

#include <stdint.h>

#include "global_config.h"
#include "threadtab.h"

struct thread_data {
	char name[MAX_IDENT_LENGTH + 1];
	uint64_t length;
	uint64_t offset;
	struct thread_data *next;
};

struct threadtab_s{
	struct thread_data *head;
	struct thread_data *tail;
	uint64_t length;
};

#endif
