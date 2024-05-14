#include "helpers.h"
#include "errors.h"
#include <stdio.h>
#include <string.h>

void *safe_malloc(size_t size)
{
  void *new_mem = malloc(size);
  if (new_mem == NULL)
  {
    handle_error(ALLOC_ERR);
    exit(ALLOC_ERR);
    return NULL;
  }

  return new_mem;
}

void init_arr(uint8_t *arr, int size)
{
  // set all values in the malloced array to zero
  // size = size in bytes
  memset(arr, 0, size);
}
