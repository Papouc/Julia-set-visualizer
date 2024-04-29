#include "helpers.h"
#include "errors.h"
#include <stdio.h>

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

void safe_realloc(void **mem, size_t size)
{
  void *temp = realloc(*mem, size);
  if (temp == NULL)
  {
    handle_error(ALLOC_ERR);
    exit(ALLOC_ERR);
  }

  *mem = temp;
}
