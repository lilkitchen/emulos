/*
 * regexp.h
 */

#ifndef EMULOS_REGEXP_H_
#define EMULOS_REGEXP_H_

#include "cache.h"
#include "item.h"

/* keep in line with scripts/regexp.py */

enum {
	REGEXP_START	= 0x01,
	REGEXP_END	= 0x02,
};

enum {
	PATTERN_REF,

	PATTERN_ONE,
	PATTERN_ANY,
	PATTERN_OR,
	PATTERN_TARGET,
};

typedef struct pattern Pattern;

struct pattern {
	char type;
	int len;
	union {
		uint16_t index;
		uint16_t *indices;
	};
};

typedef struct brain Brain;
typedef struct frame Frame;

typedef struct regexp Regexp;

struct regexp {
	char flags;
	uint16_t len;
	uint16_t min_len;
	Pattern *data;
};

Item *regexp_apply(Regexp *regexp, Brain *brain, Message *msg, Cache *cache);

void regexp_print(Regexp *regexp);

#endif
