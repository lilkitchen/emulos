/*
 * branch.c
 */

#include <stdio.h>

#include "selector.h"
#include "branch.h"

#define TAB() \
	for (int i = 0; i < indent; i++) \
		printf("    ")

void branch_print(Branch *branch, int indent)
{
	selector_print(selectors + branch->index);
	printf("\n");

	indent++;

	for (int i = 0; i < branch->ac; i++) {
		TAB();
		action_print(actions[branch->act_ids[i]]);
		printf("\n");
	}

	for (int i = 0; i < branch->bc; i++) {
		TAB();
		branch_print(branch->children + i, indent);
	}
}
