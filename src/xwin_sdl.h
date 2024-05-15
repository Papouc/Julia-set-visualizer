#ifndef __XWIN_SDL_H__
#define __XWIN_SDL_H__

#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>

#define POLL_INTERVAL 1 // ms
#define BUFF_SIZE 10
#define PATH_LEN_LIMIT 512
#define IMG_SAVE_SUCCESS 0

#define TIME_FORMAT_LET 9
#define FILENAME_LEN 14

int xwin_init(int w, int h);
void xwin_close(void);
void xwin_redraw(int w, int h, unsigned char *img);
void *xwin_poll_events(void *);
void xwin_save_img(void);

void handle_common_keys(SDL_Event sdl_ev);
void request_move_plane(int values_x[], int values_y[], bool mouse_pressed);
void request_zoom(int zoom_dir);


#endif
