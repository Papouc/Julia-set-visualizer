/*
 * Filename: prg_io_nonblock.c
 * Date:     2019/12/25 14:20
 * Author:   Jan Faigl
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include <poll.h>

#include "prg_io_nonblock.h"

/// ----------------------------------------------------------------------------
static int io_open(const char *fname, int flag)
{
  int fd = open(fname, flag | O_NOCTTY | O_SYNC);
  if (fd != -1)
  {
    // Set fd to non block mode
    int flags = fcntl(fd, F_GETFL);
    flags &= ~O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0)
    {
      fprintf(stderr, "Error in %s(), file %s, line: %d, errno %d\n", __func__, __FILE__, __LINE__, errno);
    }
  }
  return fd;
}

/// ----------------------------------------------------------------------------
int io_open_read(const char *fname)
{
  // as defined for FIFO, open will be blocked for read only unless nonblock is specified
  return io_open(fname, O_RDONLY | O_NONBLOCK);
}

/// ----------------------------------------------------------------------------
int io_open_write(const char *fname)
{
  // try to open pipe for writing in non blocking mode
  return io_open(fname, O_WRONLY | O_NONBLOCK);
}

/// ----------------------------------------------------------------------------
int io_close(int fd)
{
  return close(fd);
}

/// ----------------------------------------------------------------------------
int io_putc(int fd, char c)
{
  return write(fd, &c, 1);
}

/// ----------------------------------------------------------------------------
int io_write_buf(int fd, const void *buf, size_t size)
{
  return write(fd, buf, size);
}

/// ----------------------------------------------------------------------------
void disable_sigpipe(void)
{
  // disable SIGPIPE error in order to handle possible write errors
  // based on write functions's return value
  signal(SIGPIPE, SIG_IGN);
}

/// ----------------------------------------------------------------------------
int io_getc(int fd)
{
  char c;
  int r = read(fd, &c, 1);
  return r == 1 ? c : -1;
}

/// ----------------------------------------------------------------------------
int io_getc_timeout(int fd, int timeout_ms, unsigned char *c)
{
  struct pollfd ufdr[1];
  int r = 0;
  ufdr[0].fd = fd;
  ufdr[0].events = POLLIN | POLLRDNORM;
  if ((poll(&ufdr[0], 1, timeout_ms) > 0) && (ufdr[0].revents & (POLLIN | POLLRDNORM)))
  {
    r = read(fd, c, 1);
  }
  return r;
}

/* end of prg_io_nonblock.c */
