/*
 * token.c
 */

#include <malloc.h>
#include <string.h>

#include "build/vocab.h"

#include "emulos.h"
#include "hash.h"
#include "rng.h"
#include "utf8.h"
#include "token.h"

#define TOKEN_LEN_LIMIT 80

Token *token_create(char *str)
{
	int size = sizeof(Token) + strlen(str) + 1;
	void *buffer = malloc(size);
	if (buffer == NULL)
		emulos_fail(OUT_OF_MEMORY, "Token creation fail\n");
	memset(buffer, 0, size);

	Token *token = buffer;
	token->str = buffer + sizeof(Token);
	strcpy(token->str, str);

	return token;
}

void token_destroy(Token *token)
{
	if (token == NULL || token->ids)
		return;

	free(token);
}

static inline Token *token_find(char *str)
{
	Hash hash = hashsum(str);

	Token *token = tokens[hash % TOKENS_TOTAL];
	if (token)
		for (; token->str; token++)
			if (strcmp(token->str, str) == 0)
				return token;
	return NULL;
}

Token *token_get(char *str)
{
	utf8_lower(str);

	Token *token = token_find(str);
	if (token)
		return token;

	/* Recognize */
	char buf[TOKEN_LEN_LIMIT + 1 + 4] = {0}; // padding for unrepeat
	strcpy(buf, str);

	utf8_unrepeat(buf);
	if (buf[2] == '\0')
		goto end;

	token = token_find(buf);
	if (token)
		return token;

	strcpy(buf, str);

	int len = strlen(str);
	if (len >= TOKEN_LEN_LIMIT)
		goto end;

	int chip = len / 4;
	if (chip == 0)
		goto end;

	for (int i = 0; i <= chip; i++) {
		buf[len - i] = '\0';
		for (int k = 0; k <= chip; k++) {
			token = token_find(buf + k);
			if (token)
				return token;
		}
	}
end:
	return token_create(str);
}

Token *token_copy(Token *token)
{
	if (token == NULL)
		return NULL;

	if (token->ids)
		return token;

	return token_create(token->str);
}

WordID token_find_word(Token *token, WordID id)
{
	if (token->ids == NULL)
		return 0;

	for (int i = 0; token->ids[i]; i++) {
		if (token->ids[i] == id)
			return id;

		Word *word = words + token->ids[i];
		if (word->base == id)
			return token->ids[i];
	}

	return 0;
}

WordID token_find_extra(Token *token, WordID id, int ex)
{
	if (token->ids == NULL)
		return 0;

	for (int i = 0; token->ids[i]; i++) {
		Word *word = words + token->ids[i];
		if (word_is_extra(word, id, ex))
			return word->id;
	}
	if (ex == EXTRA_TAGS) {
		WordID x = token_find_extra(token, id, EXTRA_GNZS);
		if (x)
			return x;
		x = token_find_extra(token, id, EXTRA_SYN);
		if (x)
			return x;
	}
	return 0;
}

WordID token_find_form(Token *token, Form form)
{
	if (token->forms == NULL)
		return 0;

	Form cls = form & FORM_CLASS;
	for (int i = 0; token->forms[i]; i++) {
		if (cls && (token->forms[i] & FORM_CLASS) != cls)
			continue;

		if (form_contains(token->forms[i], form))
			return token->ids[i];
	}
	return 0;
}

WordID token_find_mark(Token *token, Mark mark)
{
	if (token->ids == NULL)
		return 0;

	for (int i = 0; token->ids[i]; i++) {
		Word *word = words + token->ids[i];
		if (word_is_mark(word, mark))
			return word->id;
	}
	return 0;
}

WordForm token_get_random_word_form(Token *token)
{
	if (token->ids == NULL)
		return (WordForm){0};

	int len = 0;
	for (; token->ids[len]; len++);

	int rn = rng_int(len);

	return (WordForm){
		.id = token->ids[rn],
		.form = token->forms[rn],
		.str = token->str
	};
}

WordID token_random_id(Token *token)
{
	if (token->ids == NULL)
		return 0;

	int len = 0;
	for (; token->ids[len]; len++);

	return token->ids[rng_int(len)];
}
