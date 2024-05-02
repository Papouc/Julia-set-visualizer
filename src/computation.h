#ifndef __COMPUTATION_H__
#define __COMPUTATION_H__

#include "errors.h"
#include "messages.h"
#include <stdbool.h>
#include <stdint.h>

#define R_OFFSET 0
#define G_OFFSET 1
#define B_OFFSET 2
#define RGB_CNT 3

typedef struct
{
  // constant c
  double c_r;
  double c_i;

  // iterations count
  int n_iters;

  // section of the imaginary plane
  double plane_max_r;
  double plane_max_i;

  double plane_min_r;
  double plane_min_i;

  // image grid
  int grid_w;
  int grid_h;
  uint8_t *grid;

  // complex plane to pixel grid transition consants
  // "how much do I have to move in complex plane to move by one pixel"
  double density_r;
  double density_i;

  // where to draw current chunk (in the grid)
  int draw_x;
  int draw_y;

  // chunk info
  double chunk_start_r; // start real axis
  double chunk_start_i; // start complex axis
  int chunk_id;         // chunk id
  int n_chunks;         // number to complete entire image

  // chznk size (must fit into char type)
  uint8_t chunk_w;
  uint8_t chunk_h;

  // computation status
  bool in_progress;
  bool has_finished;
  bool aborted;

} compute_params;

// computation control
void init_computation(void);
void tidy_computation(void);
void abort_computation(void);
void unabort_computation(void);

bool fill_set_compute_msg(message *msg);
bool fill_compute_msg(message *msg);

error update_grid(msg_compute_data *data);
error colorize_image(int width, int height, uint8_t *image);

// computation state (getters...)
bool is_in_progress(void);
bool has_finished(void);
bool is_aborted(void);
void get_grid_size(int *width, int *height);

#endif
