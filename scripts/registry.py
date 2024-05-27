#
# registry.py
#

class Registry:
	def __init__(self, init=1):
		self.count = init
		self.data = {}
		self.occur = {}

	def inc(self):
		self.count += 1

	def reg(self, val):
		if val in self.data:
			self.occur[val] += 1
			return self.data[val]

		self.data[val] = self.count
		self.occur[val] = 1
		self.inc()
		return self.data[val]

	def len(self):
		return self.count

	def check_occur(self):
		for key, val in self.occur.items():
			if val == 1:
				print("Warning: one occurance of \"%s\"" % key)

	def print(self):
		print(self.data)

class RegistryBit(Registry):
	def __init__(self, init=1):
		super().__init__(init)

	def inc(self):
		self.count *= 2
