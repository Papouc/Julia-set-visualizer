#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include "errors.h"
#include "event_queue.h"

#define PIPE_NOT_OPENED -1
#define PIPE_CHECK_INTERVAL 10 * 1000 // 10ms
#define PTHREAD_SUCCESS 0

#define MAJOR_VER 1
#define MINOR_VER 0
#define PATCH_VER 0

#define STARTUP_STRING "BAF :D!"

typedef struct
{
  message *msg;
  pthread_mutex_t *pipe_mut;
  int pipe_fd;
} chunk_comp_args;

void signal_startup();
void *process_computation(void *arg);
int open_pipe(char *pipe_name);
error send_msg(message *msg, unsigned char msg_bytes[], int pipe_fd);

error process_keyboard_event(event k_event, unsigned char msg_bytes[], int pipe_fd);
error process_app_event(event a_event, unsigned char msg_bytes[], int pipe_fd);
error process_module_event(event m_event, unsigned char msg_bytes[], int pipe_fd);

bool start_chunk_computation(message *msg, int pipe_fd);
bool fill_version_message(message *msg);
bool fill_startup_message(message *msg);

#endif
