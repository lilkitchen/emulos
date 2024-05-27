/*
 * message.c
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "emulos.h"
#include "utf8.h"
#include "message.h"

static void recognize(Message *msg);

Message *message_create(Token **dictum)
{
	int len = 0;
	for (; dictum[len]; len++);

	int size = sizeof(Message) * len;
	Message *msg = malloc(size);
	if (msg == NULL)
		emulos_fail(OUT_OF_MEMORY, "Message creation fail\n");
	memset(msg, 0, size);

	msg->len = len;

	for (int i = 0; i < len; i++)
		msg[i].token = dictum[i];

	for (int i = 0; i < len; i++) {
		Token *token = msg[i].token;

		switch (token->str[0]) {
		case '?': msg[i].role |= MSG_Q; break;
		case '!': msg[i].role |= MSG_EX; break;
		case '.': msg[i].role |= MSG_PERIOD; break;
		case ',': msg[i].role |= MSG_COMMA; break;
		case ':': msg[i].role |= MSG_COLON; break;
		case ';': msg[i].role |= MSG_SEMICOLON; break;
		case '-': msg[i].role |= MSG_DASH; break;
		}

		if (token->ids == NULL)
			recognize(msg + i);

		msg[i].role |= MSG_EST;
	}

	for (int i = 0; i < len; i++) {
		for (int k = i + 1; k < len; k++) {
			if (msg[k].role & MSG_PERIOD)
				break;

			if (msg[k].role & MSG_Q)
				msg[i].role |= MSG_IN_Q;

			if (msg[k].role & MSG_EX)
				msg[i].role |= MSG_IN_EX;
		}
	}

	return msg;
}

void message_destroy(Message *msg)
{
	for (int i = 0; i < msg->len; i++)
		token_destroy(msg[i].token);

	free(msg);
}

Message message_copy(Message *msg)
{
	if (msg == NULL)
		return (Message){0};

	Message copy = {
		.role = msg->role,
		.token = token_copy(msg->token)
	};
	return copy;
}

void message_print(Message *msg)
{
	if (msg == NULL) {
		printf("(nil)\n");
		return;
	}

	for (int i = 0; i < msg->len; i++) {
		printf("%s<", msg[i].token->str);

		if (msg[i].role & MSG_DIGITS)
			printf("0");
		if (msg[i].role & MSG_IN_Q)
			printf("?");
		if (msg[i].role & MSG_IN_EX)
			printf("!");

		printf("> ");
	}
	puts("");
}

static int recognize_laugh(Message *msg, char *str, char *a, char *b)
{
	if (utf8_match(str, a) || utf8_match(str, b)) {
		char *laugh = str;
		int count = 0;
		while (*laugh) {
			if (utf8_match(laugh, a) == 0 && utf8_match(laugh, b) == 0)
				break;
			laugh += 2;
			count++;
		}

		if (count >= 3) {
			msg->role |= MSG_LAUGH;
			return 1;
		}
	}
	return 0;
}

static void recognize(Message *msg)
{
	char *str = msg->token->str;

	; // TODO: digits

	if (utf8_match(str, "а")) {
		char *scream = str;
		while (*scream) {
			if (utf8_match(scream, "а") == 0)
				break;
			scream += 2;
		}

		if (*scream == '\0') {
			msg->role |= MSG_SCREAM;
			return;
		}
	}

	if (recognize_laugh(msg, str, "а", "х"))
		return;

	if (recognize_laugh(msg, str, "и", "х"))
		return;

	if (recognize_laugh(msg, str, "е", "х"))
		return;

	if (recognize_laugh(msg, str, "а", "з"))
		return;

	if (recognize_laugh(msg, str, "о", "л"))
		return;
}

Message message_dummy = {
	.role = MSG_EST,
	.token = &(Token){
		.str = "<NONE>"
	}
};
