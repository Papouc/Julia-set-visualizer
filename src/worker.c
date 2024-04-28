#include "worker.h"
#include "errors.h"
#include "event_queue.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

void *work(void *arg)
{
  error *err_code = safe_malloc(sizeof(error));
  *err_code = NO_ERR;

  queue_init();

  while (true)
  {
    event current_event = queue_pop();

    if (current_event.type == EV_QUIT)
    {
      break;
    }
    else if (current_event.source == EV_MODULE)
    {
      printf("Work to be done :D\r\n");
      free(current_event.data.msg);
    }
  }

  queue_cleanup();

  pthread_exit((void *)err_code);
  return NULL;
}
