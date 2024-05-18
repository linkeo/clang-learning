#include "image/bitmap.h"
#include "struct/bool.h"
#include "util/debug.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bitmap_image bitmap_new(size_t width, size_t height) {
  bitmap_image image;
  size_t byte_size = sizeof(bitmap_byte) * width * height * BYTES_PER_PIXEL;
  image = (bitmap_image)malloc(sizeof(*image) + byte_size);
  memset(image->data, (bitmap_byte)(255), byte_size);
  image->width = width;
  image->height = height;
  return image;
}

void bitmap_free(bitmap_image *image_ptr) {
  if (image_ptr) {
    free(*image_ptr);
    *image_ptr = NULL;
  }
}

inline bitmap_color bitmap_get_color(bitmap_image image, size_t x, size_t y) {
  bitmap_byte *ptr = image->data + (x + y * image->width) * BYTES_PER_PIXEL;
  return bitmap_rgb(ptr[2], ptr[1], ptr[0]);
}

inline void bitmap_set_color(bitmap_image image, size_t x, size_t y,
                             bitmap_color color) {
  bitmap_byte *ptr = image->data + (x + y * image->width) * BYTES_PER_PIXEL;
  ptr[2] = color.red;
  ptr[1] = color.green;
  ptr[0] = color.blue;
}

static bool bitmap_image_write_file_header(bitmap_image image, FILE *file,
                                           size_t file_size);
static bool bitmap_image_write_info_header(bitmap_image image, FILE *file);
bool bitmap_image_write(bitmap_image image, FILE *file) {
  debugf("bitmap_image_write start\n");
  if (!file) {
    debugf("bitmap_image_write no file\n");
    return false;
  }
  size_t width_bytes = image->width * BYTES_PER_PIXEL;
  bitmap_byte padding[3] = {0, 0, 0};
  size_t padding_size = (4 - (width_bytes) % 4) % 4;
  size_t stride = width_bytes + padding_size;

  size_t file_size =
      FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * image->height);

  debugf("bitmap_image_write_file_header start\n");
  if (!bitmap_image_write_file_header(image, file, file_size)) {
    return false;
  }
  debugf("bitmap_image_write_file_header end\n");

  debugf("bitmap_image_write_info_header start\n");
  if (!bitmap_image_write_info_header(image, file)) {
    return false;
  }
  debugf("bitmap_image_write_info_header end\n");

  for (int i = 0; i < image->height; i++) {
    if (!fwrite(image->data + (i * width_bytes), BYTES_PER_PIXEL, image->width,
                file)) {
      return false;
    }
    if (!fwrite(padding, 1, padding_size, file)) {
      return false;
    }
  }

  return true;
}

static bool bitmap_image_write_file_header(bitmap_image image, FILE *file,
                                           size_t file_size) {
  bitmap_byte header_bytes[FILE_HEADER_SIZE] = {
      0, 0,       /// signature
      0, 0, 0, 0, /// image file size in bytes
      0, 0, 0, 0, /// reserved
      0, 0, 0, 0, /// start of pixel array
  };

  header_bytes[0] = (bitmap_byte)('B');
  header_bytes[1] = (bitmap_byte)('M');
  header_bytes[2] = (bitmap_byte)(file_size);
  header_bytes[3] = (bitmap_byte)(file_size >> 8);
  header_bytes[4] = (bitmap_byte)(file_size >> 16);
  header_bytes[5] = (bitmap_byte)(file_size >> 24);
  header_bytes[10] = (bitmap_byte)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

  return !!fwrite(header_bytes, FILE_HEADER_SIZE, 1, file);
}

static bool bitmap_image_write_info_header(bitmap_image image, FILE *file) {
  bitmap_byte header_bytes[INFO_HEADER_SIZE] = {
      0, 0, 0, 0, /// header size
      0, 0, 0, 0, /// image width
      0, 0, 0, 0, /// image height
      0, 0,       /// number of color planes
      0, 0,       /// bits per pixel
      0, 0, 0, 0, /// compression
      0, 0, 0, 0, /// image size
      0, 0, 0, 0, /// horizontal resolution
      0, 0, 0, 0, /// vertical resolution
      0, 0, 0, 0, /// colors in color table
      0, 0, 0, 0, /// important color count
  };

  header_bytes[0] = (bitmap_byte)(INFO_HEADER_SIZE);
  header_bytes[4] = (bitmap_byte)(image->width);
  header_bytes[5] = (bitmap_byte)(image->width >> 8);
  header_bytes[6] = (bitmap_byte)(image->width >> 16);
  header_bytes[7] = (bitmap_byte)(image->width >> 24);
  header_bytes[8] = (bitmap_byte)(image->height);
  header_bytes[9] = (bitmap_byte)(image->height >> 8);
  header_bytes[10] = (bitmap_byte)(image->height >> 16);
  header_bytes[11] = (bitmap_byte)(image->height >> 24);
  header_bytes[12] = (bitmap_byte)(1);
  header_bytes[14] = (unsigned char)(BYTES_PER_PIXEL * 8);

  return !!fwrite(header_bytes, INFO_HEADER_SIZE, 1, file);
}
