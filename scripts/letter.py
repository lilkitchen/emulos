#
# letter.py
#

def is_vowel(c):
	if c == 'а' or c == 'о' or c == 'е' or c == 'у' or c == 'и' or \
		c == 'ы' or c == 'э' or c == 'ю' or c == 'я':
		return True
	return False

def is_hushing(c):
	if c == 'ш' or c == 'щ' or c == 'ч' or c == 'ж':
		return True
	return False

def is_beforei(c):
	if is_hushing(c) or c == 'к' or c == 'г':
		return True
	return False
