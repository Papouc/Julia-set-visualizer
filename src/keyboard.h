#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#define POLL_KEY_INTERVAL 5 * 1000 // 5ms

#define CONST_UP 1
#define CONST_DOWN -1

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
  SAVE_IMG_KEY = 'i',
  ANIM_KEY = 'u',
  CONSTANT_UP_KEY = 'm',
  CONSTANT_DOWN_KEY = 'n',
  EXIT_KEY = 'q'
} keybindings;

void *read_keyboard(void *);

#endif
