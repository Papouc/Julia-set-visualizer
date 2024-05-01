#ifndef __COMPUTATION_H__
#define __COMPUTATION_H__

#include "errors.h"
#include "messages.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
  // constant c
  double c_r;
  double c_i;

  // iterations count
  int n_iters;

  // section of the imaginary plane
  double plane_start_r;
  double plane_start_i;

  double plane_end_r;
  double plane_end_i;

  // image grid
  int grid_w;
  int grid_h;
  uint8_t *grid;

  // complex plane to pixel grid transition consants
  // "how much do I have to move in complex plane to move by one pixel"
  double density_r;
  double density_i;

  // chunk info
  int chunk_r;  // start real axis
  int chunk_i;  // start complex axis
  int chunk_id; // chunk id
  int n_chunks; // number to complete entire image

  // chznk size (must fit into char type)
  uint8_t chunk_w;
  uint8_t chunk_h;

  // computation status
  bool in_progress;
  bool has_finished;

} compute_params;

// computation control
void init_computation(void);
void tidy_computation(void);
void stop_computation(void);

bool fill_set_compute_msg(message *msg);
bool fill_compute_msg(message *msg);

// computation state
bool is_in_progress(void);
bool has_finished(void);

#endif
