/*
 * branch.h
 */

#ifndef EMULOS_BRANCH_H_
#define EMULOS_BRANCH_H_

#include "action.h"

typedef struct branch Branch;

struct branch {
	int index;

	int bc, ac;
	Branch *children;
	int *act_ids;
};

extern Branch root;
extern Action *actions[];

void branch_print(Branch *branch, int indent);

#endif
