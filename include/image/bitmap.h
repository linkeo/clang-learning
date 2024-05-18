#ifndef __IMAGE_BITMAP_H
#define __IMAGE_BITMAP_H

#include "struct/bool.h"
#include <stddef.h>
#include <stdio.h>

#define BYTES_PER_PIXEL 3 // red + green + blue
#define FILE_HEADER_SIZE 14
#define INFO_HEADER_SIZE 40

typedef unsigned char bitmap_byte;

typedef struct __bitmap_color {
  bitmap_byte red;
  bitmap_byte green;
  bitmap_byte blue;
} bitmap_color;

#define bitmap_rgb(red, green, blue) ((bitmap_color){red, green, blue})
#define BITMAP_WHITE bitmap_rgb(255, 255, 255)
#define BITMAP_BLACK bitmap_rgb(0, 0, 0)
#define BITMAP_RED bitmap_rgb(255, 0, 0)
#define BITMAP_GREEN bitmap_rgb(0, 255, 0)
#define BITMAP_BLUE bitmap_rgb(0, 0, 255)
#define BITMAP_CYAN bitmap_rgb(0, 255, 255)
#define BITMAP_YELLOW bitmap_rgb(255, 255, 0)
#define BITMAP_MAGENTA bitmap_rgb(255, 0, 255)

typedef struct __bitmap_image {
  size_t width;
  size_t height;
  bitmap_byte data[];
} *bitmap_image;

bitmap_image bitmap_new(size_t width, size_t height);
void bitmap_free(bitmap_image *image_ptr);
bitmap_color bitmap_get_color(bitmap_image image, size_t x, size_t y);
void bitmap_set_color(bitmap_image image, size_t x, size_t y,
                      bitmap_color color);

bool bitmap_image_write(bitmap_image image, FILE *file);

#endif
