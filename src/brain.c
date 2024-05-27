/*
 * brain.c
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "emulos.h"
#include "parse.h"
#include "regexp.h"
#include "selector.h"
#include "brain.h"

void brain_init(Brain *brain)
{
	memset(brain, 0, sizeof(Brain));

	brain->items = items_create();
}

#define HISTORY_DESTROY(history, hp, destroy) \
	for (int i = 0; i < brain->hp; i++) \
		destroy(brain->history[i])

void brain_quit(Brain *brain)
{
	HISTORY_DESTROY(heard, heardp, message_destroy);
	HISTORY_DESTROY(said, saidp, message_destroy);
	HISTORY_DESTROY(lblocks, lbp, label_block_destroy);

	items_destroy(brain->items);
}

void brain_wash(Brain *brain)
{
	memset(brain->cache, 0, sizeof(brain->cache));
}

#define HISTORY_SHIFT(history, hp, destroy) \
	do { \
		int p = hp; \
		if (p == HISTORY_LIMIT) \
			destroy(history[--p]); \
		else \
			hp++; \
		for (; p > 0; p--) \
			history[p] = history[p - 1]; \
	} while (0)

int brain_read(Brain *brain, char *str)
{
	Message *msg = parse(str);
	if (msg == NULL)
		return 1;

	HISTORY_SHIFT(brain->heard, brain->heardp, message_destroy);
	brain->heard[0] = msg;

	return 0;
}

int brain_react(Brain *brain, Action *action, char *reply)
{
	Message *msg = parse(reply);
	if (msg == NULL)
		return 1;

	HISTORY_SHIFT(brain->said, brain->saidp, message_destroy);
	brain->said[0] = msg;

	HISTORY_SHIFT(brain->lblocks, brain->lbp, label_block_destroy);

	Label *labels = brain->lblocks[0].labels;
	memset(labels, 0, sizeof(LabelBlock));

	int lp = 0;
	for (int i = 0; i < action->len; i++) {
		if (action[i].flags & ACTION_LFLAG) {
			if (action[i].flags & ACTION_LFLAGNOT)
				brain->flags &= ~action[i].lflag;
			else
				brain->flags |= action[i].lflag;
			continue;
		}

		if (action[i].label == 0)
			continue;

		Message *msg = NULL;

		Cache *cache = brain->cache + action[i].index;
		if (cache->item)
			msg = cache->item->msg;

		labels[lp++] = (Label){
			.name = action[i].label,
			.msg = message_copy(msg)
		};
	}

	return 0;
}

#define PUSH_ITEM(top) \
	do { \
		top->next = item; \
		item = top; \
	} while (0)

#define ADD_ITEM(data) \
	do { \
		Item *top = item_add(brain->items, msg + k, data); \
		PUSH_ITEM(top); \
	} while (0)

#define FOR_HISTORY(i) \
	for (int i = sel->start; i < limit; i++)

#define FOR_MESSAGE(msg, k) \
	Message *msg = history[i]; \
	for (int k = 0; k < msg->len; k++)

int brain_poll(Brain *brain, int index)
{
	if (brain->cache[index].est)
		return !brain->cache[index].item;

	Selector *sel = selectors + index;

	Message **history = brain->heard;
	int hp = brain->heardp;
	if (sel->flags & SELECTOR_SELF || sel->type == SELECTOR_LABEL) {
		history = brain->said;
		hp = brain->saidp;
	}

	int limit = sel->range > hp ? hp : sel->range;

	int ex = 0;
	int cross = 0;

	Item *item = NULL;
	int ind = index;
poll:;
	Selector *s = selectors + ind;
	switch (s->type) {
	case SELECTOR_LIT:
		FOR_HISTORY(i) {
			FOR_MESSAGE(msg, k) {
				if (strcmp(msg[k].token->str, s->literal) == 0)
					ADD_ITEM(0);
			}
		}
		break;

	case SELECTOR_WORD:
		FOR_HISTORY(i) {
			FOR_MESSAGE(msg, k) {
				WordID id = token_find_word(msg[k].token, s->id);
				if (id)
					ADD_ITEM(id);
			}
		}
		break;

	case SELECTOR_TAG:
		ex = EXTRA_TAGS;
	if (0)
	case SELECTOR_GNZ:
		ex = EXTRA_GNZS;
	if (0)
	case SELECTOR_SYN:
		ex = EXTRA_SYN;
	if (0)
	case SELECTOR_ANT:
		ex = EXTRA_ANT;

		FOR_HISTORY(i) {
			FOR_MESSAGE(msg, k) {
				WordID id = token_find_extra(msg[k].token, s->id, ex);
				if (id)
					ADD_ITEM(id);
			}
		}
		break;

	case SELECTOR_MARK:
		FOR_HISTORY(i) {
			FOR_MESSAGE(msg, k) {
				WordID id = token_find_mark(msg[k].token, s->mark);
				if (id)
					ADD_ITEM(id);
			}
		}
		break;

	case SELECTOR_REF:
		if (brain->cache[s->ref].est == 0)
			brain_poll(brain, s->ref);

		item = brain->cache[s->ref].item;
		break;

	case SELECTOR_OR:
		for (int i = 0; s->children[i]; i++)
			if (brain_poll(brain, s->children[i]) == 0) {
				item = brain->cache[s->children[i]].item;
				break;
			}
		break;

	case SELECTOR_CROSS:
		cross = 1;
		ind = sel->children[0];

		if (brain->cache[ind].est == 0)
			goto poll;

		item = brain->cache[ind].item;
		goto cross;

	case SELECTOR_REGEXP:
		FOR_HISTORY(i) {
			Message *msg = history[i];
			item = regexp_apply(s->regexp, brain, msg, brain->cache);
		}
		break;

	case SELECTOR_UNREC:
		FOR_HISTORY(i) {
			FOR_MESSAGE(msg, k) {
				if (msg[k].token->ids == 0 &&
				    msg[k].len && msg[k].role == MSG_EST)
					ADD_ITEM(0);
			}
		}
		break;

	case SELECTOR_FORM:
		FOR_HISTORY(i) {
			FOR_MESSAGE(msg, k) {
				WordID id;
				id = token_find_form(msg[k].token, s->form);
				if (id)
					ADD_ITEM(id);
			}
		}
		break;

	case SELECTOR_ROLE:
		FOR_HISTORY(i) {
			FOR_MESSAGE(msg, k) {
				if (msg[k].role & s->role) {
					Item *top = item_add(brain->items,
							     msg + k,
							     token_random_id(msg[k].token));
					PUSH_ITEM(top);
				}
			}
		}
		break;

	case SELECTOR_LABEL:
		FOR_HISTORY(i) {
			Label *labels = brain->lblocks[i].labels;
			for (int k = 0; k < ACTION_EFFECTS_LIMIT; k++) {
				if (labels[k].name == 0 || labels[k].name != s->label)
					continue;

				Item *top = item_add(brain->items, &labels[k].msg, 0);
				PUSH_ITEM(top);
			}
		}
		break;

	case SELECTOR_LFLAG:
		if (brain->flags & s->lflag)
			item = &item_dummy;
		break;

	default:
		printf(">>> %d\n", sel->type);
		emulos_fail(ASSERT, "Bad selector type\n");
	}

end:
	if (s->flags & SELECTOR_NOT)
		item = item ? NULL : &item_dummy;

	brain->cache[ind].est = 1;
	brain->cache[ind].item = item;

	if (!cross)
		return !item;

	item = item_copy(brain->items, item);
cross:
#define REMOVE_IF_NOT(x) \
	do { \
		if (!!x == not) { \
			*slot = it->next; \
		} else { \
			slot = &it->next; \
		} \
		it = it->next; \
	} while (0)

	s = selectors + sel->children[cross++];
	if (s->type == SELECTOR_NONE) {
		cross = 0;
		ind = index;
		goto end;
	}

	int not = s->flags & SELECTOR_NOT;

	Item **slot = &item;
	Item *it = *slot;
	switch (s->type) {
	case SELECTOR_LIT:
		while (it) {
			int x = (strcmp(it->msg->token->str, s->literal) == 0);
			REMOVE_IF_NOT(x);
		}
		break;

	case SELECTOR_WORD:
		while (it) {
			WordID id = token_find_word(it->msg->token, s->id);
			REMOVE_IF_NOT(id);
		}
		break;

	case SELECTOR_TAG:
		ex = EXTRA_TAGS;
	if (0)
	case SELECTOR_GNZ:
		ex = EXTRA_GNZS;
	if (0)
	case SELECTOR_SYN:
		ex = EXTRA_SYN;
	if (0)
	case SELECTOR_ANT:
		ex = EXTRA_ANT;
		while (it) {
			WordID id = token_find_extra(it->msg->token, s->id, ex);
			REMOVE_IF_NOT(id);
		}
		break;

	case SELECTOR_FORM:
		while (it) {
			WordID id = 0;
			id = token_find_form(it->msg->token, s->form);
			REMOVE_IF_NOT(id);
		}
		break;

	case SELECTOR_ROLE:
		while (it) {
			int x = (it->msg->role & s->role) == s->role;
			REMOVE_IF_NOT(x);
		}
		break;

	case SELECTOR_MARK:
		while (it) {
			WordID id = token_find_mark(it->msg->token, s->mark);
			REMOVE_IF_NOT(id);
		}
		break;

	default:
		printf(">>> %d\n", s->type);
		emulos_fail(ASSERT, "Bad cross selector type\n");
	}
	goto cross;
}
