#ifndef __WORKER_H__
#define __WORKER_H__

#include "errors.h"
#include "event_queue.h"
#include "messages.h"

// main event handling functions
void *work(void *arg);
error process_keyboard_event(event k_event, unsigned char msg_bytes[], int pipe_fd);
error process_module_event(event m_event);

// specific event handling functions
error handle_msg_version(message const *msg);
error handle_startup_msg(message const *msg);
error handle_compute_data(message const *msg);

#endif
