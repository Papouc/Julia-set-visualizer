#include "computation.h"
#include "helpers.h"
#include "math.h"
#include <stdlib.h>

// keep main computation properties
static compute_params params = {
  .c_r = -0.4,
  .c_i = 0.6,
  .n_iters = 60,

  .plane_max_r = 1.6,
  .plane_max_i = 1.1,
  .plane_min_r = -1.6,
  .plane_min_i = -1.1,

  .grid = NULL,
  .grid_w = 640,
  .grid_h = 480,

  .chunk_w = 64,
  .chunk_h = 48,

  .in_progress = false,
  .has_finished = true,
  .aborted = false,

  .erase_scheluded = false,
};

void init_computation(void)
{
  // allocate memory for the pixel grid
  int size = params.grid_h * params.grid_w * sizeof(uint8_t);
  params.grid = safe_malloc(size);
  init_arr(params.grid, size);

  // determine needed amount of chunks
  int grid_pixel_cnt = params.grid_w * params.grid_h;
  int chunk_pixel_cnt = params.chunk_w * params.chunk_h;
  params.n_chunks = grid_pixel_cnt / chunk_pixel_cnt;

  // setup density
  double plane_width = fabs(params.plane_max_r - params.plane_min_r);
  double plane_height = fabs(params.plane_max_i - params.plane_min_i);

  params.density_r = (plane_width / (double)params.grid_w);
  params.density_i = -(plane_height / (double)params.grid_h);
}

void tidy_computation(void)
{
  free(params.grid);
  params.grid = NULL;
}

void abort_computation(void)
{
  if (!is_aborted())
  {
    // move one chunk back
    params.chunk_id--;

    if (params.draw_x - params.chunk_w < 0)
    {
      // return to the end of the previous line
      params.draw_x = params.grid_w - params.chunk_w;
      params.draw_y -= params.chunk_h;

      params.chunk_start_i -= params.density_i * params.chunk_h;
      params.chunk_start_r = params.plane_max_r - (params.density_r * params.chunk_w);
    }
    else
    {
      // move to the previous chunk on the current line
      params.draw_x -= params.chunk_w;
      params.chunk_start_r -= params.density_r * params.chunk_w;
    }
  }

  // "freeze" computation in current state
  params.aborted = true;
}

void unabort_computation(void)
{
  // "unfreeze" computation in current state
  params.aborted = false;
}

bool is_aborted(void)
{
  // computation status getter
  return params.aborted;
}

bool is_in_progress(void)
{
  // computation status getter
  return params.in_progress;
}

bool has_finished(void)
{
  // computation status getter
  return params.has_finished;
}

bool fill_set_compute_msg(message *msg)
{
  bool success = false;

  if (!is_in_progress())
  {
    // c constant
    msg->data.set_compute.c_re = params.c_r;
    msg->data.set_compute.c_im = params.c_i;

    // denisty
    msg->data.set_compute.d_re = params.density_r;
    msg->data.set_compute.d_im = params.density_i;

    // number of iterations
    msg->data.set_compute.n = params.n_iters;

    // starting new computation
    params.has_finished = false;
    params.aborted = false;

    // succesfully filled the message
    success = true;
  }

  return success;
}

bool fill_compute_msg(message *msg)
{
  // "start computing!!" message construction
  if (!is_in_progress() && !has_finished())
  {
    // first chunk init
    params.chunk_id = 0;
    params.chunk_start_r = params.plane_min_r;
    params.chunk_start_i = params.plane_max_i;

    // draw from the left top corner
    params.draw_x = params.draw_y = 0;

    params.in_progress = true;
  }
  else if (!has_finished())
  {
    // next chunk init
    params.chunk_id++;

    // move drawing cursor one chunk to the right
    params.draw_x += params.chunk_w;
    params.chunk_start_r += params.chunk_w * params.density_r;

    if (params.draw_x >= params.grid_w)
    {
      // move cursor to the left side, one line down
      params.draw_x = 0;
      params.draw_y += params.chunk_h;

      // move the chunk in complex plane
      params.chunk_start_r = params.plane_min_r;
      params.chunk_start_i += params.chunk_h * params.density_i;
    }
  }

  if (is_in_progress() && !has_finished())
  {
    // fill message (for any state of computation)
    msg->data.compute.cid = params.chunk_id;

    msg->data.compute.re = params.chunk_start_r;
    msg->data.compute.im = params.chunk_start_i;

    msg->data.compute.n_re = params.chunk_w;
    msg->data.compute.n_im = params.chunk_h;
  }

  return is_in_progress();
}

error update_grid(msg_compute_data *data)
{
  error err_code = (data->cid == params.chunk_id) ? NO_ERR : INCORRECT_CHUNK_ERR;

  if (data->cid == 0 && params.erase_scheluded)
  {
    // make sure to draw onto blank plane after erase is scheluded
    erase_grid_contents();
    params.erase_scheluded = false;
  }

  if (err_code == NO_ERR)
  {
    // extract the pos within the chunk
    int x_in_chunk = data->i_re;
    int y_in_chunk = data->i_im;

    // jump to the current line + index in that line
    int skipped_lines_n = (params.draw_y + y_in_chunk) * params.grid_w;
    int write_index = skipped_lines_n + (params.draw_x + x_in_chunk);

    int max_boundary = params.grid_w * params.grid_h;
    if (write_index >= 0 && write_index < max_boundary)
    {
      params.grid[write_index] = data->iter;
    }

    // image completely drawn
    if (params.chunk_id + 1 >= params.n_chunks)
    {
      params.has_finished = true;
      params.in_progress = false;
      params.aborted = false;
    }
  }

  return err_code;
}

error colorize_image(int width, int height, uint8_t *image)
{
  // check if desired image matches grid params
  if (params.grid_w != width || params.grid_h != height)
  {
    return GUI_ERR;
  }

  int pixel_cnt = width * height;
  for (int pixel_i = 0; pixel_i < pixel_cnt; pixel_i++)
  {
    // t = k/n
    double t = (double)params.grid[pixel_i] / (double)params.n_iters;

    // t_complement = 1.0 - t
    double t_co = 1.0 - t;

    // put RGB values to the image
    *(image + R_OFFSET) = (uint8_t)(9.0 * pow(t_co, 1.0) * pow(t, 3.0) * 255.0);
    *(image + G_OFFSET) = (uint8_t)(15.0 * pow(t_co, 2.0) * pow(t, 2.0) * 255);
    *(image + B_OFFSET) = (uint8_t)(8.5 * pow(t_co, 3.0) * pow(t, 1.0) * 255);

    image += RGB_CNT;
  }

  return NO_ERR;
}

void erase_grid_contents()
{
  // dump all current values in the computation grid
  // re-initialize it to all zeros
  int size = params.grid_h * params.grid_w * sizeof(uint8_t);
  init_arr(params.grid, size);
}

void reset_computation()
{
  // prepare state in order to recall set compute
  params.has_finished = false;
  params.in_progress = false;

  // schelude erase (buffer will be erased during next grid update)
  params.erase_scheluded = true;
}

// ---- GETTERS ----
void set_grid_value(int index, uint8_t value)
{
  params.grid[index] = value;
}

void get_grid_size(int *width, int *height)
{
  *width = params.grid_w;
  *height = params.grid_h;
}

void get_density(double *r_part, double *i_part)
{
  *r_part = params.density_r;
  *i_part = params.density_i;
}

void get_constant(double *r_part, double *i_part)
{
  *r_part = params.c_r;
  *i_part = params.c_i;
}

void get_plane_top_left(double *r_part, double *i_part)
{
  *r_part = params.plane_min_r;
  *i_part = params.plane_max_i;
}

void get_iters_bound(uint8_t *bound)
{
  *bound = params.n_iters;
}

// ---- END GETTERS ----
