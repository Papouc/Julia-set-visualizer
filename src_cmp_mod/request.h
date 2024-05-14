#ifndef __REQUEST_H__
#define __REQUEST_H__

#define PIPE_OPEN_FAILURE -1
#define PIPE_READ_TIMEOUT 50
#define PIPE_EMPTY 0

void *read_requests(void *arg);

#endif
