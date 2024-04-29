#ifndef __ERRORS_H__
#define __ERRORS_H__

typedef enum
{
  NO_ERR = 0,
  ALLOC_ERR = 101,
  PIPE_ERR = 102,
  MSG_SEND_ERR = 103
} error;

void handle_error(error err_code);

#endif
