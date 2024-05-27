#
# hash.py
#

MASK = 0xffffffffffffffff
SEED = 0

def sum(str):
	hash = SEED

	data = bytes(str, "utf-8")
	for c in data:
		hash = c + c * hash + (hash >> 3) + ((hash << 15) & MASK)
		hash &= MASK

	return hash
