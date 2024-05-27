#
# action.py
#

import branch as br
import form as fm
import selector as sl
import symbol as sb

# keep in line with scr/action.h

REF = 0x01
DOUBLE = 0x02
SILENT = 0x04
LFLAG = 0x08
LFLAGNOT = 0x10
PG = 0x20

class Action:
	def __init__(self):
		self.id = 0
		self.args = []

		self.selectors = []

	def scan(self, str):
		if len(str) == 0:
			return "Empty action"

		chain = str.split(sb.ACTION)
		chain = [i.strip() for i in chain]

		for act in chain:
			arg = Arg()

			# Silence #
			if act[0] == sb.SILENT:
				if act[-1] != sb.SILENT_END:
					return "Unclosed silent action"
				arg.flags |= SILENT
				act = act[1:-1]

			# Master #
			while act[0] == sb.MASTERL:
				arg.master -= 1
				act = act[1:]

			while act[-1] == sb.MASTERR:
				arg.master += 1
				act = act[:-1]

			act = act.strip()

			# Effects #
			label_count = act.count(sb.LABEL)
			lflag_count = act.count(sb.LFLAG)

			if label_count or lflag_count:
				if label_count + lflag_count > 1:
					return "Too many effects"

				sign = 0
				n = 0
				if label_count:
					sign = act.find(sb.LABEL)
				else:
					sign = act.find(sb.LFLAG)
					if act[sign + 1] == sb.NOT:
						arg.flags |= LFLAGNOT
						n = 1
					arg.flags |= LFLAG

				arg.effect = act[sign + n + 1:]
				act = act[:sign].rstrip()

			# Data #
			if len(act):
				err = arg.scan(act)
				if err:
					return err

			self.args.append(arg)

		# TODO: check for slaves correctness

	def resolve_args(self, stack, labels_reg, vocab):
		for expr in stack.walk():
			self.selectors.insert(0, expr[1])

			for arg in self.args:
				err = arg.resolve(expr, labels_reg, vocab)
				if err:
					return err

		for arg in self.args:
			if arg.type == sl.LIT:
				continue
			if isinstance(arg.data, str):
				return ("Failed to resolve action arg ", arg.data)

	def add(self, root):
		sl.sort(self.selectors)

		self.add_to_branch(root)

	def add_to_branch(self, b):
		if len(self.selectors) == 0:
			b.actions.append(self)
			return

		if self.selectors[0] in b.branches:
			branch = b.branches[self.selectors[0]]

		else:
			branch = br.Branch()
			b.branches[self.selectors[0]] = branch

		del self.selectors[0]
		self.add_to_branch(branch)

	def effects_num_get(self):
		efn = 0
		for arg in self.args:
			if arg.effect:
				efn += 1
		return efn

	def export(self, output):
		output.write("(Action[]){")

		for arg in self.args:
			arg.export(len(self.args), output)

		output.write("},\n")

class Arg:
	def __init__(self):
		self.type = 0
		self.flags = 0

		self.master = 0

		self.data = None
		self.form = 0

		self.effect = 0

	def scan_group(self, type, chain):
		self.type = type
		self.data = []
		for c in chain:
			child = Arg()
			err = child.scan(c)
			if err:
				return err
			self.data.append(child)

	def scan(self, str):
		if len(str) == 0:
			return

		if str[0] == sb.LIT:
			if str[-1] != sb.LIT:
				return "Wrong literal"
			self.type = sl.LIT
			self.data = str[1:-1]
			return

		# Or #
		chain = str.split(sb.OR)
		chain = [i.strip for i in chain]
		if len(chain) > 1:
			err = self.scan_group(sl.OR, chain)
			if err:
				return err
			return

		# Cross #
		chain = str.split()
		if len(chain) > 1:
			err = self.scan_group(sl.CROSS, chain)
			if err:
				return err
			return

		# Form #
		sign = str.find(sb.FORM)
		if sign >= 0:
			form_str = str[sign:]
			chain = form_str.split(sb.FORM)
			if chain[-1] == "pg":
				self.flags |= PG
				form_str = form_str[:-3]
			self.form = fm.scan(form_str)
			if self.form is None:
				return "Wrong form"
			str = str[:sign]

		if len(str) == 0:
			return "Form without data"

		# Data #
		if str[0] == sb.WORD:
			self.type = sl.WORD
			self.data = str[1:]

		elif str[0] == sb.TAG:
			if len(str) > 1 and str[1] == sb.TAG:
				self.flags |= DOUBLE
				str = str[1:]

			self.type = sl.TAG
			self.data = str[1:]

		elif str[0] == sb.GNZ:
			if len(str) > 1 and str[1] == sb.GNZ:
				self.flags |= DOUBLE
				str = str[1:]

			self.type = sl.GNZ
			self.data = str[1:]

		elif str[0] == sb.SYN:
			self.type = sl.SYN
			self.data = str[1:]

		elif str[0] == sb.ANT:
			self.type = sl.ANT
			self.data = str[1:]

		elif str[0] == sb.MARK:
			if len(str) > 1 and str[1] == sb.MARK:
				self.flags |= DOUBLE
				str = str[1:]

			self.type = sl.MARK
			self.data = str[1:]

		else:
			self.type = sl.WORD
			self.data = str

		if len(self.data) == 0:
			return "Empty action"

		if self.data[0].isupper():
			self.flags |= REF

	def resolve(self, expr, labels_reg, vocab):
		if self.type == sl.LIT:
			return

		if self.type == sl.OR or self.type == sl.CROSS:
			for arg in self.data:
				arg.resolve(expr, labels_reg, vocab)
			return

		if self.flags & REF and self.data == expr[0]:
			self.data = expr[1]

		if self.effect and not isinstance(self.effect, int):
			if self.flags & LFLAG:
				self.effect = vocab.lflags_reg.reg(self.effect)
			else:
				self.effect = labels_reg.reg(self.effect)

		if self.type == 0 or self.flags & REF or \
			not isinstance(self.data, str):
			return

		if self.type == sl.MARK:
			self.data = vocab.marks_reg.reg(self.data)

		else:
			word_str = self.data.strip()
			self.data = vocab.word_get(word_str)
			if self.data is None:
				return ("Word not found in action", word_str)

	def export(self, ln, output):
		output.write("{%d,%d,%d,%d," %
			(self.type, self.flags, self.master, ln))

		if self.flags & REF:
			output.write(".index=%d," % self.data.index)

		elif self.type == sl.OR or self.type == sl.CROSS:
			output.write(".children=(Action[]){")
			for arg in self.data:
				arg.export(len(self.data), output)
			output.write("},")

		elif self.type == sl.LIT:
			output.write(".str=\"%s\"," % self.data)

		elif self.type == sl.MARK:
			output.write(".mark=%d," % self.data)

		elif self.type:
			output.write(".id=%d," % self.data)
		else:
			output.write(".id=0,")

		output.write("%d," % self.form)

		if self.effect:
			output.write(".e=%d" % self.effect)

		output.write("},")
