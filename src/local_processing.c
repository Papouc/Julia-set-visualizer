#include "local_processing.h"
#include "computation.h"

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
