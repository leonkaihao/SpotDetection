#pragma once
#include "type.h"

typedef struct STransform {
    float a11, a12, a13,
        a21, a22, a23,
        a31, a32, a33;
}STransform;
STransform InitTransform(float inA11, float inA21,
    float inA31, float inA12,
    float inA22, float inA32,
    float inA13, float inA23,
    float inA33);
STransform squareToQuadrilateral(float x0, float y0, float x1, float y1, float x2,
    float y2, float x3, float y3);
STransform buildAdjoint(STransform* t);
STransform quadrilateralToSquare(float x0, float y0, float x1, float y1, float x2,
    float y2, float x3, float y3);
STransform times(STransform* t, STransform* other);
STransform quadrilateralToQuadrilateral(float x0, float y0, float x1, float y1,
    float x2, float y2, float x3, float y3, float x0p, float y0p, float x1p, float y1p, float x2p, float y2p,
    float x3p, float y3p);
void transformPoints(STransform* t, int num, SPoint2f* points);