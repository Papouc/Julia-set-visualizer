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
    event new_event = { .source = EV_KEYBOARD, .type = EV_TYPE_NUM };
    new_event.data.msg = NULL;

    if (current_c == ABORT_KEY)
    {
      new_event.type = EV_ABORT;
      
    }

    if (new_event.type != EV_TYPE_NUM)
    {
      queue_push(new_event);
    }

    current_c = getchar();

    // check for buffer changes periodically
    usleep(POLL_KEY_INTERVAL);
  }

  switch_raw_mode(false);

  event exit_event = { .type = EV_QUIT, .source = EV_KEYBOARD };
  queue_push(exit_event);

  // turn entire module off
  signal_quit();

  return NULL;
}
