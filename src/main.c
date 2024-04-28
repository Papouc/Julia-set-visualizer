#include "errors.h"
#include "event_queue.h"
#include "prg_io_nonblock.h"

// threading
#include "worker.h"
#include "keyboard.h"
#include "pipe.h"
#include <pthread.h>

#define THREAD_COUNT 3

// default pipes assumption
const char *default_pipe_in = "/tmp/computational_module.out";
const char *default_pipe_out = "/tmp/computational_module.in";

int main(int argc, char *argv[])
{
  error err_code = NO_ERR;

  // init pipes (create file descriptors)
  const char *pipe_in = (argc > 1) ? argv[1] : default_pipe_in;
  const char *pipe_out = (argc > 2) ? argv[2] : default_pipe_out;

  int in_fd = io_open_read(pipe_in);
  int out_fd = io_open_write(pipe_out);

  // create threads (+ pass pipes/data)
  pthread_t threads[THREAD_COUNT];
  pthread_create(&threads[0], NULL, read_keyboard, NULL);
  pthread_create(&threads[1], NULL, read_pipe, &in_fd);
  pthread_create(&threads[2], NULL, work, &out_fd);

  queue_init();

  while (true)
  {
    event current_event = queue_pop();
  }

  queue_cleanup();

  handle_error(err_code);
  return err_code;
}
