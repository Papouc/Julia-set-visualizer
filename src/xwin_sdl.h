#ifndef __XWIN_SDL_H__
#define __XWIN_SDL_H__

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdbool.h>

#define POLL_INTERVAL 1 // ms
#define BUFF_SIZE 10
#define PATH_LEN_LIMIT 512
#define IMG_SAVE_SUCCESS 0

#define TIME_FORMAT_LET 9
#define FILENAME_LEN 14

// button params
#define BTN_CNT 2
#define BUT_W 140
#define BUT_H 50
#define BUT_MARGIN 10
#define BUT_COLOR 128, 128, 128, 160
#define TXT_COLOR 255, 255, 255

// sort of button "object"
typedef struct
{
  int pos_x;
  int pos_y;
  int width;
  int height;
  SDL_Color color;

  char *txt;
  SDL_Color txt_color;

  void (*on_click)(void);
} simple_button;

int xwin_init(int w, int h);
void xwin_close(void);
void xwin_redraw(int w, int h, unsigned char *img);
void *xwin_poll_events(void *);
void xwin_save_img(void);
void xwin_init_buttons(int w, int h);
void xwin_draw_buttons(void);

void handle_mouse_press(SDL_Event sdl_ev);
void handle_common_keys(SDL_Event sdl_ev);
void request_move_plane(int values_x[], int values_y[], bool mouse_pressed);
void request_zoom(int zoom_dir);
void request_anim(void);

#endif
