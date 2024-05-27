/*
 * action.c
 */

#include <stdio.h>

#include "cache.h"
#include "emulos.h"
#include "rng.h"
#include "selector.h"
#include "action.h"

#define FORM_RESET(form) \
	form &= ~(FORM_SCALE | FORM_SIZE | FORM_OFFENSIVE); \
	if ((form & FORM_VERB) == 0) \
		form &= ~(FORM_GENDER)

static WordForm apply(Action *action, WordID id, Form src, Form master)
{
	WordForm wf = {0};

	if (id == 0)
		id = action->id;
#ifdef DEBUG
	if (id == 0)
		emulos_fail(ASSERT, "Word id is 0 in action\n");
#endif

	src &= ~FORM_OFFENSIVE;
	master &= ~FORM_OFFENSIVE;

	Word *word = words + id;
	Form form = form_override(src, action->form, 1);
	if (form & FORM_CASE)
		master &= ~FORM_CASE;
	form = form_override(form, master, 0);

	form = form_normalize(form);

	WordID ex = 0;

	switch (action->type) {
	case SELECTOR_LIT:
		wf.str = action->str;
		return wf;

	case SELECTOR_WORD:
		break;

#define GET_EXTRA(e) \
	FORM_RESET(form); \
	ex = word_get_random_extra(word, EXTRA_##e); \
	if (ex == 0) \
		goto fail; \
	word = words + ex

	case SELECTOR_TAG:
		GET_EXTRA(TAGS);
		if (action->flags & ACTION_DOUBLE)
			word = words + word_get_random_extra(word, EXTRA_TAGS);
		break;

	case SELECTOR_SYN:
		GET_EXTRA(SYN);
		break;

	case SELECTOR_ANT:
		GET_EXTRA(ANT);
		break;

	case SELECTOR_GNZ:
		GET_EXTRA(GNZS);

		if ((action->flags & ACTION_DOUBLE) == 0)
			break;

		WordID *p = ptcls[word->id];
		if (p == NULL)
			goto fail;

		int len = 0;
		for (; p[len]; len++);

		word = words + p[rng_int(len)];
		break;

	case SELECTOR_MARK:;
		Mark mark = action->mark;
		if (action->flags & ACTION_DOUBLE) {
			FORM_RESET(form);
			mark = word_get_random_mark(word);
		}

		WordID *m = marks[mark];
		if (m == NULL)
			goto fail;

		len = 0;
		for (; m[len]; len++);

		word = words + m[rng_int(len)];
		break;
	}

	wf = word_inflect(word, form);

	if (action->flags & ACTION_SILENT)
		wf.str = "";

	return wf;
fail:
	return (WordForm){0};
}

static WordForm apply_cross(Action *action, WordID id, Form src, Form master)
{
	;
	goto fail;
fail:
	return (WordForm){0};
}

int action_apply(Action *actions, int i, WordForm *wf, Form pg, Cache *cache)
{
	Form master = 0;

	if (actions[i].master) {
		int mi = i + actions[i].master;
		WordForm *mwf = wf + mi;
		if (mwf->str == NULL)
			if (action_apply(actions, mi, wf, pg, cache))
				return 1;

		master = mwf->form & (FORM_NUMBER | FORM_GENDER | FORM_CASE);

		if ((mwf->form & FORM_CASE) == FORM_ACCUSATIVE &&
		    mwf->flags & WORD_ANIMATED &&
		    (mwf->form & FORM_GENDER) != FORM_FEMININE) {
			master &= ~FORM_CASE;
			master |= FORM_GENITIVE;
		}
	}

	if (actions[i].flags & ACTION_PG) {
		if (pg) {
			master &= ~FORM_GENDER;
			master |= pg;

		} else if ((master & FORM_GENDER) == 0) {
			master |= FORM_MASCULINE;
		}
	}

	WordForm (*apply_proc)(Action *action, WordID id, Form src, Form master) = apply;

	if (actions[i].type == SELECTOR_CROSS)
		apply_proc = apply_cross;

	if ((actions[i].flags & ACTION_REF) == 0) {
		wf[i] = apply_proc(actions + i, 0, 0, master);
		if (wf[i].str)
			return 0;
		return 1;
	}

	/* Ref */
#ifdef DEBUG
	if (cache[actions[i].index].est == 0)
		emulos_fail(ASSERT, "No cache\n");
#endif
#define WFS_MAX 16
	WordForm wfs[WFS_MAX] = {0};
	Item *used[WFS_MAX] = {0};
	int wp = 0;

	Cache *cc = cache + actions[i].index;
	Item *item = cc->item;
	if (cc->used) {
		item = cc->used;

		wf[i] = apply_proc(actions + i, item->id, item_form_get(item), master);
		if (wf[i].str)
			return 0;
		cc->used = NULL;
		return 1;
	}

	for (; item; item = item->next) {
		Form f = item_form_get(item);

		wfs[wp] = apply_proc(actions + i, item->id, f, master);
		used[wp] = item;
		if (wfs[wp].str) {
			wp++;
			if (wp == WFS_MAX)
				break;
		}
	}

	if (wp == 0) {
		cc->used = NULL;
		return 1;
	}

	int r = rng_int(wp);
	cc->used = used[r];
	wf[i] = wfs[r];
	return 0;
}

void action_print(Action *action)
{
	for (int i = 0; i < action->len; i++) {
		printf("-> ");

		for (int i = 0; i > action[i].master; i--)
			printf("<");

		if (action[i].master < 0)
			printf(" ");

		switch (action[i].type) {
		case SELECTOR_LIT:
			printf("\"%s\"", action[i].str);
			break;

		case SELECTOR_WORD:
			if (action[i].flags & ACTION_REF) {
				printf("%d", action[i].index);
			} else {
				printf(".");
				word_print_by_id(action[i].id);
			}
			break;

		case SELECTOR_CROSS:
			printf("(");
			action_print(action[i].children);
			printf(")");
			break;
		}

		form_print(action[i].form);

		if (action[i].flags & ACTION_PG)
			printf("-pg");

		printf(" ");

		if (action[i].label) {
			if (action[i].flags & ACTION_LFLAG)
				printf("%%");
			else
				printf("$");

			if (action[i].flags & ACTION_LFLAGNOT)
				printf("!");

			printf("%d ", action[i].label);
		}

		for (int i = 0; i < action[i].master; i++)
			printf(">");

		if (action[i].master > 0)
			printf(" ");
	}
}
