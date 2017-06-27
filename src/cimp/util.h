#pragma once

/*
* Loading part taken from
* http://www.vbforums.com/showthread.php?t=261522
* BMP info:
* http://en.wikipedia.org/wiki/BMP_file_format
*
* Note: the magic number has been removed from the bmpfile_header_t
* structure since it causes alignment problems
*     bmpfile_magic_t should be written/read first
* followed by the
*     bmpfile_header_t
* [this avoids compiler-specific alignment pragmas etc.]
*/

typedef struct {
    uint8_t magic[2];
} bmpfile_magic_t;

typedef struct {
    uint32_t filesz;
    uint16_t creator1;
    uint16_t creator2;
    uint32_t bmp_offset;
} bmpfile_header_t;

typedef struct {
    uint32_t header_sz;
    int32_t  width;
    int32_t  height;
    uint16_t nplanes;
    uint16_t bitspp;
    uint32_t compress_type;
    uint32_t bmp_bytesz;
    int32_t  hres;
    int32_t  vres;
    uint32_t ncolors;
    uint32_t nimpcolors;
} bitmap_info_header_t;

char* load_bmp(const char *filename,
    bitmap_info_header_t *bitmapInfoHeader);

bool save_bmp(const char *filename, const bitmap_info_header_t *bmp_ih,
    const char *data);

#define RGB2GRAY(R,G,B) ((R*19595 + G*38469 + B*7472) >> 16)

double StartTime();
double EndTime(double start);