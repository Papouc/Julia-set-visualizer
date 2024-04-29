#include "keyboard.h"
#include "errors.h"
#include "event_queue.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

void *read_keyboard(void *)
{
  switch_raw_mode(true);

  int current_c = getchar();

  // map keypress to emitted event type
  while (current_c != EXIT_KEY)
  {
    event new_event = {.source = EV_KEYBOARD, .type = EV_TYPE_NUM};
    new_event.data.param = current_c;

    switch (current_c)
    {
      case GET_VERSION_KEY:
        new_event.type = EV_GET_VERSION;
        break;
      case SET_PARAMS_KEY:
        new_event.type = EV_SET_COMPUTE;
        break;
      case RUN_COMPUTE_KEY:
        new_event.type = EV_COMPUTE;
        break;
      case ABORT_COMPUTE_KEY:
        new_event.type = EV_ABORT;
        break;
      case RESET_CID_KEY:
        new_event.type = EV_RESET_CHUNK;
        break;
      case CLEAR_BUFFER_KEY:
        new_event.type = EV_CLEAR_BUFFER;
        break;
      case REDRAW_WINDOW_KEY:
        new_event.type = EV_REFRESH;
        break;
      case COMPUTE_LOCALY_KEY:
        new_event.type = EV_COMPUTE_CPU;
        break;
    }

    if (new_event.type != EV_TYPE_NUM)
    {
      queue_push(new_event);
    }

    current_c = getchar();
  }

  // revert terminal back to original state
  switch_raw_mode(false);

  // create application end event
  event exit_event = {.type = EV_QUIT, .source = EV_KEYBOARD};
  queue_push(exit_event);

  // entire application should end
  signal_quit();

  // "return" thread exit code
  error *err_code = safe_malloc(sizeof(error));
  *err_code = NO_ERR;
  pthread_exit((void *)err_code);

  return NULL;
}

void switch_raw_mode(bool on)
{
  // static struct to preserve state between calls
  static struct termios tio, tioOld;
  tcgetattr(STDIN_FILENO, &tio);

  if (!on)
  {
    tcsetattr(STDIN_FILENO, TCSANOW, &tioOld);
  }
  else
  {
    tioOld = tio;    // backup original state
    cfmakeraw(&tio); // enable raw mode
    tcsetattr(STDIN_FILENO, TCSANOW, &tio);
  }
}
