#include "worker.h"
#include "computation.h"
#include "helpers.h"
#include "keyboard.h"
#include "prg_io_nonblock.h"
#include <stdio.h>
#include <stdlib.h>

void *work(void *arg)
{
  error *err_code = safe_malloc(sizeof(error));
  *err_code = NO_ERR;

  // wait until the pipe is opened for reading by comp module
  char *pipe_name = (char *)arg;
  int pipe_out_fd = open_pipe(pipe_name);

  // allocate bytes arr only once
  unsigned char msg_bytes[sizeof(message)];

  disable_sigpipe();
  init_computation();

  while (!should_quit())
  {
    event current_event = queue_pop();

    if (current_event.type == EV_QUIT)
    {
      // application termination event
      break;
    }

    if (current_event.source == EV_MODULE)
    {
      // handle event, carrying message received from the module
      *err_code = process_module_event(current_event);
    }
    else if (current_event.source == EV_KEYBOARD)
    {
      // handle event coming from the keyboard
      // possibly send something to the module
      *err_code = process_keyboard_event(current_event, msg_bytes, pipe_out_fd);
    }

    if (*err_code != NO_ERR)
    {
      // application error, initialize termination
      signal_quit();
    }
  }

  pthread_exit((void *)err_code);
  return NULL;
}

error process_module_event(event m_event)
{
  error err_code = NO_ERR;

  message *recieved_msg = m_event.data.msg;
  switch (recieved_msg->type)
  {
    case MSG_OK:
      fprintf(stdout, "Confirmed :D!\n");
      break;
    case MSG_ERROR:
      fprintf(stderr, "Previous command error :D!\n");
      break;
    case MSG_DONE:
      fprintf(stdout, "Computation done :D!\n");
      break;
    case MSG_VERSION:
      handle_msg_version(recieved_msg);
      break;
    case MSG_STARTUP:
      handle_startup_msg(recieved_msg);
      break;
    case MSG_COMPUTE_DATA:
      handle_compute_data(recieved_msg);
      break;
    default:
      break;
  }

  free(m_event.data.msg);
  m_event.data.msg = NULL;

  return err_code;
}

error process_keyboard_event(event k_event, unsigned char msg_bytes[], int pipe_fd)
{
  error err_code = NO_ERR;

  message new_msg = { .type = MSG_NBR };
  bool prep_success = true;

  switch (k_event.type)
  {
    case EV_GET_VERSION:
      new_msg.type = MSG_GET_VERSION;
      break;
    case EV_SET_COMPUTE:
      new_msg.type = MSG_SET_COMPUTE;
      prep_success = fill_set_compute_msg(&new_msg);
      break;
    case EV_COMPUTE:
      new_msg.type = MSG_COMPUTE;
      prep_success = fill_compute_msg(&new_msg);
      break;
    case EV_ABORT:
      new_msg.type = MSG_ABORT;
      break;
    default:
      break;
  }

  // check if this type of event sends a message (and that the message is valid)
  if (!prep_success)
  {
    handle_send_failure(new_msg.type);
  }
  else if (new_msg.type != MSG_NBR)
  {
    // send msg event

    int msg_len = 0;

    // create message content based on msg_type
    bool result = fill_message_buf(&new_msg, msg_bytes, sizeof(message), &msg_len);

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
  }
  else
  {
    // local event
    // TODO: EV_RESET_CHUNK, EV_CLEAR_BUFFER, EV_REFRESH, EV_COMPUTE_CPU
  }

  return err_code;
}

error handle_msg_version(message const *msg)
{
  // extract version info from the message
  int major = msg->data.version.major;
  int minor = msg->data.version.minor;
  int patch = msg->data.version.patch;
  fprintf(stdout, "Version: %d.%d.%d\n", major, minor, patch);

  return NO_ERR;
}

error handle_startup_msg(message const *msg)
{
  // exrract string from the message
  char *startup_msg = (char *)msg->data.startup.message;
  fprintf(stdout, "Startup message: %s\n", startup_msg);

  return NO_ERR;
}

error handle_compute_data(message const *msg)
{
  // redirect processed data to computation compile unit
  return NO_ERR;
}

void handle_send_failure(message_type msg_type)
{
  char *warning_msg = NULL;
  switch (msg_type)
  {
    case MSG_SET_COMPUTE:
      warning_msg = "failed to send set compute message";
      break;
    case MSG_COMPUTE:
      warning_msg = "failed to send compute message";
      break;
    default:
      warning_msg = "failed to send message";
      break;
  }

  fprintf(stderr, "Warning: %s :D!\n", warning_msg);
}

int open_pipe(char *pipe_name)
{
  fprintf(stdout, "Waiting for comp module to connect (press %c to interrupt) :D!\n", (char)EXIT_KEY);

  // wait until the pipe is opened for reading by comp module
  // do not block thread, still needes to react to possible quit
  int pipe_out_fd = PIPE_NOT_OPENED;
  while (pipe_out_fd == PIPE_NOT_OPENED && !should_quit())
  {
    pipe_out_fd = io_open_write(pipe_name);
  }

  fprintf(stdout, "Comp module connected :D!\n");

  return pipe_out_fd;
}
