#include "stdafx.h"
#include "type.h"
#include "util.h"
#ifdef _WIN32
#include <windows.h>
#endif


char* load_bmp(const char *filename,
    bitmap_info_header_t *bitmapInfoHeader)
{
    FILE *filePtr = fopen(filename, "rb");
    bmpfile_magic_t mag;
    bmpfile_header_t bitmapFileHeader; // our bitmap file header
    char *bitmapImage;
    size_t len;

    if (filePtr == NULL) {
        perror("fopen()");
        return NULL;
    }

    if (fread(&mag, sizeof(bmpfile_magic_t), 1, filePtr) != 1) {
        fclose(filePtr);
        return NULL;
    }

    // verify that this is a bmp file by check bitmap id
    // warning: dereferencing type-punned pointer will break
    // strict-aliasing rules [-Wstrict-aliasing]
    if (*((uint16_t*)mag.magic) != 0x4D42) {
        fprintf(stderr, "Not a BMP file: magic=%c%c\n",
            mag.magic[0], mag.magic[1]);
        fclose(filePtr);
        return NULL;
    }

    // read the bitmap file header
    if (fread(&bitmapFileHeader, sizeof(bmpfile_header_t),
        1, filePtr) != 1) {
        fclose(filePtr);
        return NULL;
    }

    // read the bitmap info header
    if (fread(bitmapInfoHeader, sizeof(bitmap_info_header_t),
        1, filePtr) != 1) {
        fclose(filePtr);
        return NULL;
    }

    if (bitmapInfoHeader->compress_type != 0)
        fprintf(stderr, "Warning, compression is not supported.\n");

    // move file point to the beginning of bitmap data
    if (fseek(filePtr, bitmapFileHeader.bmp_offset, SEEK_SET)) {
        fclose(filePtr);
        return NULL;
    }

    // allocate enough memory for the bitmap image data
    int bytesz = (((bitmapInfoHeader->width * bitmapInfoHeader->bitspp >> 3) + 3) >> 2 << 2) * bitmapInfoHeader->height;
    bitmapImage = malloc(bytesz);

    // verify memory allocation
    if (bitmapImage == NULL) {
        fclose(filePtr);
        return NULL;
    }

    // read in the bitmap image data
    len = fread(bitmapImage, sizeof(char), bytesz, filePtr);
    if (len != bytesz) {
        fclose(filePtr);
        return NULL;
    }

    // If we were using unsigned char as pixel_t, then:
    // fread(bitmapImage, 1, bitmapInfoHeader->bmp_bytesz, filePtr);

    // close file and return bitmap image data
    fclose(filePtr);
    return bitmapImage;
}

// Return: true on error.
bool save_bmp(const char *filename, const bitmap_info_header_t *bmp_ih,
    const char *data)
{
    FILE* filePtr = fopen(filename, "wb");
    bmpfile_magic_t mag = { { 0x42, 0x4d } };
    uint32_t offset;
    bmpfile_header_t bmp_fh;
    size_t i;

    if (filePtr == NULL)
        return TRUE;

    if (fwrite(&mag, sizeof(bmpfile_magic_t), 1, filePtr) != 1) {
        fclose(filePtr);
        return TRUE;
    }

    offset = sizeof(bmpfile_magic_t) +
        sizeof(bmpfile_header_t) +
        sizeof(bitmap_info_header_t) +
        (255 * sizeof(rgb_t));

    bmp_fh.filesz = offset + bmp_ih->bmp_bytesz;
    bmp_fh.filesz = offset + bmp_ih->bmp_bytesz;
    bmp_fh.creator1 = 0;
    bmp_fh.creator2 = 0;
    bmp_fh.bmp_offset = offset;

    if (fwrite(&bmp_fh, sizeof(bmpfile_header_t), 1, filePtr) != 1) {
        fclose(filePtr);
        return TRUE;
    }
    if (fwrite(bmp_ih, sizeof(bitmap_info_header_t), 1, filePtr) != 1) {
        fclose(filePtr);
        return TRUE;
    }

    // Palette
    for (i = 0; i < 255; i++) {
        const rgb_t color = { (uint8_t)i, (uint8_t)i, (uint8_t)i };
        if (fwrite(&color, sizeof(rgb_t), 1, filePtr) != 1) {
            fclose(filePtr);
            return TRUE;
        }
    }


    if (fwrite(data, sizeof(char), bmp_ih->bmp_bytesz, filePtr) != bmp_ih->bmp_bytesz) {
        fclose(filePtr);
        return TRUE;
    }

    fclose(filePtr);
    return FALSE;
}

double StartTime()
{
#if defined (_WIN32)
    LARGE_INTEGER StartingTime;
    QueryPerformanceCounter(&StartingTime);
    return (double)StartingTime.QuadPart;
#elif defined (_LINUX)
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    double milliseconds = (double)te.tv_sec * 1000 + (double)te.tv_usec / 1000; // caculate milliseconds
    return milliseconds;
#else 
    return 0.0;
#endif

}

//i = 0 -> microseconds 
//i = 1 -> milliseconds 
//i = 2/any -> seconds
double EndTime(double start)
{
#if defined (_WIN32)
    LARGE_INTEGER Frequency;
    LARGE_INTEGER EndingTime;
    double duration;

    QueryPerformanceFrequency(&Frequency);

    QueryPerformanceCounter(&EndingTime);
    duration = (double)EndingTime.QuadPart - start;

    return duration * 1000 / (double)Frequency.QuadPart;
#elif defined (_LINUX)
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    double milliseconds = (double)te.tv_sec * 1000 + (double)te.tv_usec / 1000; // caculate milliseconds
    return milliseconds - start;
#else
    return 0.0;
#endif
}