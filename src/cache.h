/*
 * cache.h
 */

#ifndef EMULOS_CACHE_H_
#define EMULOS_CACHE_H_

#include "item.h"

typedef struct cache Cache;

struct cache {
	int est;
	Item *used;
	Item *item;
};

#endif
