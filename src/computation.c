#include "computation.h"
#include "helpers.h"
#include <stdlib.h>

// keep main computation properties
static compute_params params = {
  .c_r = -0.4,
  .c_i = 0.6,
  .n_iters = 60,

  .plane_start_r = -1.6,
  .plane_start_i = 1.1,
  .plane_end_r = 1.6,
  .plane_end_i = -1.1,

  .grid = NULL,
  .grid_w = 640,
  .grid_h = 480,

  .chunk_w = 64,
  .chunk_h = 48,

};

void init_computation(void)
{
  // allocate memory for the pixel grid
  int size = params.grid_h * params.grid_w * sizeof(uint8_t);
  params.grid = safe_malloc(size);

  // determine needed amount of chunks
  int grid_pixel_cnt = params.grid_w * params.grid_h;
  int chunk_pixel_cnt = params.chunk_w * params.chunk_h;
  params.n_chunks = grid_pixel_cnt / chunk_pixel_cnt;

  // setup density
  double plane_width = abs_val(params.plane_end_r - params.plane_start_r);
  double plane_height = abs_val(params.plane_end_i - params.plane_start_i);

  params.density_r = plane_width / ((double)params.grid_w);
  params.density_i = plane_height / ((double)params.grid_h);
}

void tidy_computation(void)
{
  free(params.grid);
  params.grid = NULL;
}

void stop_computation(void)
{
  params.in_progress = false;
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

    // succesfully filled the message
    success = true;
  }

  return success;
}

bool fill_compute_msg(message *msg)
{
  return true;
}
