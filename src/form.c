/*
 * form.c
 */

#include <stdio.h>

#include "form.h"

#define CONTAINS(g) \
	if ((target & FORM_##g) && \
	    (target & FORM_##g) != (form & FORM_##g)) \
		return 0

int form_contains(Form form, Form target)
{
	CONTAINS(NUMBER);
	CONTAINS(GENDER);
	CONTAINS(CASE);
	CONTAINS(TENSE);
	CONTAINS(PERSON);
	CONTAINS(COMPARISON);
	CONTAINS(PERFECTNESS);
	CONTAINS(REFLEXIVENESS);
	CONTAINS(ACTIVENESS);
	CONTAINS(SCALE);
	CONTAINS(SIZE);
	CONTAINS(LANGUAGE);
	CONTAINS(OFFENSIVE);

	return 1;
}

#define OVERRIDE(g) \
	if (target & FORM_##g) \
		form = (form & ~FORM_##g) | (target & FORM_##g)

Form form_override(Form form, Form target, int cls)
{
	if (cls && (target & FORM_CLASS))
		form = (form & FORM_INFLECTION) | (target & FORM_CLASS);

	OVERRIDE(NUMBER);
	OVERRIDE(GENDER);
	OVERRIDE(CASE);
	OVERRIDE(TENSE);
	OVERRIDE(PERSON);
	OVERRIDE(COMPARISON);
	OVERRIDE(PERFECTNESS);
	OVERRIDE(REFLEXIVENESS);
	OVERRIDE(ACTIVENESS);
	OVERRIDE(SCALE);
	OVERRIDE(SIZE);
	OVERRIDE(LANGUAGE);
	OVERRIDE(OFFENSIVE);

	return form;
}

Form form_normalize(Form form)
{
	switch (form & FORM_CLASS) {
	case FORM_NOUN:
	case FORM_PRONOUN:
	case FORM_NUMERAL:
		form &= FORM_CLASS | FORM_NUMBER | FORM_GENDER | FORM_CASE |
			FORM_SCALE | FORM_SIZE | FORM_LANGUAGE | FORM_OFFENSIVE;
		break;

	case FORM_ADJECTIVE:
		if (form & FORM_COMPARISON)
			form &= ~(FORM_NUMBER | FORM_GENDER | FORM_CASE);

		form &= FORM_CLASS | FORM_NUMBER | FORM_GENDER | FORM_CASE |
			FORM_COMPARISON | FORM_SCALE | FORM_SIZE |
			FORM_LANGUAGE | FORM_OFFENSIVE;
		break;

	case FORM_ADVERB:
		form &= FORM_CLASS | FORM_COMPARISON | FORM_SCALE |
			FORM_LANGUAGE | FORM_OFFENSIVE;
		break;

	case FORM_CONJUNCTION:
	case FORM_PREPOSITION:
	case FORM_PARTICLE:
		form &= FORM_CLASS;
		break;

	case FORM_VERB:
		if ((form & FORM_PERSON) == FORM_INFINITIVE)
			form &= ~(FORM_NUMBER | FORM_TENSE);

		form &= ~(FORM_CASE | FORM_COMPARISON | FORM_ACTIVENESS |
			  FORM_SCALE | FORM_SIZE);
		break;

	case FORM_PARTICIPLE:
		form &= ~(FORM_PERSON | FORM_COMPARISON | FORM_SCALE | FORM_SIZE);
		break;

	case FORM_TRANSGRESSIVE:
		form &= FORM_CLASS | FORM_TENSE | FORM_PERFECTNESS |
			FORM_REFLEXIVENESS | FORM_LANGUAGE;
		break;
	}

	if ((form & FORM_LANGUAGE) == FORM_ENGLISH)
		form &= FORM_CLASS | FORM_NUMBER | FORM_COMPARISON | FORM_SIZE;

	return form;
}

void form_print(Form form)
{
	switch (form & FORM_CLASS) {
	case FORM_NOUN:
		printf("-noun");
		break;

	case FORM_ADVERB:
		printf("-adv");
		break;

	case FORM_ADJECTIVE:
		printf("-adj");
		break;

	case FORM_PARTICIPLE:
		printf("-ptcp");
		break;

	case FORM_TRANSGRESSIVE:
		printf("-tran");
		break;

	case FORM_PREPOSITION:
		printf("-prep");
		break;

	case FORM_PRONOUN:
		printf("-pron");
		break;

	case FORM_PARTICLE:
		printf("-part");
		break;

	case FORM_VERB:
		printf("-verb");
		break;

	case FORM_NOUN | FORM_VERB:
		printf("-noun-verb");
		break;

	case FORM_NUMERAL:
		printf("-numr");
		break;

	}

#define PRINT_FORM(g, f, s) \
	if ((form & FORM_##g) == FORM_##f) \
		printf(s)

	PRINT_FORM(NUMBER, SINGULAR, "-sl");
	PRINT_FORM(NUMBER, PLURAL, "-pl");

	PRINT_FORM(GENDER, MASCULINE, "-mas");
	PRINT_FORM(GENDER, FEMININE, "-fem");
	PRINT_FORM(GENDER, COMMON, "-com");

	PRINT_FORM(CASE, NOMINATIVE, "-nom");
	PRINT_FORM(CASE, GENITIVE, "-gen");
	PRINT_FORM(CASE, DATIVE, "-dat");
	PRINT_FORM(CASE, ACCUSATIVE, "-acc");
	PRINT_FORM(CASE, INSTRUMENTAL, "-ins");
	PRINT_FORM(CASE, ADPOSITIONAL, "-adp");

	PRINT_FORM(TENSE, PRESENT, "-pres");
	PRINT_FORM(TENSE, PAST, "-past");
	PRINT_FORM(TENSE, FUTURE, "-fut");

	PRINT_FORM(PERSON, INFINITIVE, "-inf");
	PRINT_FORM(PERSON, IMPERATIVE, "-imp");
	PRINT_FORM(PERSON, FIRST, "-1st");
	PRINT_FORM(PERSON, SECOND, "-2nd");
	PRINT_FORM(PERSON, THIRD, "-3rd");

	PRINT_FORM(COMPARISON, POSITIVE, "-pos");
	PRINT_FORM(COMPARISON, COMPARATIVE, "-comp");
	PRINT_FORM(COMPARISON, SUPERLATIVE, "-sup");

	PRINT_FORM(PERFECTNESS, IMPERFECT, "-impf");
	PRINT_FORM(PERFECTNESS, PERFECT, "-pf");

	PRINT_FORM(REFLEXIVENESS, NONREFLEXIVE, "-nrfx");
	PRINT_FORM(REFLEXIVENESS, REFLEXIVE, "-rfx");

	PRINT_FORM(ACTIVENESS, ACTIVE, "-act");
	PRINT_FORM(ACTIVENESS, PASSIVE, "-pass");

	PRINT_FORM(SCALE, NORMAL, "-norm");
	PRINT_FORM(SCALE, DIMINUTIVE, "-dim");
	PRINT_FORM(SCALE, AUGMENTATIVE, "-aug");

	PRINT_FORM(SIZE, FULL, "-full");
	PRINT_FORM(SIZE, SHORT, "-short");

	PRINT_FORM(LANGUAGE, RUSSIAN, "-ru");
	PRINT_FORM(LANGUAGE, ENGLISH, "-en");

	PRINT_FORM(OFFENSIVE, OFFENSIVE, "-off");
}
