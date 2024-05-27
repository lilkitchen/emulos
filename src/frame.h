/*
 * frame.h
 */

#ifndef EMULOS_FRAME_H_
#define EMULOS_FRAME_H_

#include "build/mind.h"

#include "branch.h"
#include "item.h"

typedef struct frame Frame;

struct frame {
	int bp;
	Branch *br;
};

#endif
