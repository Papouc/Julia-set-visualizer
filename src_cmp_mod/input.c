#include "input.h"
#include "event_queue.h"
#include "terminal_control.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *read_keyboard(void *)
{
  switch_raw_mode(true);

  int current_c = getchar();

  while (current_c != EXIT_KEY && !should_quit())
  {
    current_c = getchar();

    // check for buffer changes periodically
    usleep(POLL_KEY_INTERVAL);
  }

  switch_raw_mode(false);

  // turn entire module off
  signal_quit();

  return NULL;
}
