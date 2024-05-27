/*
 * brain.h
 */

#ifndef EMULOS_BRAIN_H_
#define EMULOS_BRAIN_H_

#include "build/mind.h"

#include "cache.h"
#include "item.h"
#include "label.h"
#include "message.h"
#include "frame.h"

enum {
	BRAIN_PARTNER_FEMALE = 0x01,
};

typedef struct brain Brain;

struct brain {
	uint32_t flags;

	int heardp;
	Message *heard[HISTORY_LIMIT];

	int saidp;
	Message *said[HISTORY_LIMIT];

	int lbp;
	LabelBlock lblocks[HISTORY_LIMIT];

	Items *items;
	Cache cache[SELECTORS_TOTAL];
};

void brain_init(Brain *brain);
void brain_quit(Brain *brain);

void brain_wash(Brain *brain);

int brain_read(Brain *brain, char *str);
int brain_react(Brain *brain, Action *action, char *reply);

int brain_poll(Brain *brain, int index);

#endif
