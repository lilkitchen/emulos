/*
 * action.h
 */

#ifndef EMULOS_ACTION_H_
#define EMULOS_ACTION_H_

#include "word.h"

/* keep in line with scripts/action.py */

enum {
	ACTION_REF	= 0x01,
	ACTION_DOUBLE	= 0x02,
	ACTION_SILENT	= 0x04,

	ACTION_LFLAG	= 0x08,
	ACTION_LFLAGNOT	= 0x10,

	ACTION_PG	= 0x20,
};

typedef struct action Action;

struct action {
	char type;
	char flags;

	char master;

	int len;
	union {
		Action *children;
		char *str;
		int index;
		WordID id;
		Mark mark;
	};
	Form form;
	union {
		uint16_t e;
		uint16_t label;
		uint16_t lflag;
	};
};

typedef struct cache Cache;

int action_apply(Action *action, int i, WordForm *wf, Form pg, Cache *cache);
void action_print(Action *action);

#endif
