/*
 * rng.c
 */

#include <stdint.h>
#include <time.h>

#include "rng.h"

static uint64_t seed = 12345678;

void rng_init(void)
{
#ifdef DEBUG
	seed = 0;
#else
	seed = time(NULL);
#endif
}

int rng_int(int limit)
{
	seed = seed * 1777771 + 777977;
	seed = seed >> 3;
	return seed % limit;
}
