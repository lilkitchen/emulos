#
# reader.py
#

import file as fl

class Reader:
	def __init__(self, paths):
		self.ln = 0
		self.line = None

		self.input = None

		self.path = 0
		self.paths = paths

		self.err = False
		self.err_msg = ""
		self.err_data = None

	def readline(self):
		if self.input is None:
			if self.path == len(self.paths):
				self.line = None
				return

			self.input = fl.fopen(self.paths[self.path], "r")
			if self.input is None:
				return self.fail("Failed to open input file",
					self.paths[self.path])
			self.ln = 0

		self.ln += 1
		self.line = self.input.readline()
		if self.line == "":
			self.input.close()
			self.input = None
			self.path += 1
			self.line = '\n'

		return self.line

	def fail(self, msg, data=None):
		self.err = True
		self.err_msg = msg
		self.err_data = data

	def print_err(self):
		file_name = "--"
		if self.path < len(self.paths):
			file_name = self.paths[self.path]

		print(self.err_msg, "in", file_name, "at line", self.ln, end="")
		if self.err_data is not None:
			print(" >>>", self.err_data, end="")
		print("")
