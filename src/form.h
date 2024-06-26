/*
 * form.h
 */

#ifndef EMULOS_FORM_H_
#define EMULOS_FORM_H_

#include <stdint.h>

/* keep in line with scripts/form.py */

enum {
	FORM_NONE,

	FORM_NOUN = 0x1,
	FORM_ADVERB = 0x2,
	FORM_ADJECTIVE = 0x3,
	FORM_CONJUNCTION = 0x4,
	FORM_PREPOSITION = 0x5,
	FORM_PRONOUN = 0x6,
	FORM_PARTICLE = 0x7,

	FORM_VERB = 0x8,
	FORM_PARTICIPLE = FORM_ADJECTIVE | FORM_VERB,
	FORM_TRANSGRESSIVE = FORM_ADVERB | FORM_VERB,

	FORM_NUMERAL = 0xc
};

enum {
	FORM_SINGULAR		= 0x00000010,
	FORM_PLURAL		= 0x00000020,

	FORM_MASCULINE		= 0x00000040,
	FORM_FEMININE		= 0x00000080,
	FORM_COMMON		= 0x000000c0,

	FORM_NOMINATIVE		= 0x00000100,
	FORM_GENITIVE		= 0x00000200,
	FORM_DATIVE		= 0x00000300,
	FORM_ACCUSATIVE		= 0x00000400,
	FORM_INSTRUMENTAL	= 0x00000500,
	FORM_ADPOSITIONAL	= 0x00000600,

	FORM_PRESENT		= 0x00000800,
	FORM_PAST		= 0x00001000,
	FORM_FUTURE		= 0x00001800,

	FORM_INFINITIVE		= 0x00002000,
	FORM_IMPERATIVE		= 0x00004000,
	FORM_FIRST		= 0x00006000,
	FORM_SECOND		= 0x00008000,
	FORM_THIRD		= 0x0000a000,

	FORM_POSITIVE		= 0x00010000,
	FORM_COMPARATIVE	= 0x00020000,
	FORM_SUPERLATIVE	= 0x00030000,

	FORM_IMPERFECT		= 0x00040000,
	FORM_PERFECT		= 0x00080000,

	FORM_NONREFLEXIVE	= 0x00100000,
	FORM_REFLEXIVE		= 0x00200000,

	FORM_ACTIVE		= 0x00400000,
	FORM_PASSIVE		= 0x00800000,

	FORM_NORMAL		= 0x01000000,
	FORM_DIMINUTIVE		= 0x02000000,
	FORM_AUGMENTATIVE	= 0x04000000,

	FORM_FULL		= 0x08000000,
	FORM_SHORT		= 0x10000000,

	FORM_RUSSIAN		= 0x20000000,
	FORM_ENGLISH		= 0x40000000,

	FORM_OFFENSIVE		= 0x80000000,

	FORM_CLASS		= 0x0000000f,
	FORM_INFLECTION		= 0xfffffff0,

	FORM_NUMBER = FORM_SINGULAR | FORM_PLURAL,
	FORM_GENDER = FORM_MASCULINE | FORM_FEMININE | FORM_COMMON,
	FORM_CASE = FORM_NOMINATIVE | FORM_GENITIVE | FORM_DATIVE |
		    FORM_ACCUSATIVE | FORM_INSTRUMENTAL | FORM_ADPOSITIONAL,
	FORM_TENSE = FORM_PRESENT | FORM_PAST | FORM_FUTURE,
	FORM_PERSON = FORM_INFINITIVE | FORM_IMPERATIVE | FORM_FIRST | FORM_SECOND | FORM_THIRD,
	FORM_COMPARISON = FORM_POSITIVE | FORM_COMPARATIVE | FORM_SUPERLATIVE,
	FORM_PERFECTNESS = FORM_IMPERFECT | FORM_PERFECT,
	FORM_REFLEXIVENESS = FORM_NONREFLEXIVE | FORM_REFLEXIVE,
	FORM_ACTIVENESS = FORM_ACTIVE | FORM_PASSIVE,
	FORM_SCALE = FORM_NORMAL | FORM_DIMINUTIVE | FORM_AUGMENTATIVE,
	FORM_SIZE = FORM_FULL | FORM_SHORT,
	FORM_LANGUAGE = FORM_RUSSIAN | FORM_ENGLISH,
};

typedef uint32_t Form;

int form_contains(Form form, Form target);
Form form_override(Form form, Form target, int cls);

Form form_normalize(Form form);

void form_print(Form form);

#endif
