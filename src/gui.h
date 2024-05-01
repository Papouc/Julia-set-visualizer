#ifndef __GUI_H__
#define __GUI_H__

#include "errors.h"
#include <stdint.h>

#define OPEN_WIN_SUCCESS 0

typedef struct
{
  int width;
  int height;
  uint8_t *image;
} gui_params;

error init_gui(void);
void tidy_gui(void);
error refresh_gui(void);

#endif
