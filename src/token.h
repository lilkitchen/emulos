/*
 * token.h
 */

#ifndef EMULOS_TOKEN_H_
#define EMULOS_TOKEN_H_

#include "word.h"

typedef struct token Token;

struct token {
	char *str;
	Form *forms;
	WordID *ids;
};

extern Token *tokens[];

Token *token_create(char *str);
void token_destroy(Token *token);

Token *token_get(char *str);

Token *token_copy(Token *token);

WordID token_find_word(Token *token, WordID id);
WordID token_find_extra(Token *token, WordID id, int ex);
WordID token_find_form(Token *token, Form form);
WordID token_find_mark(Token *token, Mark mark);

WordForm token_get_random_word_form(Token *token);

WordID token_random_id(Token *token);

#endif
