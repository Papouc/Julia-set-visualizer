#ifndef __LOCAL_PROCESSING_H__
#define __LOCAL_PROCESSING_H__

#include "errors.h"
#include "event_queue.h"

// event distributor
error handle_local_keyboard_ev(event ev);

// seprate event handlers
error handle_reset_chunk(void);
error handle_clear_buffer(void);
error handle_compute_cpu(void);

#endif
