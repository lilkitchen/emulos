/*
 * regexp.c
 */

#include <stdio.h>

#include "brain.h"
#include "frame.h"
#include "regexp.h"

#define STACK_LIMIT 16

#define TARGET_SET(m) \
	target = item_add(brain->items, m, 0)

Item *regexp_apply(Regexp *regexp, Brain *brain, Message *msg, Cache *cache)
{
	if (msg->len < regexp->min_len)
		return NULL;

	int stack[STACK_LIMIT][2] = {0};
	int sp = 0;
	if (regexp->flags & REGEXP_START)
		stack[0][0] = msg->len;

	Item *target;
	TARGET_SET(msg);

	int i = 0;
	int k = 0;
	goto next;

step:
	if (stack[sp][0] == msg->len)
		goto pop;

	i = stack[sp][0]++;
	k = stack[sp][1];
	if (k == 0)
		TARGET_SET(msg + i);
next:
	if (k == regexp->len) {
		if (regexp->flags & REGEXP_END && i != msg->len)
			goto step;
		return target;
	}

	if (i == regexp->len) {
pop:
		if (sp == 0)
			return NULL;
		sp--;
		goto step;
	}

#define GET_REF(index) \
	if (cache[index].est == 0) \
		brain_poll(brain, index); \
	Item *ref = cache[index].item

	Pattern *pat = regexp->data + k;
	switch (pat->type) {
	case PATTERN_REF:
		GET_REF(pat->index);
		if (item_is_match(ref, msg + i) == 0)
			goto step;
	case PATTERN_ONE:
		break;

	case PATTERN_TARGET:
		TARGET_SET(msg + i);
		break;

	case PATTERN_ANY:
		sp++;
		stack[sp][0] = i;
		stack[sp][1] = ++k;
		goto next;

	case PATTERN_OR:
		for (int j = 0; j < pat->len; j++) {
			GET_REF(pat->indices[j]);
			if (item_is_match(ref, msg + i))
				goto match;
		}
		goto step;
	}

match:
	i++;
	k++;
	goto next;
}

void regexp_print(Regexp *regexp)
{
	printf("'");

	if (regexp->flags & REGEXP_START)
		printf("^ ");

	for (int i = 0; i < regexp->len; i++) {
		Pattern *pat = regexp->data + 1;
		switch (pat->type) {
		case PATTERN_REF:
			printf("%d ", pat->index);
			break;

		case PATTERN_ONE:
			printf(". ");
			break;

		case PATTERN_ANY:
			printf("* ");
			break;

		case PATTERN_OR:
			for (int k = 0; k < pat->len; k++)
				printf("%d|", pat->indices[i]);
			printf(" ");
			break;

		case PATTERN_TARGET:
			printf("? ");
			break;
		}
	}

	if (regexp->flags & REGEXP_END)
		printf("$");

	printf("'");
}
