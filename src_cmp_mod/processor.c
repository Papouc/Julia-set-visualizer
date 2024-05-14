#include "processor.h"
#include "helpers.h"
#include "input.h"
#include "julia.h"
#include "prg_io_nonblock.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static pthread_t chunk_thread;
static pthread_mutex_t pipe_mut;

void *process_computation(void *arg)
{
  error *err_code = safe_malloc(sizeof(error));
  *err_code = NO_ERR;

  // wait until the pipe is opened for reading by comp module
  char *pipe_name = (char *)arg;
  int pipe_out_fd = open_pipe(pipe_name);

  // set up computation core
  init_julia_comp();

  // init mutex meant for making sure that only one thread
  // is writing into the pipe at once
  pthread_mutex_init(&pipe_mut, NULL);

  // prepare outgoing message buffer
  unsigned char msg_bytes[sizeof(message)];

  // enqueue startup signal
  signal_startup();

  while (!should_quit())
  {
    event ev = queue_pop();

    if (ev.type == EV_QUIT)
    {
      break;
    }

    if (ev.source == EV_APP)
    {
      *err_code = process_app_event(ev, msg_bytes, pipe_out_fd);
    }
    else if (ev.source == EV_KEYBOARD)
    {
      *err_code = process_keyboard_event(ev, msg_bytes, pipe_out_fd);
    }
    else if (ev.source == EV_MODULE)
    {
      *err_code = process_module_event(ev, msg_bytes, pipe_out_fd);
    }

    if (*err_code != NO_ERR)
    {
      signal_quit();
    }
  }

  // perform cleanup
  tidy_julia_comp();
  pthread_mutex_destroy(&pipe_mut);

  pthread_exit((void *)err_code);
  return NULL;
}

error process_keyboard_event(event k_event, unsigned char msg_bytes[], int pipe_fd)
{
  error err_code = NO_ERR;

  message response = { .type = MSG_NBR };

  if (k_event.type == EV_ABORT)
  {
    response.type = MSG_ABORT;
  }

  if (response.type != MSG_NBR)
  {
    pthread_mutex_lock(&pipe_mut);
    err_code = send_msg(&response, msg_bytes, pipe_fd);
    pthread_mutex_unlock(&pipe_mut);
  }

  return err_code;
}

error process_app_event(event a_event, unsigned char msg_bytes[], int pipe_fd)
{
  error err_code = NO_ERR;

  message respond_msg = { .type = MSG_NBR };
  message *recieved_msg = a_event.data.msg;

  bool prep_success = true;

  switch (recieved_msg->type)
  {
    case MSG_ABORT:
      respond_msg.type = MSG_OK;
      fprintf(stdout, "Abort recieved :D\n");
      break;
    case MSG_COMPUTE:
      respond_msg.type = MSG_OK;
      prep_success = start_chunk_computation(recieved_msg, pipe_fd);
      break;
    case MSG_SET_COMPUTE:
      respond_msg.type = MSG_OK;
      prep_success = set_computation(&recieved_msg->data.set_compute);
      break;
    case MSG_GET_VERSION:
      respond_msg.type = MSG_VERSION;
      prep_success = fill_version_message(&respond_msg);
    default:
      break;
  }

  if (!prep_success)
  {
    fprintf(stderr, "Warning: failed to send message :D!");
  }
  else if (respond_msg.type != MSG_NBR)
  {
    pthread_mutex_lock(&pipe_mut);
    err_code = send_msg(&respond_msg, msg_bytes, pipe_fd);
    pthread_mutex_unlock(&pipe_mut);
  }

  if (a_event.data.msg != NULL && a_event.data.msg->type != MSG_COMPUTE)
  {
    // free allocated message (cases of MSG_COMPUTE are handled in separate thread)
    free(a_event.data.msg);
    a_event.data.msg = NULL;
  }

  return err_code;
}

error process_module_event(event m_event, unsigned char msg_bytes[], int pipe_fd)
{
  error err_code = NO_ERR;

  // process events that were appended to queue by comp module itself
  message response = { .type = MSG_NBR };

  if (m_event.type == EV_STARTUP)
  {
    response.type = MSG_STARTUP;
    fill_startup_message(&response);
  }

  if (response.type != MSG_NBR)
  {
    pthread_mutex_lock(&pipe_mut);
    err_code = send_msg(&response, msg_bytes, pipe_fd);
    pthread_mutex_unlock(&pipe_mut);
  }

  return err_code;
}

int open_pipe(char *pipe_name)
{
  fprintf(stdout, "Waiting for main app to connect (press %c to interrupt) :D!\n", (char)EXIT_KEY);

  // wait until the pipe is opened for reading by comp module
  // do not block thread, still needes to react to possible quit
  int pipe_out_fd = PIPE_NOT_OPENED;
  while (pipe_out_fd == PIPE_NOT_OPENED && !should_quit())
  {
    pipe_out_fd = io_open_write(pipe_name);

    // do not eat up 100% of CPU (no other cleaner way of doing this came to my mind :D)
    usleep(PIPE_CHECK_INTERVAL);
  }

  if (pipe_out_fd != PIPE_NOT_OPENED)
  {
    fprintf(stdout, "Main app connected :D!\n");
  }

  return pipe_out_fd;
}

error send_msg(message *msg, unsigned char msg_bytes[], int pipe_fd)
{
  // write message to the named pipe described by provided
  // file descriptor
  error err_code = NO_ERR;

  int msg_len = 0;

  // create message content based on msg_type
  bool result = fill_message_buf(msg, msg_bytes, sizeof(message), &msg_len);

  if (result)
  {
    // write the message to the pipe
    err_code = (io_write_buf(pipe_fd, msg_bytes, msg_len) >= 0) ? NO_ERR : MSG_SEND_ERR;
  }
  else
  {
    // failed to send message, signal error
    err_code = MSG_SEND_ERR;
  }

  return err_code;
}

bool start_chunk_computation(message *msg, int pipe_fd)
{
  chunk_comp_args *args = (chunk_comp_args *)safe_malloc(sizeof(chunk_comp_args));
  args->msg = msg;
  args->pipe_fd = pipe_fd;
  args->pipe_mut = &pipe_mut;

  int result = pthread_create(&chunk_thread, NULL, process_chunk, (void *)args);

  if (result == PTHREAD_SUCCESS)
  {
    // this thread is not meant to be jointed (lives it's own life)
    // resources should be released at the end of each chunk computation
    pthread_detach(chunk_thread);
  }

  return result == PTHREAD_SUCCESS;
}

bool fill_version_message(message *msg)
{
  msg->data.version.major = MAJOR_VER;
  msg->data.version.minor = MINOR_VER;
  msg->data.version.patch = PATCH_VER;

  return true;
}

bool fill_startup_message(message *msg)
{
  strcpy((char *)msg->data.startup.message, STARTUP_STRING);
  return true;
}

void signal_startup()
{
  event startup_signal = { .type = EV_STARTUP, .source = EV_MODULE };
  queue_push(startup_signal);
}
