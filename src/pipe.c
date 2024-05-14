#include "pipe.h"
#include "errors.h"
#include "event_queue.h"
#include "helpers.h"
#include "prg_io_nonblock.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *read_pipe(void *arg)
{
  error *err_code = safe_malloc(sizeof(error));
  *err_code = NO_ERR;

  char *pipe_name = (char *)arg;
  int pipe_in_fd = io_open_read(pipe_name);

  if (pipe_in_fd == PIPE_OPEN_FAILURE)
  {
    // failed to open the pipe
    *err_code = PIPE_ERR;
  }
  else
  {
    unsigned char current_c;

    // empty the pipe before reading any real data
    while (io_getc_timeout(pipe_in_fd, PIPE_READ_TIMEOUT, &current_c) != PIPE_EMPTY) {}

    // build up message byte after byte
    unsigned char msg_bytes[sizeof(message)] = { [0] = 0 };
    int len = 0;

    while (!should_quit())
    {
      // do not block the thread in order to allow for quick exit if needed
      int result = io_getc_timeout(pipe_in_fd, PIPE_READ_TIMEOUT, &current_c);

      if (result > 0)
      {
        // something was read
        construct_msg(current_c, msg_bytes, &len, EV_MODULE);
      }
      else if (result == 0)
      {
        // timed out, nothing is currently available in the pipe
        continue;
      }
      else
      {
        // error during reading from the pipe
        *err_code = PIPE_ERR;
        break;
      }
    }

    io_close(pipe_in_fd);
  }

  pthread_exit((void *)err_code);
  return NULL;
}
