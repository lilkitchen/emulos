#
# mind.py
#

import reader

import action as ac
import branch as br
import registry as rg
import selector as sl
import stack as st
import symbol as sb

class Mind(reader.Reader):
	def __init__(self, input, vocab):
		super().__init__(input)

		self.vocab = vocab
		self.selector_dict = {}
		self.selectors = []
		self.actions = []

		self.labels_reg = rg.Registry()

		self.action_args_max = 0
		self.action_effects_max = 0
		self.range_max = 0
		self.options_max = {}

		self.root = br.Branch()

		self.stack = st.Stack()

		self.scan()

	def scan(self):
		indent = 0
		str = ""
		while True:
			line = self.readline()
			if line is None:
				if self.err:
					return
				break

			line = line.rstrip()
			if len(line) == 0:
				continue

			if indent == 0:
				while line[indent] == '\t':
					indent += 1

				if indent == len(line):
					continue

			if line[indent] == ' ':
				return self.fail("Spaces in indentation")

			str += line.lstrip()

			if str[-1] == '\\':
				str = str[:-1]
				continue

			if line[-1] == sb.AND or line[-1] == sb.OR:
				continue

			if str[:2] == "//":
				str = ""
				indent = 0
				continue

			while indent < self.stack.len():
				self.stack.pop()

			if indent != self.stack.len():
				return self.fail("Superflous indentation")

			if str[:2] == sb.ACTION:
				if indent == 0:
					return self.fail("Orphan action")

				action = ac.Action()
				err = action.scan(str[2:].lstrip())
				if err:
					return self.fail(err, str[2:])

				if self.action_args_max < len(action.args):
					self.action_args_max = len(action.args)

				efn = action.effects_num_get()
				if self.action_effects_max < efn:
					self.action_effects_max = efn

				err = action.resolve_args(self.stack, self.labels_reg, self.vocab)
				if err:
					return self.fail(err[0], err[1])

				self.actions.append(action)

				sel_count = len(action.selectors)
				if sel_count not in self.options_max:
					self.options_max[sel_count] = 0
				self.options_max[sel_count] += 1

				self.stack.update_max()

			else:
				self.scan_predicates(str)
				if self.err:
					return

			str = ""
			indent = 0

		omax = 0
		for key, max in self.options_max.items():
			if max > omax:
				omax = max
		self.options_max = omax

		sl.dedup(self.selectors)

		for action in self.actions:
			err = action.add(self.root)
			if err:
				return self.fail(err)

		# Checks #
		self.vocab.marks_reg.check_occur()

	def scan_predicates(self, str):
		chain = str.split(sb.AND)
		if len(chain) - 1 != str.count(sb.AND):
			return self.fail("Wrong AND selector", str)
		chain = [i.strip() for i in chain]

		if len(chain) == 1:
			if str[-1] == sb.NAME:
				self.stack.name_set(str[:-1])
				return
		else:
			self.stack.open()

		for p in chain:
			expr = self.scan_expr(p)
			if self.err:
				return

			self.stack.push(expr)

		self.stack.close()

	def scan_expr(self, str):
		name = None
		if sb.NAME in str:
			if not str[0].isupper():
				return self.fail("Wrong variable name", str)
			colon = str.index(sb.NAME)
			name = str[:colon]
			str = str[colon + 1:].lstrip()

		sel = None
		if sb.OR in str:
			chain = str.split(sb.OR)
			if len(chain) - 1 != str.count(sb.OR):
				return self.fail("Wrong OR selector", str)
			chain = [i.strip() for i in chain]

			sel = self.scan_group(sl.OR, chain)
			if self.err:
				return
		else:
			sel = self.scan_selector(str)
			if self.err:
				return

		return (name, sel)

	def scan_group(self, type, chain):
		sel = sl.Selector(type)
		for c in chain:
			child = self.scan_selector(c)
			if self.err:
				return
			sel.data.append(child)

		sel = self.record_selector(sel)
		return sel

	def scan_selector(self, str):
		sel = None

		chain = str.split()
		if sb.REGEXP in str:
			if str[0] != sb.REGEXP or str.count(sb.REGEXP) > 2:
				return self.fail("Regexp in cross selector")

			if str[-1] == sb.REGEXP:
				chain = str
			else:
				re_end = str.rindex(sb.REGEXP)
				regexp = str[:str.re_end + 1]
				str = str[re_end + 1:]
				chain = regexp + str.split()

		if isinstance(chain, list) and len(chain) > 1:
			sel = self.scan_group(sl.CROSS, chain)
			if self.err:
				return
			return sel

		sel = sl.Selector()
		sel.scan(str, self)
		if self.err:
			return

		sel = self.record_selector(sel)
		return sel

	def record_selector(self, sel):
		if sel.type == sl.LABEL and sel.flags & sl.SELF:
			return self.fail("Label selector with self flag")

		hash = sel.hashsum()
		if hash in self.selector_dict:
			found = self.selector_dict[hash]
			if found.type != sel.type or found.data != sel.data \
				or found.flags != sel.flags or found.range != sel.range:
				return self.fail("Hash collision in selectors")
			found.occur += 1
			return found

		sel.occur += 1

		self.selector_dict[hash] = sel
		self.selectors.append(sel)
		return sel

	def print_expr(self, e):
		print(e[0], ": ", sep="", end="")
		e[1].print()

	def export(self, output):
		output.write("""#include "branch.h"\n\n""")
		output.write("""#include "selector.h"\n\n""")

		# Selectors #
		output.write("""Selector selectors[] = {\n{},\n""")
		for sel in self.selectors:
			err = sel.export(self.vocab, output)
			if err:
				return self.fail(err[0], err[1])
			output.write("\n")
		output.write("""};\n\n""")

		# Actions #
		output.write("""Action *actions[] = {\n0,\n""")
		act_id = 1
		for act in self.actions:
			act.export(output)
			act.id = act_id
			act_id += 1
		output.write("""};\n\n""")

		# Branches #
		output.write("""Branch root = {0, %d, 0,\n""" % self.root.len())

		output.write("""(Branch[]){\n""")
		for key in self.root.branches:
			self.export_branch(key, self.root.branches[key], output)
			if self.err:
				return
		output.write("""},0};""")

	def export_branch(self, sel, br, output):
		output.write("{")

		# Selector #
		output.write("%d," % sel.index)

		# Counts #
		output.write("%d,%d,\n" % (len(br.branches), len(br.actions)))

		# Branches #
		if len(br.branches):
			output.write("(Branch[]){\n")
			for key in br.branches:
				self.export_branch(key, br.branches[key], output)

			output.write("},\n")
		else:
			output.write("0,")

		# Actions #
		if len(br.actions):
			output.write("(int[]){\n")
			for action in br.actions:
				output.write("%d," % action.id)

			output.write("}")
		else:
			output.write("0")

		output.write("},")

	def export_header(self, output):
		output.write("#ifndef EMULOS_MIND_H_\n")
		output.write("#define EMULOS_MIND_H_\n\n")

		output.write("#define SELECTORS_TOTAL %d\n\n" % (len(self.selectors) + 1))

		output.write("#define FRAMES_LIMIT %d\n\n" % (self.stack.max + 1))

		output.write("#define ACTION_LEN_LIMIT %d\n" % (self.action_args_max + 1))
		output.write("#define ACTION_EFFECTS_LIMIT %d\n\n" %
			(self.action_effects_max + 1))

		output.write("#define HISTORY_LIMIT %d\n" % (self.range_max + 2))
		output.write("#define OPTIONS_LIMIT %d\n\n" % (self.options_max + 1))

		output.write("#endif")
