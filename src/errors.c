#include "errors.h"
#include <stdio.h>
#include <stdlib.h>

void handle_error(error err_code)
{
  char *err_msg = NULL;

  switch (err_code)
  {
    case ALLOC_ERR:
      err_msg = "Memory allocation failure";
      break;
    case PIPE_ERR:
      err_msg = "Named pipe read/write failure";
      break;
    case MSG_SEND_ERR:
      err_msg = "Sending message to the comp module failure";
      break;
    case NO_ERR:
      break;
  }

  if (err_msg != NULL)
  {
    fprintf(stderr, "Error: %s :D!\r\n", err_msg);
  }
}
