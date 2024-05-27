#
# form.py
#

import symbol as sb

# keep in line with src/form.h

NOUN = 1
ADVERB = 2
ADJECTIVE = 3
CONJUNCTION = 4
PREPOSITION = 5
PRONOUN = 6
PARTICLE = 7

VERB = 8
PARTICIPLE = ADJECTIVE | VERB
TRANSGRESSIVE = ADVERB | VERB
NUMERAL = 12

SINGULAR = 0x00000010
PLURAL = 0x00000020

MASCULINE = 0x00000040
FEMININE = 0x00000080
COMMON = 0x000000c0

NOMINATIVE = 0x00000100
GENITIVE = 0x00000200
DATIVE = 0x00000300
ACCUSATIVE = 0x00000400
INSTRUMENTAL = 0x00000500
ADPOSITIONAL = 0x00000600

PRESENT = 0x00000800
PAST = 0x00001000
FUTURE = 0x00001800

INFINITIVE = 0x00002000
IMPERATIVE = 0x00004000
FIRST = 0x00006000
SECOND = 0x00008000
THIRD = 0x0000a000

POSITIVE = 0x00010000
COMPARATIVE = 0x00020000
SUPERLATIVE = 0x00030000

IMPERFECT = 0x00040000
PERFECT = 0x00080000

NONREFLEXIVE = 0x00100000
REFLEXIVE = 0x00200000

ACTIVE = 0x00400000
PASSIVE = 0x00800000

NORMAL = 0x01000000
DIMINUTIVE = 0x02000000
AUGMENTATIVE = 0x04000000

FULL = 0x08000000
SHORT = 0x10000000

RUSSIAN = 0x20000000
ENGLISH = 0x40000000

OFFENSIVE = 0x80000000

CLASS = 0x0000000f
INFLECTION = 0xfffffff0

NUMBER = SINGULAR | PLURAL
GENDER = MASCULINE | FEMININE | COMMON
CASE = NOMINATIVE | GENITIVE | DATIVE \
	| ACCUSATIVE | INSTRUMENTAL | ADPOSITIONAL
TENSE = PRESENT | PAST | FUTURE
PERSON = INFINITIVE | IMPERATIVE | FIRST | SECOND | THIRD
COMPARISON = POSITIVE | COMPARATIVE | SUPERLATIVE
PERFECTNESS = IMPERFECT | PERFECT
REFLEXIVENESS = NONREFLEXIVE | REFLEXIVE
ACTIVENESS = ACTIVE | PASSIVE
SCALE = NORMAL | DIMINUTIVE | AUGMENTATIVE
LANGUAGE = RUSSIAN | ENGLISH
SIZE = FULL | SHORT

DICT = {
	"noun": NOUN,
	"verb": VERB,
	"adv": ADVERB,
	"adj": ADJECTIVE,
	"ptcp": PARTICIPLE,
	"tran": TRANSGRESSIVE,
	"conj": CONJUNCTION,
	"prep": PREPOSITION,
	"pron": PRONOUN,
	"part": PARTICLE,
	"numr": NUMERAL,

	"sl": SINGULAR,
	"pl": PLURAL,

	"mas": MASCULINE,
	"fem": FEMININE,
	"com": COMMON,

	"nom": NOMINATIVE,
	"gen": GENITIVE,
	"dat": DATIVE,
	"acc": ACCUSATIVE,
	"ins": INSTRUMENTAL,
	"adp": ADPOSITIONAL,

	"pres": PRESENT,
	"past": PAST,
	"fut": FUTURE,

	"inf": INFINITIVE,
	"imp": IMPERATIVE,
	"1st": FIRST,
	"2nd": SECOND,
	"3rd": THIRD,

	"pos": POSITIVE,
	"comp": COMPARATIVE,
	"sup": SUPERLATIVE,

	"impf": IMPERFECT,
	"pf": PERFECT,

	"nrfx": NONREFLEXIVE,
	"rfx": REFLEXIVE,

	"act": ACTIVE,
	"pass": PASSIVE,

	"norm": NORMAL,
	"dim": DIMINUTIVE,
	"aug": AUGMENTATIVE,

	"full": FULL,
	"short": SHORT,

	"ru": RUSSIAN,
	"en": ENGLISH,

	"off": OFFENSIVE,
}

def scan(str):
	form = 0

	chain = str.lstrip(sb.FORM).rstrip().split(sb.FORM)
	for f in chain:
		if f not in DICT:
			return
		form |= DICT[f]
	return form

def is_class(form, cls):
	if (form & CLASS) == cls:
		return True
	return False

def stringify(form):
	str = ""
	cls = form & CLASS
	if cls == NOUN:
		str += "-noun"
	elif cls == ADVERB:
		str += "-adv"
	elif cls == ADJECTIVE:
		str += "-adj"
	elif cls == CONJUNCTION:
		str += "-conj"
	elif cls == PREPOSITION:
		str += "-prep"
	elif cls == PRONOUN:
		str += "-pron"
	elif cls == PARTICLE:
		str += "-part"
	elif cls == VERB:
		str += "-verb"
	elif cls == (NOUN | VERB):
		str += "-noun-verb"
	elif cls == PARTICIPLE:
		str += "-ptcp"
	elif cls == TRANSGRESSIVE:
		str += "-tran"
	elif cls == NUMERAL:
		str += "-num"

	if form & SINGULAR:
		str += "-sl"
	if form & PLURAL:
		str += "-pl"

	p = form & GENDER
	if p:
		if p == MASCULINE:
			str += "-mas"
		if p == FEMININE:
			str += "-fem"
		if p == COMMON:
			str += "-com"

	p = form & CASE
	if p:
		if p == NOMINATIVE:
			str += "-nom"
		if p == GENITIVE:
			str += "-gen"
		if p == DATIVE:
			str += "-dat"
		if p == ACCUSATIVE:
			str += "-acc"
		if p == INSTRUMENTAL:
			str += "-ins"
		if p == ADPOSITIONAL:
			str += "-adp"

	p = form & TENSE
	if p:
		if p == PRESENT:
			str += "-pres"
		if p == PAST:
			str += "-past"
		if p == FUTURE:
			str += "-fut"

	p = form & PERSON
	if p:
		if p == INFINITIVE:
			str += "-inf"
		if p == IMPERATIVE:
			str += "-imp"
		if p == FIRST:
			str += "-1st"
		if p == SECOND:
			str += "-2nd"
		if p == THIRD:
			str += "-3rd"

	p = form & COMPARISON
	if p:
		if p == POSITIVE:
			str += "-pos"
		if p == COMPARATIVE:
			str += "-comp"
		if p == SUPERLATIVE:
			str += "-sup"

	if form & IMPERFECT:
		str += "-impf"
	if form & PERFECT:
		str += "-pf"

	if form & NONREFLEXIVE:
		str += "-nrfx"
	if form & REFLEXIVE:
		str += "-rfx"

	if form & ACTIVE:
		str += "-act"
	if form & PASSIVE:
		str += "-pass"

	p = form & SCALE
	if p:
		if p == NORMAL:
			str += "-norm"
		if p == DIMINUTIVE:
			str += "-dim"
		if p == AUGMENTATIVE:
			str += "-aug"

	if form & FULL:
		str += "-full"
	if form & SHORT:
		str += "-short"

	if form & RUSSIAN:
		str += "-ru"
	if form & ENGLISH:
		str += "-en"

	if form & OFFENSIVE:
		str += "-off"

	return str
