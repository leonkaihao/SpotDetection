#pragma once
#ifndef BOOL
typedef int BOOL;
#define FALSE 0
#define TRUE 1
#endif

#define MAX_BRIGHTNESS 255
// C99 doesn't define M_PI (GNU-C99 does)
#define M_PI 3.14159265358979323846264338327

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t nothing;
} rgb_t;

// Use short int instead `unsigned char' so that we can
// store negative values.
typedef short int pixel_t;


typedef struct {
    float x;
    float y;
}SPoint2f;

typedef struct {
    int x;
    int y;
}SPoint2i;


typedef struct {
    int x;
    int y;
    int width;
    int height;
}SRect2i;


typedef struct {
    float x;
    float y;
    float width;
    float height;
}SRect2f;


#define CV_DEFAULT_BUFSZ 64

#define SC_DEF_VECTOR_(type) \
typedef struct ScVector##type \
{\
    int size;\
    int cap;\
    int elemsz;\
    union {\
        type* ptr;\
        void* pv;\
    } data;\
}ScVector##type;
SC_DEF_VECTOR_(uint8_t)
SC_DEF_VECTOR_(short)
SC_DEF_VECTOR_(int)
SC_DEF_VECTOR_(float)
SC_DEF_VECTOR_(double)

#define SC_VECTOR_(type) ScVector##type
#define SC_VECTOR_SZ_(var) (var)->size
#define SC_VECTOR_INIT_(var, sz) \
if ((var)) { \
    int size = 0, cap = CV_DEFAULT_BUFSZ, esz = 0; \
    if (sz > 0) { \
        size = sz; \
        cap = sz + CV_DEFAULT_BUFSZ; \
    } \
    esz = sizeof((var)->data.ptr[0]);\
    (var)->data.pv = malloc(esz * cap); \
    (var)->size = size; \
    (var)->cap = cap; \
    (var)->elemsz = esz; \
}

#define SC_VECTOR_UNINIT_(var) \
if ((var) && (var)->data.pv){ \
    free((var)->data.pv); \
}\
(var)->data.ptr = NULL;

#define SC_VECTOR_RESZ_(var, sz) \
if ((var)) { \
    int size = (var)->size, cap = (var)->cap;\
    if (sz > cap) { \
        cap = sz + CV_DEFAULT_BUFSZ;\
    } \
    size = sz; \
    (var)->data.pv = realloc((var)->data.pv, (var)->elemsz * cap); \
    (var)->size = size; \
    (var)->cap = cap; \
}

#define SC_VECTOR_CLEAR_(var) \
(var)->size = 0;

#define SC_VECTOR_PUSH_(var, elem) \
if ((var)->data.pv) {\
    if ((var)->size == (var)->cap - 1) {\
        (var)->data.pv = realloc((var)->data.pv, (var)->elemsz * (var)->cap * 2);\
        if ((var)->data.pv) (var)->cap <<= 1;\
    }\
    (var)->data.ptr[(var)->size++] = elem;\
}

#define SC_VECTOR_POP_(var) \
if ((var)->size != 0) { (var)->size--;}

#define SC_VECTOR_GET_P_(var, i) \
((var)->size > i && i >= 0) ? &((var)->data.ptr[i]) : NULL