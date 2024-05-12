#ifndef __TERMINAL_CONTROL_H__
#define __TERMINAL_CONTROL_H__

#include <stdbool.h>

#define STD_IN_FD 0

/// @brief switches terminal to raw mode and enables non blocking input
/// @param on enable/disable raw+nonblock mode
void switch_raw_mode(bool on);

#endif
