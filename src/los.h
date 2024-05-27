/*
 * los.h
 */

#ifndef EMULOS_LOS_H_
#define EMULOS_LOS_H_

#include "action.h"
#include "brain.h"
#include "frame.h"

#define LOS_REPLY_LIMIT 1024

typedef struct los {
	char reply[LOS_REPLY_LIMIT];

	Brain brain;

	Frame frames[FRAMES_LIMIT];
	Action *options[OPTIONS_LIMIT];
} Los;

Los *los_create(void);
void los_destroy(Los *los);

int los_tell(Los *los, char *str);

#endif
