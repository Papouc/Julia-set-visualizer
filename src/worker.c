#include "worker.h"
#include "helpers.h"
#include "prg_io_nonblock.h"
#include <stdio.h>
#include <stdlib.h>

void *work(void *arg)
{
  error *err_code = safe_malloc(sizeof(error));
  *err_code = NO_ERR;

  // block execution until the pipe is opened for reading by comp module
  char *pipe_name = (char *)arg;
  int pipe_out_fd = io_open_write(pipe_name);

  // allocate bytes arr only once
  unsigned char msg_bytes[sizeof(message)];

  queue_init();

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

  queue_cleanup();

  pthread_exit((void *)err_code);
  return NULL;
}

error process_module_event(event m_event)
{
  error err_code = NO_ERR;
  printf("Work to be done :D\r\n");

  message *recieved_msg = m_event.data.msg;
  switch (recieved_msg->type)
  {
    case MSG_OK:
      fprintf(stdout, "Confirmed");
    case MSG_ERROR:
      fprintf(stderr, "Previous command error");
      break;
    case MSG_DONE:
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

  printf("Keyboard event :D\r\n");

  message new_msg = {.type = MSG_NBR};
  switch (k_event.type)
  {
    case EV_GET_VERSION:
      new_msg.type = MSG_GET_VERSION;
      break;
    case EV_SET_COMPUTE:
      new_msg.type = MSG_SET_COMPUTE;
      break;
    case EV_COMPUTE:
      new_msg.type = MSG_COMPUTE;
      break;
    case EV_ABORT:
      new_msg.type = MSG_ABORT;
      break;
    default:
      break;
  }

  // check if this type of event sends a message
  if (new_msg.type != MSG_NBR)
  {
    // send msg event

    int msg_len = 0;

    // create message content based on msg_type
    bool result = fill_message_buf(&new_msg, msg_bytes, sizeof(message), &msg_len);

    if (result)
    {
      // write the message to the pipe
      io_write_buf(pipe_fd, msg_bytes, msg_len);
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
  int major = msg->data.version.major;
  int minor = msg->data.version.minor;
  int patch = msg->data.version.patch;
  fprintf(stdout, "Version: %d.%d.%d\r\n", major, minor, patch);

  return NO_ERR;
}

error handle_startup_msg(message const *msg)
{
  char *startup_msg = (char *)msg->data.startup.message;
  fprintf(stdout, "Startup message: %s\r\n", startup_msg);

  return NO_ERR;
}

error handle_compute_data(message const *msg)
{
  // redirect processed data to computation compile unit
  return NO_ERR;
}
