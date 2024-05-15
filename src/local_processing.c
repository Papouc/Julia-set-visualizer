#include "local_processing.h"
#include "complex_plain.h"
#include "computation.h"
#include "gui.h"
#include "helpers.h"
#include "xwin_sdl.h"
#include <math.h>
#include <unistd.h>

static local_mode_params params = {
  .local_mode_enabled = false,
};

error handle_local_keyboard_ev(event ev)
{
  error err_code = NO_ERR;

  bool redraw = false;

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
      set_local_mode(true);
      redraw = true;
      break;
    case EV_MOVE_PLANE:
      msg_move_plane data = ev.data.msg->data.move_plane;
      move_plane(data.x_diff, data.y_diff);
      redraw = true;
      break;
    case EV_ZOOM:
      zoom_plane(ev.data.param);
      redraw = true;
      break;
    case EV_SAVE_IMG:
      xwin_save_img();
      break;
    case EV_PLAY_ANIM:
      set_local_mode(true);
      play_animation();
      break;
    case EV_CHANGE_CONST:
      if (!params.local_mode_enabled)
      {
        set_local_mode(true);
      }
      params.c_r += ev.data.param * C_MANUAL_DIFF;
      params.c_i += ev.data.param * C_MANUAL_DIFF;
      redraw = true;
      break;
    default:
      break;
  }

  if (redraw)
  {
    err_code = handle_compute_cpu();
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
  refresh_gui();
  return NO_ERR;
}

error handle_compute_cpu(void)
{
  // only redraw in local mode
  if (!params.local_mode_enabled)
  {
    return NO_ERR;
  }

  // gather necessary info about computation
  int width = 0, height = 0;
  get_grid_size(&width, &height);

  uint8_t max_iters = 0;
  get_iters_bound(&max_iters);

  // erase everything that is currently on the screen
  // + cancle any ongoing computation
  cancel_computation();
  erase_grid_contents();

  // pack base info about the calculation
  complex_calc calc_info = {
    .n_iters = max_iters,
    .c_r = params.c_r,
    .c_i = params.c_i
  };

  // manually append value for every pixel into the grid
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      calc_info.n_r = params.start_r + (double)x * params.density_r;
      calc_info.n_i = params.start_i + (double)y * params.density_i;

      uint8_t iters = get_iters_at_pos(&calc_info);

      set_grid_value((y * width) + x, iters);
    }
  }

  refresh_gui();

  return NO_ERR;
}

void set_local_mode(bool on)
{
  params.local_mode_enabled = on;

  if (on)
  {
    // reset plane start to base pos
    double start_r = 0.0, start_i = 0.0;
    get_plane_top_left(&start_r, &start_i);

    // default plane start
    params.start_r = start_r;
    params.start_i = start_i;

    double density_r = 0.0, density_i = 0.0;
    get_density(&density_r, &density_i);

    // default density
    params.density_r = density_r;
    params.density_i = density_i;

    // default constant
    double const_r = 0.0, const_i = 0.0;
    get_constant(&const_r, &const_i);

    params.c_r = const_r;
    params.c_i = const_i;

    // no zoom by default
    params.zoom_factor = 1.0;
  }
}

void move_plane(int x_diff, int y_diff)
{
  if (params.local_mode_enabled)
  {
    params.start_r -= (double)x_diff * PLANE_MOVE_SPEED * params.zoom_factor;
    params.start_i += (double)y_diff * PLANE_MOVE_SPEED * params.zoom_factor;
  }
}

void zoom_plane(int zoom_dir)
{
  if (params.local_mode_enabled)
  {
    // adjust zoom
    params.zoom_factor *= (zoom_dir > 0) ? 1.0 - ZOOM_OFFSET : 1.0 + ZOOM_OFFSET;

    // get plain bounds
    double start_r = 0.0, start_i = 0.0;
    get_plane_top_left(&start_r, &start_i);

    double end_r = 0.0, end_i = 0.0;
    get_plane_bottom_right(&end_r, &end_i);

    int grid_w = 0, grid_h = 0;
    get_grid_size(&grid_w, &grid_h);

    // apply zoom to the plane
    start_r *= params.zoom_factor;
    start_i *= params.zoom_factor;
    end_r *= params.zoom_factor;
    end_i *= params.zoom_factor;

    // update plane value
    double plane_width = fabs(end_r - start_r);
    double plane_height = fabs(start_i - end_i);

    params.density_r = (plane_width / (double)grid_w);
    params.density_i = -(plane_height / (double)grid_h);
  }
}

void play_animation(void)
{
  if (!params.local_mode_enabled)
  {
    return;
  }

  for (int frame_i = 0; frame_i < FRAMES_COUNT; frame_i++)
  {
    params.c_i += C_FRAME_DIFF;
    params.c_r += C_FRAME_DIFF;

    handle_compute_cpu();

    usleep(FRAME_TIME);
  }
}
