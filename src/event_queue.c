#include "event_queue.h"

void queue_init(void)
{
}

void queue_cleanup(void)
{
}

event queue_pop(void)
{
  event e = {EV_KEYBOARD, EV_COMPUTE};
  return e;
}

void queue_push(event ev)
{
}
