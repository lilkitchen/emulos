/*
 * los.c
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "emulos.h"
#include "parse.h"
#include "rng.h"
#include "selector.h"
#include "utf8.h"
#include "los.h"

Los *los_create(void)
{
	Los *los = malloc(sizeof(Los));
	if (los == NULL)
		emulos_fail(OUT_OF_MEMORY, "Los creation fail\n");
	memset(los, 0, sizeof(Los));

	brain_init(&los->brain);

	return los;
}

void los_destroy(Los *los)
{
	brain_quit(&los->brain);

	free(los);
}

#define OUTPUT(c) \
	do { \
		output[p++] = c; \
		if (p == LOS_REPLY_LIMIT) \
			return 1; \
	} while (0)

static int los_output(Los *los, Action *action)
{
	Form pg = FORM_FEMININE * (los->brain.flags & BRAIN_PARTNER_FEMALE);

	WordForm wf[ACTION_LEN_LIMIT] = {0};

	char *output = los->reply;
	int p = 0;

	for (int i = 0; i < action->len; i++) {
		if (action[i].type == 0)
			continue;

		if (wf[i].str == NULL)
			if (action_apply(action, i, wf, pg, los->brain.cache))
				return 1;

		for (char *s = wf[i].str; *s; s++) {
			if (*s == '@') {
				if (pg)
					for (char *c = "а"; *c; c++)
						OUTPUT(*c);
				continue;
			}
			OUTPUT(*s);
		}
	}

	output[p] = '\0';
	return 0;
}

int los_tell(Los *los, char *str)
{
	Brain *brain = &los->brain;

	if (str[0] == '\0')
		return 1;

#ifdef DEBUG
	if (strcmp(str, "print") == 0) {
		for (int i = 0; i < root.bc; i++)
			branch_print(root.children + i, 0);
		return 1;
	}
#endif
	brain_wash(brain);

	if (brain_read(brain, str))
		return 1;

	int longest = 0, trumps = 0, trumpiest = 0;
	int op = 0;
	Action **options = los->options;

	Frame *frames = los->frames;
	memset(frames, 0, sizeof(los->frames));
	frames[0].br = &root;
	int fp = 0;
think:;
	Branch *target = frames[fp].br->children + frames[fp].bp++;
	int index = target->index;
	Selector *tsel = selectors + index;

	if (tsel->flags & SELECTOR_TRUMP)
		trumps++;

	fp++;
	frames[fp].br = target;

	if (brain_poll(brain, index))
		goto pop;

	/* Actions */
	if (target->ac == 0)
		goto check;

	if (trumps) {
		if (trumps < trumpiest)
			goto check;
		op = 0;
		trumpiest = trumps;

	} else if (trumpiest) {
		goto check;

	} else if (fp > longest) {
		longest = fp;
		op = 0;

	} else if (fp < longest) {
		goto check;
	}

	for (int i = 0; i < target->ac; i++)
		options[op++] = actions[target->act_ids[i]];

check:
	if (frames[fp].bp == frames[fp].br->bc) {
pop:
		if (fp == 0)
			goto reply;

		tsel = selectors + frames[fp].br->index;
		if (tsel->flags & SELECTOR_TRUMP)
			trumps--;

		frames[fp] = (Frame){0};
		fp--;

		goto check;
	}
	goto think;

reply:;
	int tries = 0;
retry:
	memset(los->reply, 0, sizeof(los->reply));

	if (op == tries)
		return 1;

	Action *choice = options[rng_int(op)];
	if (los_output(los, choice)) {
#ifdef DEBUG
		los->reply[50] = '\0';
		printf("Failed: >>%s<<\n", los->reply);
#endif
		tries++;
		goto retry;
	}

	if (brain_react(brain, choice, los->reply))
		return 1;

	if (los->reply[0] == '\0')
		return 1;

	utf8_upperc(los->reply);
	return 0;
}
