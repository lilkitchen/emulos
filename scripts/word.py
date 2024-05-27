#
# word.py
#

import form as fm
import symbol as sb

# keep in line with src/word.h

ANIMATED = 0x01

FLAGS = {
	"animated": ANIMATED,
}

class FormData:
	def __init__(self, form, data):
		self.form = form
		self.data = data

class Word:
	def __init__(self, id):
		self.id = id
		self.flags = 0

		self.formdata = []
		self.tags = []
		self.gnzs = []
		self.syn = []
		self.ant = []
		self.marks = []
		self.base = []

		self.ptcls = []

	def add(self, form, data):
		if not (form & fm.ENGLISH):
			form |= fm.RUSSIAN
		self.formdata.append(FormData(form, data))

	def add_cases(self, form, chain):
		self.add(form | fm.NOMINATIVE, chain[0])
		self.add(form | fm.GENITIVE, chain[1])
		self.add(form | fm.DATIVE, chain[2])
		self.add(form | fm.ACCUSATIVE, chain[3])
		self.add(form | fm.INSTRUMENTAL, chain[4])
		self.add(form | fm.ADPOSITIONAL, chain[5])

	def add_verb(self, form, data, rf, suf):
		self.add(form, data)
		if rf:
			self.add(form | fm.REFLEXIVE, data + suf)

	def read_flags(self, str):
		chain = str.lstrip(sb.WFLAG).split(sb.WFLAG)
		chain = [f.strip() for f in chain]

		for f in chain:
			if f not in FLAGS:
				return "Unknown word flag"
			self.flags |= FLAGS[f]

	def is_empty(self):
		if len(self.formdata) == 0:
			return True
		return False

	def print(self):
		for fd in self.formdata:
			print(fd.data, end=", ")
		print("\t", end="")
		for t in self.tags:
			print(t, end=", ")
		print("\t", end="")
		for g in self.gnzs:
			print(g, end=", ")
		print("\t", end="")
		for syn in self.syn:
			print(syn, end=", ")
		print("\t", end="")
		for ant in self.ant:
			print(ant, end=", ")
		print("\t", end="")
		for mark in self.marks:
			print(mark, end=", ")
		print("\t", end="")
		print(self.base, end=", ")
		print("\n")

	def print_forms(self):
		for fd in self.formdata:
			print(fm.stringify(fd.form))
			print(fd.data)
