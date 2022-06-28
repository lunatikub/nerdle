#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

#include "interface.h"

struct coord {
  int x;
  int y;
};

static void coord_set(struct coord *c, int x, int y)
{
  c->x = x;
  c->y = y;
}

struct color {
  int r;
  int g;
  int b;
};

static bool color_approx_eq(const struct color *c1, const struct color *c2)
{
#define LOCAL_APPROX 10
#define TOTAL_APPROX 20

  int diff_r = abs(c1->r - c2->r);
  int diff_g = abs(c1->g - c2->g);
  int diff_b = abs(c1->b - c2->b);

  if (diff_r > LOCAL_APPROX ||
      diff_g > LOCAL_APPROX ||
      diff_b > LOCAL_APPROX) {
    return false;
  }

  return (diff_r + diff_g + diff_b) < TOTAL_APPROX;

#undef LOCAL_APPROX
#undef TOTAL_APPROX
}

/**
 * Different colors of the cells from the site.
 */
static const struct color c_empty = { 231, 235, 241 };
static const struct color c_white = { 255, 255, 255 };
//static const struct color c_right = { 87, 172, 120 };
//static const struct color c_wrong = { 233, 198, 1 };
//static const struct color c_discarded = { 162, 162, 162 };

/**
 * Use x11 as the interface with the site.
 */
struct interface {
  /* X11 */
  Display *display;
  Window win;
  int screen;
  Colormap map;
  XImage *image;
  unsigned width;
  unsigned height;
  /* Properties */
  struct coord first; /* Left-up location of the grid. */
  uint32_t width_loc_sz;
  uint32_t width_space_sz;
  uint32_t height_loc_sz;
  uint32_t height_space_sz;
};

struct interface* interface_create(void)
{
  struct interface *in = calloc(1, sizeof(*in));

  in->display = XOpenDisplay(NULL);
  if (in->display == NULL) {
    return NULL;
  }

  in->screen = DefaultScreen(in->display);
  in->win = RootWindow(in->display, in->screen);
  in->map = DefaultColormap(in->display, in->screen);
  in->width = DisplayWidth(in->display, in->screen);
  in->height = DisplayHeight(in->display, in->screen);
  printf("[nerdle] bot started...\n");
  printf("[nerdle] screen size: %d x %d\n", in->width, in->height);

  return in;
}

static void image_refresh(struct interface *in)
{
  if (in->image != NULL) {
    XFree(in->image);
  }
  in->image = XGetImage(in->display, in->win, 0, 0,
                        in->width, in->height,
                        AllPlanes, XYPixmap);
}

static void set_mouse_coordinates(struct interface *in, struct coord *coord)
{
  int childx, childy;
  unsigned int mask;
  Window w1, w2;

  XQueryPointer(in->display, in->win, &w1, &w2,
        	&coord->x, &coord->y, &childx, &childy, &mask);
}

static void get_color_pixel(struct interface *in, int x, int y, struct color *color)
{
  XColor xcolor;

  xcolor.pixel = XGetPixel(in->image, x, y);
  XQueryColor(in->display, in->map, &xcolor);

  color->r = xcolor.red / 256;
  color->g = xcolor.green / 256;
  color->b = xcolor.blue / 256;
}

static bool find_h_inc(struct interface *in, struct coord *from,
                       struct coord *to, const struct color *color)
{
  struct color current;
  for (unsigned x = from->x; x < in->width; ++x) {
    get_color_pixel(in, x, from->y, &current);
    if (color_approx_eq(&current, color) == true) {
      coord_set(to, x, from->y);
      return true;
    }
  }
  return false;
}

static bool find_v_dec(struct interface *in, struct coord *from,
                       struct coord *to, const struct color *color)
{
  struct color current;
  for (int y = from->y; y >= 0; --y) {
    get_color_pixel(in, from->x, y, &current);
    if (color_approx_eq(&current, color) == true) {
      coord_set(to, from->x, y);
      return true;
    }
  }
  return false;
}

static bool find_v_inc(struct interface *in, struct coord *from,
                       struct coord *to, const struct color *color)
{
  struct color current;
  for (unsigned y = from->y; y < in->height; ++y) {
    get_color_pixel(in, from->x, y, &current);
    if (color_approx_eq(&current, color) == true) {
      coord_set(to, from->x, y);
      return true;
    }
  }
  return false;
}

static void set_start_coord(struct interface *in, struct coord *start)
{
  printf("[nerdle] set the focus (by clicking) on the tabulation of the browser...\n");
  printf("[nerdle] and be closest to midle of the first location of the first line...\n");
  printf("[nerdle]             ---   ---     \n");
  printf("[nerdle] here -> <- |   | |   |    \n");
  printf("[nerdle]             ---   ---     \n");
  printf("[nerdle]             ---   ---     \n");
  printf("[nerdle]            |   | |   |    \n");
  printf("[nerdle]             ---   ---  ...\n");
  printf("[nerdle]            .\n");
  printf("[nerdle]            .\n");
  printf("[nerdle]            .\n");
#define TIME_TO_WAIT 3
  for (uint32_t i = 0; i < TIME_TO_WAIT; ++i) {
    printf("[nerdle] %u/%u...\n", i + 1, TIME_TO_WAIT);
    sleep(1);
  }
#undef TIME_TO_WAIT
  set_mouse_coordinates(in, start);
  image_refresh(in);
}

static void set_horizontal_properties(struct interface *in, struct coord *start)
{
  struct coord from;
  struct coord to;

  /* width location size */
  from = *start;
  assert(find_h_inc(in, &from, &to, &c_empty));
  from = to;
  in->first.x = to.x;
  assert(find_h_inc(in, &from, &to, &c_white));
  in->width_loc_sz = to.x - from.x;

  /* horizontal space size between two locations */
  from = to;
  assert(find_h_inc(in, &from, &to, &c_white));
  from = to;
  assert(find_h_inc(in, &from, &to, &c_empty));
  in->width_space_sz = to.x - from.x;

  printf("[nerdle] width location size: %u\n", in->width_loc_sz);
  printf("[nerdle] first locaton x coordinate: %u\n", in->first.x);
  printf("[nerdle] horizontal space between two locations: %u\n", in->width_space_sz);
}

static void set_vertical_properties(struct interface *in, struct coord *start)
{
  struct coord from;
  struct coord to;

  /* height location size */
  from.x = in->first.x + in->width_loc_sz / 2;
  from.y = start->y;
  assert(find_v_dec(in, &from, &to, &c_white));
  in->first.y = to.y;
  from = to;
  ++from.y;
  assert(find_v_inc(in, &from, &to, &c_white));
  in->height_loc_sz = to.y - from.y;

  /* vertical space size between two locations */
  from = to;
  assert(find_v_inc(in, &from, &to, &c_empty));
  in->height_space_sz = to.y - from.y;

  printf("[handle] height location size: %u\n", in->height_loc_sz);
  printf("[handle] first location y coordinate: %u\n", in->first.y);
  printf("[handle] vertical space between two locations: %u\n", in->height_space_sz);
}

void interface_start(struct interface *in)
{
  struct coord start;

  set_start_coord(in, &start);
  set_horizontal_properties(in, &start);
  set_vertical_properties(in, &start);
}