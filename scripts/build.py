#
# build.py
#

import sys

import file as fl
import mind as md
import vocab as vc

VOCAB_C = "build/vocab.c"
VOCAB_H = "build/vocab.h"
MIND_C = "build/mind.c"
MIND_H = "build/mind.h"

def vocab_make(paths):
	vocab = vc.Vocab(paths)
	if vocab.err:
		vocab.print_err()
		sys.exit(1)
	return vocab

def main():
	script_paths = []
	vocab_paths = []
	mind_paths = []
	for path in sys.argv[1:]:
		if path[-1] == 'y':
			script_paths.append(path)

		elif path[-1] == 'b':
			vocab_paths.append(path)

		elif path[-1] == 'd':
			mind_paths.append(path)

	vocab = None

	if fl.older(VOCAB_C, script_paths + vocab_paths):
		vocab = vocab_make(vocab_paths)

		# Words #
		output = fl.fopen(VOCAB_C, "w")
		if not output:
			sys.exit(1)
		vocab.export_words(output)
		output.close()
		if vocab.err:
			vocab.print_err()
			sys.exit(1)

		# Header #
		output = fl.fopen(VOCAB_H, "w")
		if not output:
			sys.exit(1)
		vocab.export_header(output)
		output.close()
		if vocab.err:
			vocab.print_err()
			sys.exit(1)

	if fl.older(MIND_C, script_paths + vocab_paths + mind_paths):
		if len(mind_paths) == 0:
			return

		if vocab is None:
			vocab = vocab_make(vocab_paths)
			if vocab.err:
				vocab.print_err()
				sys.exit(1)

		mind = md.Mind(mind_paths, vocab)
		if mind.err:
			mind.print_err()
			sys.exit(1)

		output = fl.fopen(MIND_C, "w")
		if not output:
			sys.exit(1)
		mind.export(output)
		output.close()
		if mind.err:
			mind.print_err()
			sys.exit(1)

		# Header #
		output = fl.fopen(MIND_H, "w")
		if not output:
			sys.exit(1)
		mind.export_header(output)
		output.close()
		if mind.err:
			mind.print_err()
			sys.exit(1)

if __name__ == "__main__":
	main()
