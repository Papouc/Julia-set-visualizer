#ifndef __HELPERS_H__
#define __HELPERS_H__
#include <stdlib.h>

void *safe_malloc(size_t size);
void safe_realloc(void **mem, size_t size);

double abs_val(double num);

#endif
