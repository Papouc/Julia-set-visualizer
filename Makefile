CFLAGS+= -Wall -Werror -std=gnu99 -g
LDFLAGS=-pthread

HW=prgsem
MAIN_BIN=bin/prgsem-main

CFLAGS+=$(shell sdl2-config --cflags)
LDFLAGS+=$(shell sdl2-config --libs) -lSDL2_image

fun=$(notdir $1)

all: $(MAIN_BIN)

# all source files in src folder
SRC_FILES=$(wildcard src/*.c)
$(info src files is $(SRC_FILES))

# change extensions to .o
OBJS=$(patsubst %.c,%.o,$(SRC_FILES))

# change directory to bin
OBJS_BIN_DIR=$(patsubst %.o, bin/%.o, $(notdir $(OBJS)))

CURRENT_FILE=NIC

$(MAIN_BIN): $(OBJS_BIN_DIR)
	$(CC) $(OBJS_BIN_DIR) $(LDFLAGS) -o $@

# pattern rule does not work here since I cannot manipulate prequisity value directly
$(OBJS_BIN_DIR): $(SRC_FILES)
	$(eval CURRENT_FILE=$(patsubst %.o,%.c,src/$(notdir $@)))
	$(CC) -c $(CFLAGS) $(CURRENT_FILE) -o $@

clean:
	rm -f $(MAIN_BIN) $(OBJS_BIN_DIR)
