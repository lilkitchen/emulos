# Makefile for Emulos

all:

APP := build/bin/emulos

ifdef DEBUG
	BUILDFLAGS = -g -Wall -O0 -DDEBUG
else
	BUILDFLAGS = -O2
endif

CFLAGS := -std=gnu17 $(BUILDFLAGS) -I. -Isrc -Ibuild

SRC_DIRS := \
	src \

SRC := $(wildcard $(addsuffix /*.c, $(SRC_DIRS)))
OBJ := $(addprefix build/obj/, $(SRC:.c=.o))

# -----------------------------------------------------------------------------
# Directories

OBJ_DIRS := $(addprefix build/obj/, $(SRC_DIRS))

DIRS := build build/bin build/obj $(OBJ_DIRS)

$(DIRS):
	@mkdir -p $@

# -----------------------------------------------------------------------------
# Dependencies

include $(wildcard $(addsuffix /*.d, $(OBJ_DIRS)))

# -----------------------------------------------------------------------------
# Data

SCRIPTS := $(wildcard $(addsuffix /*.py, scripts))
VOCAB := $(wildcard data/*.vocab)
MIND := $(wildcard data/*.mind)

build/vocab.c build/mind.c build/vocab.h build.mind.h: $(SCRIPTS) $(VOCAB) $(MIND)
	@python scripts/build.py $^

build/obj/vocab.o: build/vocab.c src/token.h src/word.h
	$(CC) $(CFLAGS) -c -Ithird-party/include $< -o $@

build/obj/mind.o: build/mind.c src/branch.h src/selector.h
	$(CC) $(CFLAGS) -c -Ithird-party/include $< -o $@

OBJ_GEN := build/obj/vocab.o build/obj/mind.o

build/obj/src/brain.o: $(OBJ_GEN)

OBJ += $(OBJ_GEN)

# -----------------------------------------------------------------------------
# Phonies

.PHONY: all run win clean

all: $(APP)

run: $(APP)
	@./$<

$(APP): $(OBJ)
	$(CC) $(CFLAGS) -o $(APP) $^ -lreadline -lncurses

WINLIBS := libncursesw.a libreadline.a libtermcap.a
WINLIBS := $(addprefix third-party/lib/, $(WINLIBS))

win: CC = x86_64-w64-mingw32-gcc
win: $(OBJ)
	$(CC) $(CFLAGS) -o build/bin/emulos.exe $^ $(WINLIBS)

clean:
	@rm -rf build

# -----------------------------------------------------------------------------
# Pattern rules

build/obj/%.o: %.c | $(DIRS)
	$(CC) $(CFLAGS) -c -MMD -Ithird-party/include $< -o $@
