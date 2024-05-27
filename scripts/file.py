#
# file.py
#

import os

def fopen(path, mode):
	try:
		file = open(path, mode, encoding="utf-8")
	except IOError:
		print("Failed to open " + path)
		return
	return file

def older(path, paths):
	if not os.path.isfile(path):
		return True

	mtime = os.path.getmtime(path)
	for p in paths:
		if mtime < os.path.getmtime(p):
			return True
	return False
