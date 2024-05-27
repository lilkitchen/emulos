/*
 * parse.c
 */

#include <malloc.h>
#include <string.h>

#include "parse.h"

#define TOKEN_LEN_LIMIT 64
#define MESSAGE_LEN_LIMIT 128

static char buf[TOKEN_LEN_LIMIT];
static Token *dictum[MESSAGE_LEN_LIMIT];

#define ADD() \
	if (dp == MESSAGE_LEN_LIMIT) \
		goto fail; \
	buf[bp] = '\0'; \
	dictum[dp++] = token_get(buf); \
	bp = 0

Message *parse(char *str)
{
	int bp = 0;
	int dp = 0;

	char c;
parse:
	switch (c = *str) {
	case '\0':
		if (bp == 0)
			break;
		str--;
		goto add;

	case ' ': case '\t': case '\n':
		if (bp == 0)
			goto next;
		goto add;

	case ',': case '!': case '?': case '.': case ':': case ';':
	case '/': case '(': case ')': case '[': case ']': case '{': case '}':
	case '+': case '=': case '_': case '*': case '%':
		ADD();
		bp = 1;
		buf[0] = c;
add:
		ADD();
next:
		str++;
		goto parse;

	default:
		if (bp == TOKEN_LEN_LIMIT)
			goto fail;

		buf[bp++] = c;
		str++;
		goto parse;
	}

	dictum[dp] = NULL;

	return message_create(dictum);
fail:
	for (int i = 0; i < dp; i++)
		token_destroy(dictum[i]);

	return NULL;
}
