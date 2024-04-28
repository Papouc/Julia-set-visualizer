/*
 * Filename: event_queue.h
 * Date:     2017/04/15 12:41
 * Author:   Jan Faigl
 */

#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__

#include "messages.h"
#include "pthread.h"

#define QUEUE_CAPACITY 100

typedef enum
{
  EV_NUCLEO,
  EV_KEYBOARD,
  EV_NUM
} event_source;

typedef enum
{
  EV_COMPUTE,     // request compute on nucleo with particular
  EV_RESET_CHUNK, // reset the chunk id
  EV_ABORT,
  EV_GET_VERSION,
  EV_THREAD_EXIT,
  EV_QUIT,
  EV_SERIAL,
  EV_SET_COMPUTE,
  EV_COMPUTE_CPU,
  EV_CLEAR_BUFFER,
  EV_REFRESH,
  EV_TYPE_NUM
} event_type;

typedef struct
{
  int param;
} event_keyboard;

typedef struct
{
  message *msg;
} event_serial;

typedef struct
{
  event_source source;
  event_type type;
  union
  {
    int param;
    message *msg;
  } data;
} event;

typedef struct
{
  int oldest;
  int size;
  int capacity;
  event *data;

  pthread_mutex_t queue_mtx;
  pthread_cond_t queue_cond;

} event_queue;

void queue_init(void);
void queue_cleanup(void);

event queue_pop(void);

void queue_push(event ev);

#endif

/* end of event_queue.h */
