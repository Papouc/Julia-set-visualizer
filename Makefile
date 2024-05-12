CFLAGS+= -Wall -std=gnu99 -g
LDFLAGS=-pthread -lm -lcomplex_plain -lprgio -lmsg -L./bin

CMPCFLAGS=-Wall -std=gnu99 -g
CMPLDFLAGS=-pthread -lm -lcomplex_plain -lprgio -lmsg -L./bin

HW=prgsem

# main binaries
MAIN_BIN=bin/prgsem-main
CMP_BIN=bin/prgsem-comp-module

# static libraries shared between binaries
CP_LIB=bin/libcomplex_plain.a
IO_LIB=bin/libprgio.a
MSG_LIB=bin/libmsg.a

# add header search paths
CFLAGS+=-I./libs/src_cp_lib
CFLAGS+=-I./libs/src_io_lib
CFLAGS+=-I./libs/src_msg_lib

CMPCFLAGS+=-I./libs/src_cp_lib
CMPCFLAGS+=-I./libs/src_io_lib
CMPCFLAGS+=-I./libs/src_msg_lib

# sdl configuration
CFLAGS+=$(shell sdl2-config --cflags)
LDFLAGS+=$(shell sdl2-config --libs) -lSDL2_image

# dummy target to make everything
all: $(MAIN_BIN) $(CMP_BIN) $(CP_LIB) $(IO_LIB) $(MSG_LIB)

# ---- MAIN APP ----

SRC_FILES=$(wildcard src/*.c)
OBJS=$(patsubst %.c,%.o,$(SRC_FILES))
OBJS_BIN_DIR=$(patsubst %.o,bin/%.o,$(notdir $(OBJS)))

CURRENT_FILE=NONE

$(MAIN_BIN): $(OBJS_BIN_DIR)
	$(CC) $(OBJS_BIN_DIR) $(LDFLAGS) -o $@

# pattern rule does not work here since I cannot manipulate prequisity value directly
$(OBJS_BIN_DIR): $(SRC_FILES) $(CP_LIB) $(IO_LIB) $(MSG_LIB)
	$(eval CURRENT_FILE=$(patsubst %.o,%.c,src/$(notdir $@)))
	$(CC) -c $(CFLAGS) $(CURRENT_FILE) -o $@

# ---- END MAIN APP ----

# ---- COMPUTE MODULE ----

SRC_CMP_FILES=$(wildcard src_cmp_mod/*.c)
CMP_OBJS=$(patsubst %.c,%.o,$(SRC_CMP_FILES))
CMP_OBJS_BIN_DIR=$(patsubst %.o,bin/%.o,$(notdir $(CMP_OBJS)))

$(CMP_BIN): $(CMP_OBJS_BIN_DIR)
	$(CC) $(CMP_OBJS_BIN_DIR) $(CMPLDFLAGS) -o $@

$(CMP_OBJS_BIN_DIR): $(SRC_CMP_FILES) $(CP_LIB) $(IO_LIB) $(MSG_LIB)
	$(eval CURRENT_FILE=$(patsubst %.o,%.c,src_cmp_mod/$(notdir $@)))
	$(CC) -c $(CMPCFLAGS) $(CURRENT_FILE) -o $@

# ---- END COMPUTE MODULE ----

# ---- STATIC COMPLEX_PLANE LIBRARY ----

LIB_FILE=libs/src_cp_lib/complex_plain.c
LIB_OBJ=$(patsubst %.c,%.o,$(LIB_FILE))
LIB_OBJ_BIN_DIR=$(patsubst %.o,bin/%.o,$(notdir $(LIB_OBJ)))

$(CP_LIB): $(LIB_OBJ_BIN_DIR)
	ar -rcs $(CP_LIB) $(LIB_OBJ_BIN_DIR)

$(LIB_OBJ_BIN_DIR): $(LIB_FILE)
	$(CC) -c -Wall -g -lm $(LIB_FILE) -o $(LIB_OBJ_BIN_DIR)

# ---- END STATIC COMPLEX_PLANE LIBRARY ----

# ---- STATIC IO LIBRARY ----

LIB_IO_FILE=libs/src_io_lib/prg_io_nonblock.c
LIB_IO_OBJ=$(patsubst %.c,%.o,$(LIB_IO_FILE))
LIB_IO_OBJ_BIN_DIR=$(patsubst %.o,bin/%.o,$(notdir $(LIB_IO_OBJ)))

$(IO_LIB): $(LIB_IO_OBJ_BIN_DIR)
	ar -rcs $(IO_LIB) $(LIB_IO_OBJ_BIN_DIR)

$(LIB_IO_OBJ_BIN_DIR): $(LIB_IO_FILE)
	$(CC) -c -Wall -g $(LIB_IO_FILE) -o $(LIB_IO_OBJ_BIN_DIR)


# ---- END STATIC IO LIBRARY ----

# ---- STATIC MESSAGING LIBRARY ----

LIB_MSG_FILES=libs/src_msg_lib/event_queue.c libs/src_msg_lib/messages.c
LIB_MSG_OBJ=$(patsubst %.c,%.o,$(LIB_MSG_FILES))
LIB_MSG_OBJ_BIN_DIR=$(patsubst %.o,bin/%.o,$(notdir $(LIB_MSG_OBJ)))

$(MSG_LIB): $(LIB_MSG_OBJ_BIN_DIR)
	ar -rcs $(MSG_LIB) $(LIB_MSG_OBJ_BIN_DIR)

$(LIB_MSG_OBJ_BIN_DIR): $(LIB_MSG_FILES)
	$(eval CURRENT_FILE=$(patsubst %.o,%.c,libs/src_msg_lib/$(notdir $@)))
	$(CC) -c -Wall -g -pthread $(CURRENT_FILE) -o $@

# ---- END STATIC MESSAGING LIBRARY ----

clean:
	rm -f $(MAIN_BIN) $(CMP_BIN)
	rm -f $(CP_LIB) $(IO_LIB) $(MSG_LIB)
	rm -f $(OBJS_BIN_DIR) $(LIB_OBJ_BIN_DIR) $(CMP_OBJS_BIN_DIR) $(LIB_IO_OBJ_BIN_DIR) $(LIB_MSG_OBJ_BIN_DIR)
