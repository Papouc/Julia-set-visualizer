#include "gui.h"
#include "computation.h"
#include "helpers.h"
#include "xwin_sdl.h"
#include <stdlib.h>

static gui_params params = {
  .image = NULL
};

error init_gui(void)
{
  // try to open graphical window
  // sized to hold the image

  get_grid_size(&params.width, &params.height);

  // + allocate space to hold the image
  int size = params.width * params.height * RGB_CNT * sizeof(uint8_t);
  params.image = safe_malloc(size);

  int result = xwin_init(params.width, params.height);

  return (result == OPEN_WIN_SUCCESS) ? NO_ERR : GUI_ERR;
}

void tidy_gui(void)
{
  free(params.image);
  params.image = NULL;

  xwin_close();
}

error refresh_gui(void)
{
  error err_code = NO_ERR;

  if (params.image != NULL)
  {
    // put color values into image based on calculated grid values
    err_code = colorize_image(params.width, params.height, params.image);

    if (err_code == NO_ERR)
    {
      // project changes to the graphical window
      xwin_redraw(params.width, params.height, params.image);
    }
  }

  return err_code;
}
