#include "errors.h"
#include "event_queue.h"

#include "keyboard.h"
#include "pipe.h"
#include "worker.h"
#include "xwin_sdl.h"
#include <pthread.h>
#include <stdlib.h>

#define THREAD_COUNT 4

// default pipes assumption
char *default_pipe_in = "/tmp/computational_module.out";
char *default_pipe_out = "/tmp/computational_module.in";

int main(int argc, char *argv[])
{
  error err_code = NO_ERR;

  // init pipe names
  // pipe in = data coming into the app from the module
  // pipe out = data coming to the module from the app
  char *pipe_in = (argc > 1) ? argv[1] : default_pipe_in;
  char *pipe_out = (argc > 2) ? argv[2] : default_pipe_out;

  // initialize app's main queue before starting any of the threads
  queue_init();

  // create threads (+ pass pipes/data)
  pthread_t threads[THREAD_COUNT];
  pthread_create(&threads[0], NULL, read_pipe, pipe_in);
  pthread_create(&threads[1], NULL, read_keyboard, NULL);
  pthread_create(&threads[2], NULL, work, pipe_out);
  pthread_create(&threads[3], NULL, xwin_poll_events, NULL);

  // wait for all threads
  for (int thread_i = 0; thread_i < THREAD_COUNT; thread_i++)
  {
    void *thread_ret = NULL;
    pthread_join(threads[thread_i], &thread_ret);

    if (thread_ret == NULL)
    {
      continue;
    }

    // preserve value of threads error code
    error thread_err_code = *((error *)thread_ret);
    if (thread_err_code != NO_ERR)
    {
      err_code = thread_err_code;
      handle_error(err_code);
      signal_quit();
    }

    free(thread_ret);
  }

  // deinitialize the app's main queue
  queue_cleanup();

  return err_code;
}
