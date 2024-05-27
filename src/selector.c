/*
 * selector.c
 */

#include <stdio.h>

#include "selector.h"

void selector_print(Selector *sel)
{
	if (sel->flags & SELECTOR_NOT)
		printf("!");

	if (sel->flags & SELECTOR_TRUMP)
		printf("+");

	if (sel->flags & SELECTOR_SELF)
		printf("_");

	switch (sel->type) {
	case SELECTOR_NONE:
		printf("(none)");
		return;

	case SELECTOR_LIT:
		printf("\"%s\"", sel->literal);
		goto range;

	case SELECTOR_WORD:
		printf(".");
		break;

	case SELECTOR_TAG:
		printf("#");
		break;

	case SELECTOR_GNZ:
		printf("^");
		break;

	case SELECTOR_SYN:
		printf("~");
		break;

	case SELECTOR_ANT:
		printf("`");
		break;

	case SELECTOR_MARK:
		printf("@%d", sel->mark);
		goto range;

	case SELECTOR_REF:
		printf("<%d>", sel->ref);
		goto range;

	case SELECTOR_OR:
		printf("[");
		for (int i = 0; sel->children[i]; i++) {
			selector_print(selectors + sel->children[i]);
			printf(", ");
		}
		printf("]");
		goto range;

	case SELECTOR_CROSS:
		printf("(");
		for (int i = 0; sel->children[i]; i++) {
			selector_print(selectors + sel->children[i]);
			printf(", ");
		}
		printf(")");
		goto range;

	case SELECTOR_REGEXP:
		regexp_print(sel->regexp);
		goto range;

	case SELECTOR_UNREC:
		printf("?");
		goto range;

	case SELECTOR_FORM:
		form_print(sel->form);
		goto range;

	case SELECTOR_ROLE:
		printf("*%d", sel->role);
		goto range;

	case SELECTOR_LABEL:
		printf("$%d", sel->label);
		goto range;

	default:
		printf("CORRUPTED");
		return;
	}

	word_print(words + sel->id);
range:
	if (sel->start == 0) {
		if (sel->range != 1)
			printf("/%d", sel->range);
	} else {
		printf("/%d/%d", sel->start, sel->range);
	}
}
