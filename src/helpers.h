#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "messages.h"
#include <stdlib.h>

void *safe_malloc(size_t size);
void safe_realloc(void **mem, size_t size);

void init_msg(message *new_msg);
void init_arr(uint8_t *arr, int size);

#endif
