/*
 * item.c
 */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "emulos.h"
#include "rng.h"
#include "item.h"

#define BLOCK_SIZE 100

typedef struct block Block;

struct block {
	Item data[BLOCK_SIZE];
	Block *next, *prev;
};

struct items {
	int bp, p;
	Block root;
	Block *top;
};

Items *items_create(void)
{
	Items *items = malloc(sizeof(Items));
	if (items == NULL)
		emulos_fail(OUT_OF_MEMORY, "Items creation fail\n");
	memset(items, 0, sizeof(Items));

	return items;
}

void items_destroy(Items *items)
{
	Block *block = items->root.next;
	while (block) {
		Block *next = block->next;
		free(block);
		block = next;
	}
	free(items);
}

void items_clear(Items *items)
{
	items->bp = 0;
	items->p = 0;
}

Item *item_add(Items *items, Message *msg, WordID id)
{
	Block *block = &items->root;
	for (int i = 0; i < items->bp; i++)
		block = block->next;

	Item *item = block->data + items->p++;

	item->msg = msg;
	item->id = id;

	if (item->msg->role) {
		if (item->id == 0)
			item->id = token_random_id(msg->token);
	} else {
		item->msg = &message_dummy;
	}

	if (items->p == BLOCK_SIZE) {
		items->bp++;
		items->p = 0;
		if (block->next == NULL) {
			block->next = malloc(sizeof(Block));
			if (block->next == NULL)
				emulos_fail(OUT_OF_MEMORY, "Items block creation fail\n");
			memset(block->next, 0, sizeof(Block));
			block->next->prev = block;
		}
		items->top = block->next;
	}

	return item;
}

Item *item_copy(Items *items, Item *item)
{
	Item *copy = NULL;
	Item **tail = &copy;
	for (; item; item = item->next) {
		*tail = item_add(items, item->msg, item->id);
		tail = &(*tail)->next;
	}
	return copy;
}

int item_is_match(Item *item, Message *msg)
{
	for (; item; item = item->next)
		if (item->msg == msg)
			return 1;
	return 0;
}

Form item_form_get(Item *item)
{
	Word *word = words + item->id;

	return word_form_find(word, item->msg->token->str);
}

int item_len(Item *item)
{
	int len = 0;
	for (Item *i = item; i; i = i->next)
		len++;
	return len;
}

Item *item_choose(Item *item)
{
	int len = item_len(item);

	len = rng_int(len);
	for (; len; item = item->next)
		len--;

	return item;
}

void item_print(Item *item)
{
	if (item == NULL) {
		printf("(nil item)\n");
		return;
	}
	printf("Item: \n");
	for (; item; item = item->next) {
		printf("\t%s, ", item->msg->token->str);
		word_print_by_id(item->id);
		printf("\n");
	}
}

Item item_dummy = {
	.msg = &message_dummy
};
