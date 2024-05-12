#include "errors.h"
#include "event_queue.h"
#include "input.h"
#include "processor.h"
#include "request.h"
#include <pthread.h>
#include <stdlib.h>

#define THREAD_COUNT 3

// default pipes assumption
char *default_pipe_in = "/tmp/computational_module.in";
char *default_pipe_out = "/tmp/computational_module.out";

int main(int argc, char *argv[])
{
  error err_code = NO_ERR;

  // init pipe names
  // pipe in = data coming into the module from the app
  // pipe out = data coming to the app from the module
  char *pipe_in = (argc > 1) ? argv[1] : default_pipe_in;
  char *pipe_out = (argc > 2) ? argv[2] : default_pipe_out;

  // initialize module's main queue
  queue_init();

  // create required threads
  pthread_t threads[THREAD_COUNT];
  pthread_create(&threads[0], NULL, read_requests, pipe_in);
  pthread_create(&threads[1], NULL, read_keyboard, NULL);
  pthread_create(&threads[2], NULL, process_computation, pipe_out);

  for (int thread_i = 0; thread_i < THREAD_COUNT; thread_i++)
  {
    void *thread_ret = NULL;
    pthread_join(threads[thread_i], &thread_ret);

    if (thread_ret == NULL)
    {
      continue;
    }

    error thread_err_code = *((error *)thread_ret);
    if (thread_err_code != NO_ERR)
    {
      err_code = thread_err_code;
      handle_error(err_code);
      signal_quit();
    }

    free(thread_ret);
  }

  queue_cleanup();

  return err_code;
}
