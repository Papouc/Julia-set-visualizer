#include <pthread.h>
#include <stdio.h>
#include "event_queue.h"

#define THREAD_COUNT 2

// default pipes assumption
char *default_pipe_in = "/tmp/computational_module.out";
char *default_pipe_out = "/tmp/computational_module.in";

int main(int argc, char *argv[])
{
  // init pipe names
  // pipe in = data coming into the module from the app
  // pipe out = data coming to the app from the module
  char *pipe_in = (argc > 1) ? argv[1] : default_pipe_in;
  char *pipe_out = (argc > 2) ? argv[2] : default_pipe_out;

  printf("%s", pipe_in);
  printf("%s", pipe_out);

  return 0;
}
