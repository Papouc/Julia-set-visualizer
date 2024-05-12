#include "request.h"
#include "errors.h"
#include "prg_io_nonblock.h"
#include <stdlib.h>

void *read_requests(void *arg)
{
  error *err_code = malloc(sizeof(error));

  char *pipe_name = (char *)arg;
  int pipe_in_fd = io_open_read(pipe_name);

  if (pipe_in_fd == PIPE_OPEN_FAILURE)
  {
    *err_code = PIPE_ERR;
  }
  else
  {
  }

  return NULL;
}
