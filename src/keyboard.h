#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <stdbool.h>

#define STD_IN_FD 0
#define POLL_KEY_INTERVAL 5 * 1000 // 5ms

typedef enum
{
  GET_VERSION_KEY = 'g',
  SET_PARAMS_KEY = 's',
  RUN_COMPUTE_KEY = '1',
  ABORT_COMPUTE_KEY = 'a',
  RESET_CID_KEY = 'r',
  CLEAR_BUFFER_KEY = 'l',
  REDRAW_WINDOW_KEY = 'p',
  COMPUTE_LOCALY_KEY = 'c',
  EXIT_KEY = 'q'
} keybindings;

void *read_keyboard(void *);
void switch_raw_mode(bool on);

#endif
