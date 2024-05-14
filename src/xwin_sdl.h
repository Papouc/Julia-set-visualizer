#ifndef __XWIN_SDL_H__
#define __XWIN_SDL_H__

#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>

#define POLL_INTERVAL 1 // ms
#define BUFF_SIZE 10

int xwin_init(int w, int h);
void xwin_close(void);
void xwin_redraw(int w, int h, unsigned char *img);
void *xwin_poll_events(void *);

void request_move_plane(int values_x[], int values_y[], bool mouse_pressed);
void request_zoom(int zoom_dir);

#endif
