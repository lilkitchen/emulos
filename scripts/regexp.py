#
# regexp.py
#

# keep in line with src/regexp.h

START = 0x01
END = 0x02

REF = 0
ONE = 1
ANY = 2
OR = 3
TARGET = 4

SB = {
	ONE: '.',
	ANY: '*',
	OR: '|',
	TARGET: '?',
}

class Regexp:
	def __init__(self):
		self.flags = 0
		self.any_count = 0
		self.pats = []

	def scan(self, str):
		if str[0] == '^':
			self.flags |= START
			str = str[1:]

		if str[-1] == '$':
			self.flags |= END
			str = str[:-1]

		if len(str) == 0:
			return "Empty regexp"

		chain = str.split()
		for e in chain:
			if SB[OR] in e:
				self.pats.append(Or(e.split(SB[OR])))

			elif e[0] == SB[ONE]:
				self.pats.append(One())

			elif e[0] == SB[ANY]:
				if self.pats[-1].type == ANY:
					return "Consecutive *"
				self.pats.append(Any())
				self.any_count += 1

			elif e[0] == SB[TARGET]:
				self.pats.append(Target())

			else:
				self.pats.append(Ref(e))

		if self.pats[0].type == ANY:
			if self.flags & START:
				self.flags &= ~START
			del self.pats[0]

		if self.pats[-1].type == ANY:
			if self.flags & END:
				self.flags &= ~END
			self.pats.pop()

		# keep in line with src/regexp.c #
		if self.any_count >= 16:
			return "Too many * in regexp"

	def resolve(self, expr):
		for pat in self.pats:
			pat.resolve(expr)

	def get_unresolved_names(self):
		names = ""
		for pat in self.pats:
			name = pat.resolve_check()
			if name:
				names += name + ' '

		if len(names) > 0:
			return names

	def hashsum(self):
		hash = (self.flags + 2) * (self.any_count + 3)
		for pat in self.pats:
			hash *= pat.hashsum()
			hash = hash >> 1
		return hash

	def print(self):
		if self.flags & START:
			print('^', end=" ")

		for pat in self.pats:
			pat.print()
			print(" ", end="")

		if self.flags & END:
			print('$', end="")
		print('\'', end="")

	def export(self, output):
		ln = len(self.pats)
		output.write(".regexp=&(Regexp){%d,%d,%d," %
			(self.flags, ln, ln - self.any_count))
		output.write("(Pattern[]){")
		for pat in self.pats:
			pat.export(output)
		output.write("}}")

class Pattern:
	def __init__(self, type):
		self.type = type

	def resolve(self, expr):
		pass

	def resolve_check(self):
		pass

	def hashsum(self):
		return self.type

	def print(self):
		print(SB[self.type])

	def export(self, output):
		output.write("{%d}," % self.type)

class Ref(Pattern):
	def __init__(self, data):
		super().__init__(REF)
		self.data = data

	def resolve(self, expr):
		if self.data == expr[0]:
			self.data = expr[1]

	def resolve_check(self):
		if isinstance(self.data, str):
			return self.data

	def hashsum(self):
		return self.data.hashsum()

	def print(self):
		print(self.data.index, end="")

	def export(self, output):
		output.write("{%d,.index=%d}," % (self.type, self.data.index))

class One(Pattern):
	def __init__(self):
		super().__init__(ONE)

class Any(Pattern):
	def __init__(self):
		super().__init__(ANY)

class Or(Pattern):
	def __init__(self, data):
		super().__init__(OR)
		self.data = []
		for pat in data:
			self.data.append(Ref(pat))

	def resolve(self, expr):
		for pat in self.data:
			pat.resolve(expr)

	def resolve_check(self):
		for pat in self.data:
			pat.resolve_check()

	def hashsum(self):
		hash = 0
		for pat in self.data:
			hash += pat.hashsum()
			hash = hash >> 1
		return hash

	def print(self):
		for pat in self.data:
			pat.print()

	def export(self, output):
		output.write("{%d,%d,.indices=(uint16_t[]){" %
				(self.type, len(self.data)))
		for pat in self.data:
			pat.export(output)
		output.write("}")

class Target(Pattern):
	def __init__(self):
		super().__init__(TARGET)
