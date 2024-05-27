#
# selector.py
#

import form as fm
import hash as hs
import regexp as re
import symbol as sb

# keep in line with src/selector.h

LIT = 1
WORD = 2
TAG = 3
GNZ = 4
SYN = 5
ANT = 6
MARK = 7

REF = 8

OR = 9
AND = 10
CROSS = 11
REGEXP = 12

UNREC = 13

FORM = 14
ROLE = 15
LABEL = 16
LFLAG = 17
WFLAG = 18

NOT = 0x01
TRUMP = 0x02
SELF = 0x04

PREF = {
	sb.LIT: LIT,
	sb.WORD: WORD,
	sb.TAG: TAG,
	sb.GNZ: GNZ,
	sb.SYN: SYN,
	sb.ANT: ANT,
	sb.MARK: MARK,

	sb.REGEXP: REGEXP,
	sb.UNREC: UNREC,

	sb.FORM: FORM,
	sb.ROLE: ROLE,
	sb.LABEL: LABEL,
	sb.LFLAG: LFLAG,
	sb.WFLAG: WFLAG,
}

PREF_FLAG = {
	sb.NOT: NOT,
	sb.TRUMP: TRUMP,
	sb.SELF: SELF,
}

# keep in line with src/message.h

MSG_ROLE = {
	"q": 0x0002,
	"ex": 0x0004,
	"period": 0x0008,
	"comma": 0x0010,
	"colon": 0x0020,
	"semicolon": 0x0040,
	"dash": 0x0080,
	"digits": 0x0100,

	"in_q": 0x0200,
	"in_ex": 0x0400,

	"laugh": 0x0800,
	"scream": 0x1000,
}

class Selector:
	def __init__(self, type=0):
		self.type = type
		self.flags = 0
		self.start = 0
		self.range = 1
		self.data = None
		if type == OR or type == CROSS:
			self.data = []

		self.occur = 0
		self.index = 0

	def scan(self, str, mind):
		if sb.RANGE in str:
			ch = str.split(sb.RANGE)
			if len(ch) > 3:
				return mind.fail("Wrong selector range", str)

			a = 0
			b = 0

			try:
				a = int(ch[1])
			except ValueError:
				return mind.fail("Wrong selector range number", str)
			self.range = a

			if len(ch) == 3:
				try:
					b = int(ch[2])
				except ValueError:
					return mind.fail("Wrong selector range number", str)
				self.start = a
				self.range = b

			str = str[:str.find(sb.RANGE)]

			if self.range > mind.range_max:
				mind.range_max = self.range

		while str[0] in PREF_FLAG:
			self.flags |= PREF_FLAG[str[0]]
			str = str[1:]

		if str[0] in PREF:
			self.type = PREF[str[0]]
			str = str[1:]

		else:
			if not str[0].isalpha():
				return mind.fail("Wrong selector prefix", str)

			self.type = REF

		err = self.scan_data(str)
		if err:
			return mind.fail(err, str)

		# Resolve #
		if self.type == LABEL:
			self.data = mind.labels_reg.reg(self.data)

		elif self.type == MARK:
			self.data = mind.vocab.marks_reg.reg(self.data)

		elif self.type == LFLAG:
			self.data = mind.vocab.lflags_reg.reg(self.data)

		for expr in mind.stack.walk():
			self.resolve(expr)

		# TODO: resolve check

	def resolve(self, expr):
		if self.type == OR or self.type == CROSS:
			for s in self.data:
				s.resolve(expr)

		elif self.type == REGEXP:
			self.data.resolve(expr)

		elif self.type == REF:
			if expr[0] == self.data:
				self.data = expr[1]

	def scan_data(self, str):
		if self.type == UNREC:
			return

		elif self.type == REGEXP:
			if str[-1] != sb.REGEXP:
				return "Unclosed regexp"

			self.data = re.Regexp()
			err = self.data.scan(str[:-1])
			if err:
				return err
			return

		elif self.type == LIT:
			if str[-1] != sb.LITERAL:
				return "Unclosed literal"

			str = str[:-1]

		elif self.type == FORM:
			self.data = fm.scan(str)
			if self.data is None:
				return "Wrong form format"
			return

		elif self.type == ROLE:
			self.data = 0
			chain = str.split(sb.ROLE)
			for r in chain:
				if r not in MSG_ROLE:
					return "Unknown role"
				self.data |= MSG_ROLE[r]
			return

		self.data = str
		return

	def hashsum(self):
		hash = (self.type * 3) * (self.flags * 123 + 13) * (self.range * 13 + 12)

		if self.type == UNREC:
			hash += 12345

		elif self.type == OR or self.type == CROSS:
			for child in self.data:
				hash += child.hashsum()
				hash = hash >> 3

		elif self.type == REGEXP or self.type == REF:
			hash += self.data.hashsum()

		elif self.type == FORM or self.type == LABEL or \
			self.type == ROLE or self.type == MARK or self.type == LFLAG:
			hash *= self.data * 13 + 123

		else:
			hash += hs.sum(self.data)

		return hash & hs.MASK

	def print(self):
		t = ""
		for p in PREF:
			if PREF[p] == self.type:
				t = p

		print(t, end="")
		if self.type == OR:
			print("(", end="")
			for sel in self.data:
				sel.print()
			print(")", end="")

		elif self.type == CROSS:
			print("[", end="")
			for sel in self.data:
				sel.print()
			print("]", end="")

		elif self.type == REGEXP:
			self.data.print()

		elif self.type == REF:
			print("ref:", self.data, end="")

		else:
			print(self.data, end="")

		print("; ", end="")

	def export(self, vocab, output):
		output.write("{%d,%d,%d,%d," %
			(self.type, self.flags, self.start, self.range))

		if self.type == UNREC:
			output.write(".id=0")

		elif self.type == OR or self.type == CROSS:
			output.write(".children=(int[]){")
			for sel in self.data:
				output.write("%d," % sel.index)
			output.write("0}")

		elif self.type == REGEXP:
			self.data.export(output)

		elif self.type == REF:
			output.write(".ref=%d" % self.data.index)

		elif self.type == LIT:
			output.write(".literal=\"%s\"" % self.data)

		elif self.type == FORM:
			output.write(".form=%d" % self.data)

		elif self.type == ROLE:
			output.write(".role=%d" % self.data)

		elif self.type == LABEL:
			output.write(".label=%d" % self.data)

		elif self.type == LFLAG:
			output.write(".lflag=%d" % self.data)

		elif self.type == MARK:
			output.write(".mark=%d" % self.data)

		else:
			word = vocab.word_get(self.data)
			if not word:
				return ("Word not found", self.data)

			output.write(".id=%d" % word)

		output.write("},")

def sort(arr):
	if len(arr) <= 1:
		return

	max = len(arr) - 1
	step = max
	i = 0
	while True:
		if arr[i].occur < arr[i + step].occur:
			arr[i], arr[i + step] = arr[i + step], arr[i]
		i += 1
		if i + step > max:
			if step == 1:
				break
			step = int(step / 1.25)
			i = 0

def dedup(arr):
	sort(arr)

	n = 0
	for sel in reversed(arr):
		if sel.occur < n:
			sel.occur = n
		n = sel.occur + 1

	i = 1
	for sel in arr:
		sel.index = i
		i += 1
