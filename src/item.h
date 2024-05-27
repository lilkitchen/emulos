/*
 * item.h
 */

#ifndef EMULOS_ITEM_H_
#define EMULOS_ITEM_H_

#include "message.h"

typedef struct items Items;

typedef struct item Item;

struct item {
	Message *msg;
	WordID id;

	Item *next;
};

Items *items_create(void);
void items_destroy(Items *items);

void items_clear(Items *items);

Item *item_add(Items *items, Message *msg, WordID id);
Item *item_copy(Items *items, Item *item);

int item_is_match(Item *item, Message *msg);

Form item_form_get(Item *item);

int item_len(Item *item);
Item *item_choose(Item *item);

void item_print(Item *item);

extern Item item_dummy;

#endif
