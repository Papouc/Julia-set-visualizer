#ifndef __LOCAL_PROCESSING_H__
#define __LOCAL_PROCESSING_H__

#include "errors.h"
#include "event_queue.h"

#define PLANE_MOVE_SPEED 0.04
#define ZOOM_OFFSET 0.1

typedef struct
{
  // local mode state
  bool local_mode_enabled;

  // plain start coordinates (top left corner)
  double start_r;
  double start_i;

  // plain zoom
  double zoom_factor;
  double density_r;
  double density_i;

} local_mode_params;

// event distributor
error handle_local_keyboard_ev(event ev);

// seprate event handlers
error handle_reset_chunk(void);
error handle_clear_buffer(void);
error handle_compute_cpu(void);

void set_local_mode(bool on);
void move_plane(int x_diff, int y_diff);
void zoom_plane(int zoom_dir);

#endif
