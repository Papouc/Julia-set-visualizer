#include "event_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static event_queue main_queue;
static bool app_quit;

void queue_init(void)
{
  // init stuff related to queue
  main_queue.data = (event *)malloc(QUEUE_CAPACITY * sizeof(event));
  main_queue.capacity = QUEUE_CAPACITY;
  main_queue.size = 0;
  main_queue.oldest = 0;

  // failed to create queue
  if (main_queue.data == NULL)
  {
    app_quit = true;
    return;
  }

  // init stuff related to threading
  pthread_mutex_init(&main_queue.queue_mtx, NULL);
  pthread_cond_init(&main_queue.queue_cond, NULL);

  // set application to the running state
  app_quit = false;
}

void queue_cleanup(void)
{
  // pop all remaining events from the queue
  // and free evantual messages
  int size_now = main_queue.size;
  for (int i = 0; i < size_now; i++)
  {
    event current_event = queue_pop();

    event_type t_with_msg = (event_type)EV_MODULE;
    if (current_event.type == t_with_msg && current_event.data.msg != NULL)
    {
      free(current_event.data.msg);
    }
  }

  free(main_queue.data);
  pthread_mutex_destroy(&main_queue.queue_mtx);
  pthread_cond_destroy(&main_queue.queue_cond);
}

event queue_pop(void)
{
  pthread_mutex_lock(&main_queue.queue_mtx);

  while (main_queue.size <= 0)
  {
    // wait until there is something in the queue
    // awaits signal from push
    pthread_cond_wait(&main_queue.queue_cond, &main_queue.queue_mtx);
  }

  int orig_size = main_queue.size;

  event to_pop = main_queue.data[main_queue.oldest];

  main_queue.oldest = (main_queue.oldest + 1) % main_queue.capacity;
  main_queue.size--;

  if (orig_size >= main_queue.capacity)
  {
    // send signal when queue is not full anymore
    pthread_cond_signal(&main_queue.queue_cond);
  }

  pthread_mutex_unlock(&main_queue.queue_mtx);

  return to_pop;
}

void queue_push(event ev)
{
  pthread_mutex_lock(&main_queue.queue_mtx);

  while (main_queue.size >= main_queue.capacity)
  {
    // wait until there is free space in the queue
    // awaits signal from pop
    pthread_cond_wait(&main_queue.queue_cond, &main_queue.queue_mtx);
  }

  int orig_size = main_queue.size;

  int push_index = (main_queue.oldest + main_queue.size) % main_queue.capacity;
  main_queue.data[push_index] = ev;

  main_queue.size++;

  if (orig_size <= 0)
  {
    // send signal when the queue is not empty anymore
    pthread_cond_signal(&main_queue.queue_cond);
  }

  pthread_mutex_unlock(&main_queue.queue_mtx);
}

bool should_quit(void)
{
  // bool, being 1 byte size type should be atomic
  // thus no mutext used :D
  return app_quit;
}

void signal_quit(void)
{
  // bool, being 1 byte size type should be atomic
  // thus no mutext used :D
  app_quit = true;
}

void construct_msg(unsigned char current_c, unsigned char msg_bytes[], int *len, event_source src)
{
  // pos in bytes arr
  static int write_index;
  if (*len <= 0)
  {
    if (!get_message_size(current_c, len))
    {
      fprintf(stderr, "Unknown message recieved :D!");
      return;
    }

    // start of a new message
    write_index = 0;
    msg_bytes[write_index] = current_c;
    write_index++;
  }
  else if (write_index < *len)
  {
    // gradually append bytes to the message
    msg_bytes[write_index] = current_c;
    write_index++;

    // wait until all bytes of the message are read
    if (write_index < *len)
    {
      return;
    }

    // create real message from cached bytes
    message *created_msg = (message *)malloc(sizeof(message));

    if (created_msg == NULL)
    {
      return;
    }

    // initialize new message
    created_msg->cksum = 0;
    created_msg->type = 0;
    memset(&(created_msg->data), 0, sizeof(created_msg->data));

    bool result = parse_message_buf(msg_bytes, *len, created_msg);

    if (!result)
    {
      // failed to parse message
      free(created_msg);
    }
    else
    {
      // message parsed successfully, push new event to the queue
      event new_event = { .type = EV_SERIAL, .source = src };
      new_event.data.msg = created_msg;

      queue_push(new_event);
    }

    // prepare for next message
    *len = 0;
  }
}
