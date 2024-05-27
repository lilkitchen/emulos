/*
 * label.h
 */

#ifndef EMULOS_LABEL_H_
#define EMULOS_LABEL_H_

#include "build/mind.h"

#include "message.h"

typedef struct label Label;

struct label {
	uint16_t name;
	Message msg;
};

typedef struct label_block LabelBlock;

struct label_block {
	Label labels[ACTION_EFFECTS_LIMIT];
};

void label_block_destroy(LabelBlock block);

#endif
