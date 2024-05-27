#
# token.py
#

import hash

class Token:
	def __init__(self, str):
		self.str = str
		self.hash = hash.sum(str)
		self.fds = []

	def print(self):
		print(self.str, self.hash, ":")
		for fd in self.fds:
			print(fd[1], ",", end="")
		print("")

def tokenize(words):
	hashes = set()
	tokens = {}

	for i, w in enumerate(words):
		for fd in w.formdata:
			str = fd.data

			if str not in tokens:
				tok = tokens[str] = Token(str)
				if tok.hash in hashes:
					print("Hash collision:", tok.str)
					return {}
				hashes.add(tok.hash)

			tok = tokens[str]
			tok.fds.append((fd.form, i))
	return tokens

def table_create(tokens):
	tl = len(tokens)
	table = [None] * tl

	for name, tok in tokens.items():
		i = tok.hash % tl
		if table[i] is None:
			table[i] = []
		table[i].append(tok)

	return table
