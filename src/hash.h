/*
 * hash.h
 */

#ifndef EMULOS_HASH_H_
#define EMULOS_HASH_H_

#include <stdint.h>

typedef uint64_t Hash;

Hash hashsum(char *str);

#endif
