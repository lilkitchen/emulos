/*
 * emulos.h
 */

#ifndef EMULOS_EMULOS_H_
#define EMULOS_EMULOS_H_

#include <setjmp.h>

#define emulos_fail(c, msg) \
	do { \
		emulos_err = msg; \
		longjmp(emulos_jb, EMULOS_##c); \
	} while (0)

enum {
	EMULOS_OK,

	EMULOS_OUT_OF_MEMORY,
	EMULOS_ASSERT,
};

void emulos_init(void);

extern jmp_buf emulos_jb;
extern char *emulos_err;

#endif
