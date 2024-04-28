#ifndef __PIPE_H__
#define __PIPE_H__

#define PIPE_OPEN_FAILURE -1
#define PIPE_READ_TIMEOUT 50
#define PIPE_EMPTY 0

void *read_pipe(void *arg);
void construct_msg(unsigned char current_c, unsigned char msg_bytes[], int *len);

#endif
