/*
 * label.c
 */

#include "label.h"

void label_block_destroy(LabelBlock block)
{
	for (int i = 0; i < ACTION_EFFECTS_LIMIT; i++) {
		if (block.labels[i].name == 0)
			break;

		token_destroy(block.labels[i].msg.token);
	}
}
