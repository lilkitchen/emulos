#
# inflect.py
#

import form as fm
import letter
import word as wd

LATIN = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'}

def noun(word, form, str):
	end = str[-1]
	r = str[:-1]

	if end in LATIN:
		return [str]

	if (form & fm.NUMBER) == fm.PLURAL:
		if end == 'и':
			ac = '' if word.flags & wd.ANIMATED else 'и'
			if r[-1] == 'к':
				return [str, r, r + 'ам', r + ac, r + 'ами', r + 'ах']
			ac = 'ов' if word.flags & wd.ANIMATED else end
			if letter.is_vowel(r[-1]):
				ac = 'ев' if word.flags & wd.ANIMATED else end
			if (form & fm.GENDER) == fm.FEMININE:
				ac = 'ей' if word.flags & wd.ANIMATED else end
			if letter.is_beforei(r[-1]):
				return [str, r + 'ей', r + 'ам', r + ac, r + 'ами', r + 'ах']
			if (form & fm.GENDER) == fm.FEMININE:
				return [str, r + 'ей', r + 'ям', r + ac, r + 'ями', r + 'ях']
			if letter.is_vowel(r[-1]):
				return [str, r + 'ев', r + 'ям', r + ac, r + 'ями', r + 'ях']
			return [str, r + 'ов', r + 'ам', r + ac, r + 'ами', r + 'ах']

		if end == 'ы':
			if (form & fm.GENDER) == fm.FEMININE:
				ac = '' if word.flags & wd.ANIMATED else 'ы'
				return [str, r, r + 'ам', r + ac, r + 'ами', r + 'ах']
			ac = 'ов' if word.flags & wd.ANIMATED else 'ы'
			return [str, r + 'ов', r + 'ам', r + ac, r + 'ами', r + 'ах']

		if end == 'а':
			if (form & fm.GENDER) == fm.COMMON or (form & fm.SCALE) == fm.AUGMENTATIVE:
				return [str, r, r + 'ам', r + 'а', r + 'ами', r + 'ах']
			return [str, r + 'ов', r + 'ам', r + 'а', r + 'ами', r + 'ах']

		if end == 'я':
			ac = 'ей' if word.flags & wd.ANIMATED else 'я'
			if letter.is_vowel(r[-1]):
				return [str, r + 'ий', r + 'ям', r + ac, r + 'ями', r + 'ях']
			return [str, r + 'ей', r + 'ям', r + ac, r + 'ями', r + 'ях']

	if end == 'а':
		if letter.is_beforei(r[-1]):
			return [str, r + 'и', r + 'е', r + 'у', r + 'ой', r + 'e']
		return [str, r + 'ы', r + 'е', r + 'у', r + 'ой', r + 'e']

	if end == 'я':
		return [str, r + 'и', r + 'е', r + 'ю', r + 'ей', r + 'e']

	if end == 'о':
		return [str, r + 'а', r + 'у', r + 'о', r + 'ом', r + 'е']

	if end == 'е':
		if (form & fm.GENDER) == fm.MASCULINE:
			return [str, r + 'а', r + 'у', str, r + 'ем', r + 'е']
		if letter.is_vowel(r[-1]):
			return [str, r + 'я', r + 'ю', str, r + 'ем', r + 'и']
		return [str, r + 'я', r + 'ю', str, r + 'ем', str]

	if end == 'ь' or end == 'й':
		if (form & fm.GENDER) == fm.FEMININE:
			return [str, r + 'и', r + 'и', str, str + 'ю', r + 'и']
		if word.flags & wd.ANIMATED:
			return [str, r + 'я', r + 'ю', r + 'я', r + 'ем', r + 'e']
		return [str, r + 'я', r + 'ю', r, r + 'ем', r + 'e']

	if end == 'к' and (r[-1] == 'о' or r[-1] == 'е') and \
		(form & fm.GENDER) == fm.MASCULINE:
		rt = r[:-1]
		if word.flags & wd.ANIMATED:
			return [str, rt + 'ка', rt + 'ку', rt + 'ка', rt + 'ком', rt + 'ке']
		return [str, rt + 'ка', rt + 'ку', str, rt + 'ком', rt + 'ке']

	ac = 'а' if word.flags & wd.ANIMATED else ''
	return [str, str + 'а', str + 'у', str + ac, str + 'ом', str + 'e']

def adjective(word, form, str):
	end = str[-2:]
	r = str[:-2]
	f = ''

	if end[-1] in LATIN:
		return [str]

	if end == 'ся':
		f = 'ся'
		str = r
		end = str[-2:]
		r = str[:-2]

	if end == 'ой' or end == 'ое':
		if letter.is_beforei(r[-1]):
			return fin([str, r + 'ого', r + 'ому', r + end, r + 'им', r + 'ом'], f)
		return fin([str, r + 'ого', r + 'ому', r + end, r + 'ым', r + 'ом'], f)

	if end == 'ий':
		return fin([str, r + 'его', r + 'ему', str, r + 'им', r + 'ем'], f)

	if end == 'ый':
		return fin([str, r + 'ого', r + 'ому', str, r + 'ым', r + 'ом'], f)

	if end == 'ая':
		if (form & fm.COMPARISON) == fm.SUPERLATIVE \
			or fm.is_class(form, fm.PARTICIPLE):
			if (form & fm.ACTIVENESS) == fm.PASSIVE:
				return fin([str, r + 'ой', r + 'ой', r + 'ую', r + 'ой', r + 'ой'], f)
			return fin([str, r + 'ей', r + 'ей', r + 'ую', r + 'ей', r + 'ей'], f)
		if letter.is_beforei(r[-1]):
			return fin([str, r + 'ей', r + 'ей', r + 'ую', r + 'ей', r + 'ей'], f)
		return fin([str, r + 'ой', r + 'ой', r + 'ую', r + 'ой', r + 'ой'], f)

	if end == 'яя':
		return fin([str, r + 'ей', r + 'ей', r + 'юю', r + 'ей', r + 'ей'], f)

	if end == 'ее':
		return fin([str, r + 'его', r + 'ему', str, r + 'им', r + 'ем'], f)

	if end == 'ье':
		return fin([str, r + 'ьего', r + 'ьему', str, r + 'ьим', r + 'ьем'], f)

	if end == 'ья':
		return fin([str, r + 'ьей', r + 'ьей', r + 'ью', r + 'ьей', r + 'ьей'], f)

	if end == 'ие':
		return fin([str, r + 'их', r + 'им', str, r + 'ими', r + 'их'], f)

	if end == 'ые':
		return fin([str, r + 'ых', r + 'ым', str, r + 'ыми', r + 'ых'], f)

	return ["none", "none", "none", "none", "none", "none"]

def fin(chain, f):
	ch = []
	for str in chain:
		ch.append(str + f)

	return ch
