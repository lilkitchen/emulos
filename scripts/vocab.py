#
# vocab.py
#

import reader
import form as fm
import inflect
import registry as rg
import symbol as sb
import token as tk
import word as wd

EXTRA = {
	sb.TAG, sb.GNZ, sb.SYN, sb.ANT, sb.MARK, sb.WORD
}

class Vocab(reader.Reader):
	def __init__(self, input):
		super().__init__(input)

		self.line = None

		self.words = [wd.Word(0)]

		self.lflags_reg = rg.RegistryBit()
		self.lflags_reg.reg("pg")

		self.marks_reg = rg.Registry()
		self.marks = []

		self.SCAN = {
			fm.NOUN: self.scan_noun,
			fm.ADVERB: self.scan_adv,
			fm.ADJECTIVE: self.scan_adj,
			fm.PARTICIPLE: self.scan_ptcp,
			fm.TRANSGRESSIVE: self.scan_tran,
			fm.CONJUNCTION: self.scan_part,
			fm.PREPOSITION: self.scan_part,
			fm.PRONOUN: self.scan_noun,
			fm.PARTICLE: self.scan_part,

			fm.VERB: self.scan_verb,
			fm.NOUN | fm.VERB: self.scan_noun,
			fm.NUMERAL: self.scan_noun,
		}

		self.scan()
		if self.err:
			return

		self.token_table = []

		self.tokens = tk.tokenize(self.words)
		if not self.tokens:
			return self.fail("Failed to tokenize")

		self.token_table = tk.table_create(self.tokens)

		self.marks = [[] for i in range(self.marks_reg.len())]

		# Resolve #
		self.not_found = set()
		for w in self.words:
			w.tags = self.resolve_arr(w.tags)
			w.gnzs = self.resolve_arr(w.gnzs)
			w.syn = self.resolve_arr(w.syn)
			w.ant = self.resolve_arr(w.ant)
			w.base = self.resolve_arr(w.base)

		# Join all extra #
		for w in self.words:
			for id in w.tags:
				self.words[id].tags.add(w.id)
			for id in w.syn:
				self.words[id].syn.add(w.id)
			for id in w.ant:
				self.words[id].ant.add(w.id)

			for id in w.gnzs:
				self.words[id].ptcls.append(w.id)

			for m in w.marks:
				self.marks[m].append(w.id)

	def resolve_arr(self, arr):
		ret = []
		for e in arr:
			if e not in self.tokens:
				if e not in self.not_found:
					print("Warning: extra \"%s\" not found" % e)
					self.not_found.add(e)
				continue
			ret.append(self.tokens[e].fds[0][1])
		s = set()
		for r in ret:
			s.add(r)
		return s

	def get_line(self):
		while True:
			line = self.readline()
			if line is None:
				return

			if line.lstrip()[:2] != "//":
				break
		return line

	def line_split(self):
		return self.line.strip().split(' ')

	def line_split_next(self):
		line = self.get_line()
		if line is None or len(line) == 0 or line[0] == '\n' or \
			line[0] == sb.FORM or line[0] in EXTRA:
			return
		return self.line_split()

	def check_for_unexpected_data(self):
		if self.line_split_next():
			self.fail("Unexpected data", self.line)

	def scan(self):
		line = self.get_line()

		word = wd.Word(1)
		while True:
			line = self.line
			if line is None:
				if self.err:
					return
				break

			if line[0] == sb.WFLAG:
				err = word.read_flags(line)
				if err:
					return self.fail(err, line)
				self.get_line()
				continue

			if len(line) == 0 or line[0] == '\n':
				if not word.is_empty():
					self.words.append(word)
					word = wd.Word(len(self.words))
				if len(line) == 0:
					return
				self.get_line()
				continue

			if line[0] in EXTRA:
				chain = self.line_split()
				for ex in chain:
					if ex[0] == sb.TAG:
						word.tags.append(ex[1:])

					elif ex[0] == sb.GNZ:
						word.gnzs.append(ex[1:])

					elif ex[0] == sb.SYN:
						word.syn.append(ex[1:])

					elif ex[0] == sb.ANT:
						word.ant.append(ex[1:])

					elif ex[0] == sb.MARK:
						ps = self.marks_reg.reg(ex[1:])
						word.marks.append(ps)

					elif ex[0] == sb.WORD:
						word.base.append(ex[1:])

					else:
						return self.fail("Wrong extra data", ex)

				self.get_line()
				continue

			if line[0] == sb.FORM:
				form = fm.scan(line)
				if form is None:
					return self.fail("Wrong form", line)

				line = self.get_line()
				if line is None:
					return self.fail("Unexpected end")

				self.SCAN[form & fm.CLASS](word, form)
				if self.err:
					return
				continue

			return self.fail("Wrong syntax", line)

	def scan_noun(self, word, form):
		# Singular #
		chain = self.line_split()

		if len(chain) == 1:
			chain = inflect.noun(word, form, chain[0])

		elif len(chain) != 6:
			return self.fail("Wrong noun syntax", self.line)

		if len(chain) == 1:
			# english #
			word.add(form | fm.SINGULAR, chain[0])
		else:
			word.add_cases(form | fm.SINGULAR, chain)

		singular = chain[0]

		# Plural #
		chain = self.line_split_next()
		if not chain:
			if form & fm.ENGLISH:
				word.add(form | fm.PLURAL, singular + 's')
			return

		if len(chain) == 1:
			chain = inflect.noun(word, form | fm.PLURAL, chain[0])

		elif len(chain) != 6:
			return self.err("Wrong plural noun syntax", self.line)

		if len(chain) == 1:
			word.add(form | fm.PLURAL, chain[0])
		else:
			word.add_cases(form | fm.PLURAL, chain)

		self.check_for_unexpected_data()

	def scan_verb(self, word, form):
		chain = self.line_split()

		# English #
		if form & fm.ENGLISH:
			if len(chain) != 3:
				return self.fail("Wrong english verb form count", self.line)

			word.add(form | fm.INFINITIVE | fm.PRESENT, chain[0])
			word.add(form | fm.INFINITIVE | fm.PAST, chain[1])
			word.add(form | fm.INFINITIVE | fm.PAST | fm.PERFECT, chain[2])

			return self.check_for_unexpected_data()

		if len(chain) != 3:
			return self.fail("Wrong verb form count", self.line)

		rf = False
		if form & fm.REFLEXIVE:
			rf = True
			form &= ~fm.REFLEXIVENESS

		if not form & fm.PERFECT:
			form |= fm.IMPERFECT

		elif chain[0][-2:] == 'ся':
			form |= fm.REFLEXIVE

		word.add_verb(form | fm.INFINITIVE, chain[0], rf, 'ся')
		word.add_verb(form | fm.IMPERATIVE | fm.SINGULAR, chain[1], rf, 'сь')
		if form & fm.REFLEXIVE:
			word.add_verb(form | fm.IMPERATIVE | fm.PLURAL,
					chain[1][-2:] + 'тесь', rf, 'сь')
		else:
			word.add_verb(form | fm.IMPERATIVE | fm.PLURAL,
					chain[1] + 'те', rf, 'сь')
		word.add_verb(form | fm.SINGULAR | fm.PAST | fm.THIRD | fm.FEMININE,
			chain[2], rf, 'сь')
		word.add_verb(form | fm.SINGULAR | fm.PAST | fm.THIRD | fm.COMMON,
			chain[2][:-1] + 'о', rf, 'сь')

		chain = self.line_split_next()
		if not chain or len(chain) != 4:
			return self.fail("Wrong verb syntax", self.line)

		form |= fm.SINGULAR
		if form & fm.PERFECT:
			word.add_verb(form | fm.PAST | fm.MASCULINE | fm.THIRD, chain[0], rf, 'ся')
			word.add_verb(form | fm.FUTURE | fm.FIRST, chain[1], rf, 'сь')
			word.add_verb(form | fm.FUTURE | fm.SECOND, chain[2], rf, 'ся')
			word.add_verb(form | fm.FUTURE | fm.THIRD, chain[3], rf, 'ся')

		else:
			word.add_verb(form | fm.PRESENT | fm.FIRST, chain[0], rf, 'сь')
			word.add_verb(form | fm.PRESENT | fm.SECOND, chain[1], rf, 'ся')
			word.add_verb(form | fm.PRESENT | fm.THIRD, chain[2], rf, 'ся')
			word.add_verb(form | fm.MASCULINE | fm.THIRD | fm.PAST, chain[3], rf, 'ся')
		form &= ~fm.SINGULAR

		# Plural #
		chain = self.line_split_next()
		if not chain or len(chain) != 4:
			return self.fail("Wrong verb syntax", self.line)

		form |= fm.PLURAL
		if form & fm.PERFECT:
			word.add_verb(form | fm.PAST | fm.THIRD, chain[0], rf, 'сь')
			word.add_verb(form | fm.FUTURE | fm.FIRST, chain[1], rf, 'ся')
			word.add_verb(form | fm.FUTURE | fm.SECOND, chain[2], rf, 'сь')
			word.add_verb(form | fm.FUTURE | fm.THIRD, chain[3], rf, 'ся')

		else:
			word.add_verb(form | fm.FIRST | fm.PRESENT, chain[0], rf, 'ся')
			word.add_verb(form | fm.SECOND | fm.PRESENT, chain[1], rf, 'сь')
			word.add_verb(form | fm.THIRD | fm.PRESENT, chain[2], rf, 'ся')
			word.add_verb(form | fm.THIRD | fm.PAST, chain[3], rf, 'сь')

		self.check_for_unexpected_data()

	def scan_adv(self, word, form):
		chain = self.line_split()

		if len(chain) >= 1:
			word.add(form | fm.POSITIVE, chain[0])

		if len(chain) >= 2:
			word.add(form | fm.COMPARATIVE, chain[1])

		if len(chain) >= 3:
			word.add(form | fm.SUPERLATIVE, chain[2])

		self.check_for_unexpected_data()

	def scan_adj_like(self, word, form, rf):
		chain = self.line_split()

		if len(chain) == 1:
			ch = chain[0].split('/')
			if len(ch) != 4:
				return self.fail("Wrong adjective syntax")

			root = ch[0][:-2]
			chain = [ch[0], root + ch[1], root + ch[2], root + ch[3]]

		elif len(chain) == 6:
			word.add_cases(form | fm.SINGULAR | fm.MASCULINE, chain)

			chain = self.line_split_next()
			if not chain or len(chain) != 6:
				return self.fail("Expected 6 feminine forms")
			word.add_cases(form | fm.SINGULAR | fm.FEMININE, chain)

			chain = self.line_split_next()
			if not chain or len(chain) != 6:
				return self.fail("Expected 6 common forms")
			word.add_cases(form | fm.SINGULAR | fm.COMMON, chain)

			chain = self.line_split_next()
			if not chain or len(chain) != 6:
				return self.fail("Expected 6 plural forms")
			word.add_cases(form | fm.PLURAL, chain)
			return

		if len(chain) != 4:
			return self.fail("Expected 4 adjective forms", self.line)

		mas = inflect.adjective(word, form | fm.MASCULINE, chain[0])
		fem = inflect.adjective(word, form | fm.FEMININE, chain[1])
		com = inflect.adjective(word, form | fm.COMMON, chain[2])
		pl = inflect.adjective(word, form | fm.PLURAL, chain[3])

		word.add_cases(form | fm.SINGULAR | fm.MASCULINE, mas)
		word.add_cases(form | fm.SINGULAR | fm.FEMININE, fem)
		word.add_cases(form | fm.SINGULAR | fm.COMMON, com)
		word.add_cases(form | fm.PLURAL, pl)
		if rf:
			form |= fm.REFLEXIVE
			word.add_cases(form | fm.SINGULAR | fm.MASCULINE, inflect.fin(mas, 'ся'))
			word.add_cases(form | fm.SINGULAR | fm.FEMININE, inflect.fin(fem, 'ся'))
			word.add_cases(form | fm.SINGULAR | fm.COMMON, inflect.fin(com, 'ся'))
			word.add_cases(form | fm.PLURAL, inflect.fin(pl, 'ся'))

	def scan_adj(self, word, form):
		chain = self.line_split()

		if form & fm.SHORT:
			if len(chain) != 4:
				return self.fail("Expected 4 short forms", self.line)

			word.add(form | fm.MASCULINE, chain[0])
			word.add(form | fm.FEMININE, chain[1])
			word.add(form | fm.COMMON, chain[2])
			word.add(form | fm.PLURAL, chain[3])

			self.check_for_unexpected_data()
			return

		# English #
		if form & fm.ENGLISH:
			if len(chain) >= 1:
				word.add(form | fm.POSITIVE, chain[0])

			if len(chain) >= 2:
				word.add(form | fm.COMPARATIVE, chain[1])

			if len(chain) >= 3:
				word.add(form | fm.SUPERLATIVE, chain[2])
			self.check_for_unexpected_data()
			return

		self.scan_adj_like(word, form, False)
		if self.err:
			return

		# Comparison #
		chain = self.line_split_next()
		if not chain:
			return

		if len(chain) != 1 and len(chain) != 2 and len(chain) != 5:
			return self.fail("Expected 1 or 5 comparative forms", self.line)

		word.add(form | fm.COMPARATIVE, chain[0])
		if len(chain) == 2:
			ch = chain[1].split('/')
			if len(ch) != 4:
				return self.fail("Wrong comparison syntax")

			root = ch[0][:-2]
			chain = [chain[0], ch[0], root + ch[1], root + ch[2], root + ch[3]]

		if len(chain) == 5:
			form |= fm.SUPERLATIVE
			ch = inflect.adjective(word, form | fm.MASCULINE, chain[1])
			word.add_cases(form | fm.MASCULINE, ch)
			ch = inflect.adjective(word, form | fm.FEMININE, chain[2])
			word.add_cases(form | fm.FEMININE, ch)
			ch = inflect.adjective(word, form | fm.COMMON, chain[3])
			word.add_cases(form | fm.COMMON, ch)
			ch = inflect.adjective(word, form | fm.PLURAL, chain[4])
			word.add_cases(form | fm.PLURAL, ch)

		self.check_for_unexpected_data()

	def scan_ptcp(self, word, form):
		chain = self.line_split()

		rf = False
		if form & fm.REFLEXIVE:
			rf = True
			form &= ~fm.REFLEXIVENESS

		elif chain[0][-2:] == 'ся':
			form |= fm.REFLEXIVE

		self.scan_adj_like(word, form, rf)
		if self.err:
			return

		if not (form & fm.PERFECT or form & fm.PASSIVE):
			line = self.line_split_next()
			if line is None:
				return

			self.scan_adj_like(word, form | fm.PAST, rf)
			if self.err:
				return

		self.check_for_unexpected_data()

	def scan_tran(self, word, form):
		chain = self.line_split()

		rf = False
		if form & fm.REFLEXIVE and chain[0][-2:] != 'сь':
			rf = True
			form &= ~fm.REFLEXIVENESS

		if len(chain) >= 1:
			word.add(form, chain[0])
			if rf:
				word.add(form | fm.REFLEXIVE, chain[0] + 'сь')

		if len(chain) >= 2:
			word.add(form | fm.PAST, chain[1])
			if rf:
				word.add(form | fm.REFLEXIVE | fm.PAST, chain[1] + 'шись')

		if len(chain) >= 3:
			word.add(form | fm.PERFECT, chain[2])
			if rf:
				word.add(form | fm.REFLEXIVE | fm.PERFECT, chain[2] + 'сь')

		self.check_for_unexpected_data()

	def scan_part(self, word, form):
		chain = self.line_split()
		word.add(form, chain[0])
		self.check_for_unexpected_data()

	def word_get(self, str):
		if str not in self.tokens:
			return

		for fd in self.tokens[str].fds:
			return fd[1]

	def print_words(self):
		for w in self.words:
			w.print_forms()

	def export_words(self, output):
		output.write("""#include "word.h"\n\n""")
		output.write("""Word words[] = {\n""")

		for w in self.words:
			output.write("{%d," % w.id)

			# Base #
			if len(w.base) == 0:
				output.write("0,")
			else:
				for b in w.base:
					output.write("%d," % b)
					break

			# Flags #
			output.write("%d," % w.flags)

			# Form count #
			output.write("%d," % len(w.formdata))

			# Forms #
			output.write("(Form[]){")
			for fd in w.formdata:
				output.write("%d," % fd.form)
			output.write("},")

			# Strings #
			output.write("(char*[]){")
			for fd in w.formdata:
				output.write("\"%s\"," % fd.data)
			output.write("},")

			# Tags, generals, etc #
			output.write("{")
			self.export_arr(w.tags, output)
			self.export_arr(w.gnzs, output)
			self.export_arr(w.syn, output)
			self.export_arr(w.ant, output)
			output.write("},")

			if len(w.marks):
				output.write("(uint16_t[]){")
				for m in w.marks:
					output.write("%d," % m)
				output.write("0}")
			else:
				output.write("0")

			output.write("},\n")

		output.write("""};\n\n""")

		# Generals and Marks #
		output.write("""WordID *ptcls[] = {\n""")
		for w in self.words:
			if not w.ptcls:
				output.write("0,")
				continue

			output.write("(WordID[]){")
			for p in w.ptcls:
				output.write("%d," % p)
			output.write("0},\n")
		output.write("};\n\n")

		output.write("""WordID *marks[] = {\n""")
		for m in self.marks:
			if not m:
				output.write("0,")
				continue

			output.write("(WordID[]){")
			for id in m:
				output.write("%d," % id)
			output.write("0},\n")
		output.write("};\n\n")

		self.export_tokens(output)

	def export_arr(self, arr, output):
		if len(arr) == 0:
			output.write("0,")
			return

		output.write("(WordID[]){")

		for id in arr:
			output.write("%d," % id)

		output.write("0},")

	def export_tokens(self, output):
		output.write("""#include "token.h"\n\n""")
		output.write("""Token *tokens[] = {\n""")

		for entry in self.token_table:
			if entry is None:
				output.write("0,\n")
				continue

			output.write("(Token[]){")
			for tok in entry:
				output.write("{")
				output.write("\"%s\"," % tok.str)
				output.write("(Form[]){")
				for fd in tok.fds:
					output.write("%d," % fd[0])
				output.write("0},")

				output.write("(WordID[]){")
				for fd in tok.fds:
					output.write("%d," % fd[1])
				output.write("0}},")

			output.write("{}},\n")

		output.write("""};""")

	def export_header(self, output):
		output.write("#ifndef EMULOS_VOCAB_H_\n")
		output.write("#define EMULOS_VOCAB_H_\n\n")

		output.write("#define WORDS_TOTAL %d\n" % len(self.words))
		output.write("#define TOKENS_TOTAL %d\n\n" % len(self.token_table))

		output.write("#endif")
