Actions
=======

Syntax:

	-> [<*] <data>[-form][-pg] [effect] [>*]

Number of angle brackets on the sides indicate position of the master word,
which form will be inherited

-pg is partner gender flag affecting gender form of the data

Action data types:
	- "literal"
	- .word
	- #tag
	- ^generalization
	- ~synonym
	- \`antonym
	- @mark
	- VARIABLE
	- #VARIABLE_TAG
	- ^VARIABLE_GENERALIZATION
	- ~VARIABLE_SYNONYM
	- \`VARIABLE_ANTONYM
	- ##VARIABLE_TAG_OF_TAG
	- ^^VARIABLE_PATICULAR_OF_GENERALIZATION
	- @@VARIABLE_WORD_OF_MARK
	- #VARIABLE #cross_tag
	- #tag; #or_tag

Action effect types:
	- $label
	- %flag_set
	- %!flag_clear

Examples:

-> ""
-> #music			// music related word
-> @music_genre		// music genre
-> ^VARIABLE		// generalization of VARIABLE
-> "Two words: " -> VAR-noun-acc -> " and " -> VAR2-noun-acc
