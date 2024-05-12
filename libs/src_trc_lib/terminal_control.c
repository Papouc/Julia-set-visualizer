#include "terminal_control.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

void switch_raw_mode(bool on)
{
  // static struct to preserve state between calls
  static struct termios tio, tioOld;
  tcgetattr(STDIN_FILENO, &tio);

  if (!on)
  {
    // remove non blocking flag from stdin
    int flags = fcntl(STD_IN_FD, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(STD_IN_FD, F_SETFL, flags);

    // set termios back to original state
    tcsetattr(STDIN_FILENO, TCSANOW, &tioOld);
  }
  else
  {
    // backup original state
    tioOld = tio;

    // set non blocking input
    fcntl(STD_IN_FD, F_SETFL, O_NONBLOCK);
    cfmakeraw(&tio);      // enable raw mode
    tio.c_oflag |= OPOST; // enable output post processing
    tcsetattr(STDIN_FILENO, TCSANOW, &tio);
  }
}
