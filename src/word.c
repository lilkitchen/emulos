/*
 * word.c
 */

#include <stdio.h>

#include "rng.h"
#include "word.h"

#define GOTO_BASE() \
	if (word->base) { \
		word = words + word->base; \
		goto base; \
	}

WordForm word_inflect(Word *word, Form form)
{
	WordForm wf = {
		.id = word->id,
	};

	if ((form & FORM_NUMBER) == FORM_PLURAL)
		form &= ~FORM_GENDER;
base:;
	int i = 0;
	Form *forms = word->forms;

	for (i = 0; i < word->fc; i++)
		if (forms[i] == form)
			goto found;

	Form cls = form & FORM_CLASS;
	for (i = 0; i < word->fc; i++) {
		if ((forms[i] & FORM_CLASS) != cls)
			continue;

		if (form_contains(forms[i], form))
			goto found;
	}

	GOTO_BASE();
	return wf;
found:
	wf.flags = word->flags;
	wf.form = forms[i];
	wf.str = word->data[i];
	return wf;
}

WordID word_get_random_extra(Word *word, int ex)
{
base:;
	WordID *extra = word->extra[ex];

	int len = 0;
	if (extra)
		for (; extra[len]; len++);

	if (len)
		return extra[rng_int(len)];

	GOTO_BASE();
	return 0;
}

Mark word_get_random_mark(Word *word)
{
base:;
	int len = 0;
	if (word->marks)
		for (; word->marks[len]; len++);

	if (len)
		return word->marks[rng_int(len)];

	GOTO_BASE();
	return 0;
}

int word_is_extra(Word *word, WordID id, int ex)
{
base:
	switch (ex) {
	case EXTRA_TAGS:
	case EXTRA_GNZS:
	case EXTRA_SYN:
		if (word->id == id)
			return 1;
	}

	WordID *extra = word->extra[ex];
	if (extra)
		for (int i = 0; extra[i]; i++)
			if (extra[i] == id)
				return 1;
	GOTO_BASE();
	return 0;
}

int word_is_mark(Word *word, Mark mark)
{
base:
	if (word->marks)
		for (int i = 0; word->marks[i]; i++)
			if (word->marks[i] == mark)
				return 1;
	GOTO_BASE();
	return 0;
}

Form word_form_find(Word *word, char *str)
{
	for (int i = 0; i < word->fc; i++)
		if (word->data[i] == str)
			return word->forms[i];
	return 0;
}

void word_print_by_id(WordID id)
{
	if (id == 0)
		return;

	word_print(words + id);
}

void word_print(Word *word)
{
	printf("<");
	if (word->data == NULL) {
		printf("NULL_WORD");
		goto end;
	}

	printf("%s", word->data[0]);
	if (word->data[1])
		printf(" %s", word->data[1]);
end:
	printf(">");
}
