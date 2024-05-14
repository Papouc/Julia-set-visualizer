#ifndef __INPUT_H__
#define __INPUT_H__

#define POLL_KEY_INTERVAL 5 * 1000

typedef enum
{
  EXIT_KEY = 'q',
  ABORT_KEY = 'a'
} keybindings;

void *read_keyboard(void *);

#endif
