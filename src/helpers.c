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

void init_msg(message *new_msg)
{
  // properly initialize dynamically allocated message in order
  // to prevent valgrind memory errors
  new_msg->cksum = 0;
  new_msg->type = 0;
  memset(&(new_msg->data), 0, sizeof(new_msg->data));
}
