#
# stack.py
#

class Stack:
	def __init__(self):
		self.data = []
		self.names = []
		self.max = 0

		self.opened = False

	def push(self, expr):
		if expr[0] is None and len(self.names) and self.names[-1] is not None:
			expr = (self.names[-1], expr[1])

		if self.opened:
			self.data[-1].append(expr)
			return

		self.data.append(expr)
		self.names.append(None)

	def pop(self):
		self.data.pop()
		self.names.pop()

	def open(self):
		self.data.append([])
		self.names.append(None)

		self.opened = True

	def close(self):
		self.opened = False

	def name_set(self, name):
		self.names[-1] = name

	def update_max(self):
		depth = 0
		for expr in self.walk():
			depth += 1

		if self.max < depth:
			self.max = depth

	def len(self):
		return len(self.data)

	def walk(self):
		for expr in reversed(self.data):
			if isinstance(expr, tuple):
				yield expr
				continue

			for e in expr:
				yield e
