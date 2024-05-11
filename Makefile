CFLAGS+= -Wall -std=gnu99 -g
LDFLAGS=-pthread -lm -lcomplex_plain -L./bin

HW=prgsem
MAIN_BIN=bin/prgsem-main
CP_LIB=bin/libcomplex_plain.a

CFLAGS+=$(shell sdl2-config --cflags)
LDFLAGS+=$(shell sdl2-config --libs) -lSDL2_image

CFLAGS+=-I./src_cp_lib

# dummy target to make everything
all: $(MAIN_BIN) $(CP_LIB)

# ---- MAIN APP ----

SRC_FILES=$(wildcard src/*.c)
OBJS=$(patsubst %.c,%.o,$(SRC_FILES))
OBJS_BIN_DIR=$(patsubst %.o, bin/%.o, $(notdir $(OBJS)))

CURRENT_FILE=NONE

$(MAIN_BIN): $(OBJS_BIN_DIR) $(CP_LIB)
	$(CC) $(OBJS_BIN_DIR) $(LDFLAGS) -o $@

# pattern rule does not work here since I cannot manipulate prequisity value directly
$(OBJS_BIN_DIR): $(SRC_FILES) $(CP_LIB)
	$(eval CURRENT_FILE=$(patsubst %.o,%.c,src/$(notdir $@)))
	$(CC) -c $(CFLAGS) $(CURRENT_FILE) -o $@

# ---- END MAIN APP ----

# ---- STATIC COMPLEX_PLANE LIBRARY ----

LIB_FILE=src_cp_lib/complex_plain.c
LIB_OBJ=$(patsubst %.c,%.o,$(LIB_FILE))
LIB_OBJ_BIN_DIR=$(patsubst %.o,bin/%.o,$(notdir $(LIB_OBJ)))

$(CP_LIB): $(LIB_OBJ_BIN_DIR)
	ar -rcs $(CP_LIB) $(LIB_OBJ_BIN_DIR)

$(LIB_OBJ_BIN_DIR): $(LIB_FILE)
	$(CC) -c -Wall -g -lm $(LIB_FILE) -o $(LIB_OBJ_BIN_DIR)

# ---- END STATIC COMPLEX_PLANE LIBRARY ----

clean:
	rm -f $(MAIN_BIN) $(CP_LIB) $(OBJS_BIN_DIR) $(LIB_OBJ_BIN_DIR)
