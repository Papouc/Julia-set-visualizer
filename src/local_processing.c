#include "local_processing.h"
#include "complex_plain.h"
#include "computation.h"
#include "gui.h"
#include "helpers.h"

error handle_local_keyboard_ev(event ev)
{
  error err_code = NO_ERR;

  // delegate incoming event to specific handlers
  switch (ev.type)
  {
    case EV_RESET_CHUNK:
      err_code = handle_reset_chunk();
      break;
    case EV_CLEAR_BUFFER:
      err_code = handle_clear_buffer();
      break;
    case EV_COMPUTE_CPU:
      err_code = handle_compute_cpu();
      break;
    default:
      break;
  }

  return err_code;
}

error handle_reset_chunk(void)
{
  reset_computation();
  return NO_ERR;
}

error handle_clear_buffer(void)
{
  erase_grid_contents();
  return NO_ERR;
}

error handle_compute_cpu(void)
{
  // gather necessary info about computation
  int width = 0, height = 0;
  get_grid_size(&width, &height);

  uint8_t max_iters = 0;
  get_iters_bound(&max_iters);

  double denisty_r = 0.0, denisty_i = 0.0;
  get_density(&denisty_r, &denisty_i);

  double const_r = 0.0, const_i = 0.0;
  get_constant(&const_r, &const_i);

  double start_r = 0.0, start_i = 0.0;
  get_plane_top_left(&start_r, &start_i);

  // erase everything that is currently on the screen
  handle_clear_buffer();

  // pack base info about the calculation
  complex_calc calc_info = {
    .n_iters = max_iters,
    .c_r = const_r,
    .c_i = const_i
  };

  // manually append value for every pixel into the grid
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      calc_info.n_r = start_r + (double)x * denisty_r;
      calc_info.n_i = start_i + (double)y * denisty_i;

      uint8_t iters = get_iters_at_pos(&calc_info);

      set_grid_value((y * width) + x, iters);
    }
  }

  refresh_gui();

  return NO_ERR;
}
