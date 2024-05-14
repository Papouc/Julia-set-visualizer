#include "julia.h"
#include "complex_plain.h"
#include "processor.h"
#include <pthread.h>
#include <stdlib.h>

static julia_params params;
static pthread_mutex_t params_mut;

void init_julia_comp()
{
  pthread_mutex_init(&params_mut, NULL);
  params.computation_set = false;
}

void tidy_julia_comp()
{
  pthread_mutex_destroy(&params_mut);
  params.computation_set = false;
}

bool set_computation(msg_set_compute *msg)
{
  // in case of SET_COMPUTE message is recieved while
  // computation of a chunk is in progress
  pthread_mutex_lock(&params_mut);

  params.c_r = msg->c_re;
  params.c_i = msg->c_im;

  params.density_r = msg->d_re;
  params.density_i = msg->d_im;

  params.max_iters = msg->n;

  bool result = params.computation_set = true;

  pthread_mutex_unlock(&params_mut);

  return result;
}

#include <stdio.h>

void *process_chunk(void *msg)
{
  pthread_mutex_lock(&params_mut);
  bool is_set = params.computation_set;
  pthread_mutex_unlock(&params_mut);

  // check if computation was previously set
  if (!is_set)
  {
    return NULL;
  }

  // needed stuff from the processor thread (file descriptor, pipe mutex...)
  chunk_comp_args *args = (chunk_comp_args *)msg;

  pthread_mutex_lock(&params_mut);

  // CRITICAL SECTION: reading params
  // pack comp info to a struct needed by complex plain API
  complex_calc calc_info = {
    .c_r = params.c_r,
    .c_i = params.c_i,
    .n_iters = params.max_iters
  };

  pthread_mutex_unlock(&params_mut);

  // message recieved from control app
  msg_compute data = args->msg->data.compute;

  int height = data.n_im;
  int width = data.n_re;

  // buffer for outgoing messages
  unsigned char msg_bytes[sizeof(message)];

  // module quit handling
  bool force_quit = false;

  // loop through the entire chunk
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      force_quit = should_quit();
      if (force_quit)
      {
        break;
      }

      pthread_mutex_lock(&params_mut);

      // CRITICAL SECTION: reading params
      calc_info.n_r = data.re + (double)x * params.density_r;
      calc_info.n_i = data.im + (double)y * params.density_i;

      pthread_mutex_unlock(&params_mut);

      // desired number of iterations
      uint8_t iters = get_iters_at_pos(&calc_info);

      // build response message
      message comp_data_msg = { .type = MSG_COMPUTE_DATA };

      comp_data_msg.data.compute_data.cid = data.cid;
      comp_data_msg.data.compute_data.i_re = x;
      comp_data_msg.data.compute_data.i_im = y;
      comp_data_msg.data.compute_data.iter = iters;

      pthread_mutex_lock(args->pipe_mut);

      // CRITICAL SECTION: writing to pipe
      send_msg(&comp_data_msg, msg_bytes, args->pipe_fd);

      pthread_mutex_unlock(args->pipe_mut);
    }

    if (force_quit)
    {
      break;
    }
  }

  if (!force_quit)
  {
    // entire chunk computed, send done message
    message done_msg = { .type = MSG_DONE };

    pthread_mutex_lock(args->pipe_mut);

    // CRITICAL SECTION: writing to pipe
    send_msg(&done_msg, msg_bytes, args->pipe_fd);

    pthread_mutex_unlock(args->pipe_mut);
  }

  free(args->msg);
  free(args);

  return NULL;
}
