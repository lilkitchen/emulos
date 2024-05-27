/*
 * emulos.c
 */

#include "rng.h"
#include "emulos.h"

jmp_buf emulos_jb;
char *emulos_err;

void emulos_init(void)
{
	rng_init();
}
