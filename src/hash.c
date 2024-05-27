/*
 * hash.c
 */

#include "hash.h"

#define SEED 0

Hash hashsum(char *str)
{
	uint64_t hash = SEED;

	while (*str) {
		uint64_t c = (unsigned char)*str;
		hash = c + c * hash + (hash >> 3) + (hash << 15);
		str++;
	}

	return hash;
}
