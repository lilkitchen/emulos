/*
 * selector.h
 */

#ifndef EMULOS_SELECTOR_H_
#define EMULOS_SELECTOR_H_

#include "regexp.h"
#include "word.h"

/* keep in line with scripts/selecctor.py */

enum {
	SELECTOR_NONE,

	SELECTOR_LIT,
	SELECTOR_WORD,
	SELECTOR_TAG,
	SELECTOR_GNZ,
	SELECTOR_SYN,
	SELECTOR_ANT,
	SELECTOR_MARK,

	SELECTOR_REF,

	SELECTOR_OR,
	SELECTOR_AND,
	SELECTOR_CROSS,
	SELECTOR_REGEXP,

	SELECTOR_UNREC,

	SELECTOR_FORM,
	SELECTOR_ROLE,
	SELECTOR_LABEL,
	SELECTOR_LFLAG,
	SELECTOR_WFLAG,
};

enum {
	SELECTOR_NOT	= 0x01,
	SELECTOR_TRUMP	= 0x02,
	SELECTOR_SELF	= 0x04,
};

typedef struct selector Selector;

struct selector {
	char type;
	char flags;

	uint16_t start;
	uint16_t range;

	union {
		int *children;
		Regexp *regexp;
		int ref;
		char *literal;
		WordID id;
		struct {
			Form form;
			Form mask;
		};
		int role;
		int label;
		int lflag;
		Mark mark;
	};
};

extern Selector selectors[];

void selector_print(Selector *sel);

#endif
