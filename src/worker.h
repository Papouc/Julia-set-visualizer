#ifndef __WORKER_H__
#define __WORKER_H__

#include "errors.h"
#include "event_queue.h"
#include "messages.h"

#define PIPE_NOT_OPENED -1

// main event handling functions
void *work(void *arg);
error process_keyboard_event(event k_event, unsigned char msg_bytes[], int pipe_fd);
error process_app_event(event a_event, unsigned char msg_bytes[], int pipe_fd);
error process_module_event(event m_event);

int open_pipe(char *pipe_name);
error send_msg(message *msg, unsigned char msg_bytes[], int pipe_fd);

// specific event handling functions
error handle_msg_version(message *msg);
error handle_startup_msg(message *msg);
error handle_compute_data(message *msg);
error handle_msg_done(message *msg);

void handle_send_failure(message_type msg_type);

#endif
