#ifndef __JULIA_H__
#define __JULIA_H__

#include "messages.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
  double c_r;
  double c_i;

  double density_r;
  double density_i;

  uint8_t max_iters;

  bool computation_set;
} julia_params;

void init_julia_comp();
void tidy_julia_comp();

bool set_computation(msg_set_compute *msg);
void *process_chunk(void *msg);

#endif
