/*
 * message.h
 */

#ifndef EMULOS_MESSAGE_H_
#define EMULOS_MESSAGE_H_

#include "token.h"

/* keep in line with scripts/selector.py */

enum {
	MSG_EST		= 0x0001,

	MSG_Q		= 0x0002,
	MSG_EX		= 0x0004,
	MSG_PERIOD	= 0x0008,
	MSG_COMMA	= 0x0010,
	MSG_COLON	= 0x0020,
	MSG_SEMICOLON	= 0x0040,
	MSG_DASH	= 0x0080,
	MSG_DIGITS	= 0x0100,

	MSG_IN_Q	= 0x0200,
	MSG_IN_EX	= 0x0400,

	MSG_LAUGH	= 0x0800,
	MSG_SCREAM	= 0x1000,
};

typedef struct message Message;

struct message {
	int role;
	int len;
	Token *token;
};

Message *message_create(Token **dictum);
void message_destroy(Message *msg);

Message message_copy(Message *msg);

void message_print(Message *msg);

extern Message message_dummy;

#endif
