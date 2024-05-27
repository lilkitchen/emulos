#
# branch.py
#

class Branch:
	def __init__(self):
		self.branches = {}
		self.actions = []

	def len(self):
		return len(self.branches)
