/*
 * word.h
 */

#ifndef EMULOS_WORD_H_
#define EMULOS_WORD_H_

#include <stdint.h>

#include "form.h"

/* keep in line with scripts/word.py */

enum {
	WORD_ANIMATED	= 0x01,
};

enum {
	EXTRA_TAGS,
	EXTRA_GNZS,
	EXTRA_SYN,
	EXTRA_ANT,

	EXTRA_TOTAL
};

typedef uint16_t WordID;
typedef uint16_t Mark;
typedef struct word Word;

struct word {
	WordID id;
	WordID base;

	uint16_t flags;

	uint16_t fc;
	Form *forms;
	char **data;

	WordID *extra[EXTRA_TOTAL];
	Mark *marks;
};

typedef struct word_form {
	WordID id;
	uint16_t flags;
	Form form;
	char *str;
} WordForm;

WordForm word_inflect(Word *word, Form form);

WordID word_get_random_extra(Word *word, int ex);
Mark word_get_random_mark(Word *word);

int word_is_extra(Word *word, WordID id, int ex);
int word_is_mark(Word *word, Mark mark);

Form word_form_find(Word *word, char *str);

void word_print_by_id(WordID id);
void word_print(Word *word);

extern Word words[];

extern WordID *ptcls[];
extern WordID *marks[];

#endif
