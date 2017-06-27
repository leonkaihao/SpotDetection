#include "stdafx.h"
#include "type.h"
#include "warp.h"

STransform InitTransform(float inA11, float inA21,
    float inA31, float inA12,
    float inA22, float inA32,
    float inA13, float inA23,
    float inA33) {
    STransform trans = {
        inA11, inA12, inA13,
        inA21, inA22, inA23,
        inA31, inA32, inA33 
    };
    return trans;
}

STransform squareToQuadrilateral(float x0, float y0, float x1, float y1, float x2,
    float y2, float x3, float y3) {
    float dx3 = x0 - x1 + x2 - x3;
    float dy3 = y0 - y1 + y2 - y3;
    if (dx3 == 0.0f && dy3 == 0.0f) {
        STransform result = InitTransform(x1 - x0, x2 - x1, x0, y1 - y0, y2 - y1, y0, 0.0f,
            0.0f, 1.0f);
        return result;
    }
    else {
        float dx1 = x1 - x2;
        float dx2 = x3 - x2;
        float dy1 = y1 - y2;
        float dy2 = y3 - y2;
        float denominator = dx1 * dy2 - dx2 * dy1;
        float a13 = (dx3 * dy2 - dx2 * dy3) / denominator;
        float a23 = (dx1 * dy3 - dx3 * dy1) / denominator;
        STransform result = InitTransform(x1 - x0 + a13 * x1, x3 - x0 + a23 * x3, x0, y1 - y0
            + a13 * y1, y3 - y0 + a23 * y3, y0, a13, a23, 1.0f);
        return result;
    }
}

STransform buildAdjoint(STransform* t) {
    // Adjoint is the transpose of the cofactor matrix:  
    STransform result = InitTransform(
        t->a22 * t->a33 - t->a23 * t->a32, t->a23 * t->a31 - t->a21 * t->a33, t->a21 * t->a32 - t->a22 * t->a31, 
        t->a13 * t->a32 - t->a12 * t->a33, t->a11 * t->a33 - t->a13 * t->a31, t->a12 * t->a31 - t->a11 * t->a32, 
        t->a12 * t->a23 - t->a13 * t->a22, t->a13 * t->a21 - t->a11 * t->a23, t->a11 * t->a22 - t->a12 * t->a21);
    return result;
}

STransform quadrilateralToSquare(float x0, float y0, float x1, float y1, float x2,
    float y2, float x3, float y3) {
    // Here, the adjoint serves as the inverse:  
    STransform result = squareToQuadrilateral(x0, y0, x1, y1, x2, y2, x3, y3);
    return buildAdjoint(&result);
}


STransform times(STransform* t, STransform* other) {

    STransform result = InitTransform(t->a11 * other->a11 + t->a21 * other->a12 + t->a31 * other->a13,
        t->a11 * other->a21 + t->a21 * other->a22 + t->a31 * other->a23, t->a11 * other->a31 + t->a21 * other->a32 + t->a31
        * other->a33, t->a12 * other->a11 + t->a22 * other->a12 + t->a32 * other->a13, t->a12 * other->a21 + t->a22
        * other->a22 + t->a32 * other->a23, t->a12 * other->a31 + t->a22 * other->a32 + t->a32 * other->a33, t->a13
        * other->a11 + t->a23 * other->a12 + t->a33 * other->a13, t->a13 * other->a21 + t->a23 * other->a22 + t->a33
        * other->a23, t->a13 * other->a31 + t->a23 * other->a32 + t->a33 * other->a33);
    return result;
}

STransform quadrilateralToQuadrilateral(float x0, float y0, float x1, float y1,
    float x2, float y2, float x3, float y3, float x0p, float y0p, float x1p, float y1p, float x2p, float y2p,
    float x3p, float y3p) {
     STransform qToS, sToQ;
    qToS = quadrilateralToSquare(x0, y0, x1, y1, x2, y2, x3, y3);
    sToQ = squareToQuadrilateral(x0p, y0p, x1p, y1p, x2p, y2p, x3p, y3p);
    return times(&sToQ, &qToS);
}

void transformPoints(STransform* t, int num, SPoint2f* points) {
    int max = num;
    for (int i = 0; i < max; i++) {
        float x = points[i].x;
        float y = points[i].y;
        float denominator = t->a13 * x + t->a23 * y + t->a33;
        points[i].x = (t->a11 * x + t->a21 * y + t->a31) / denominator;
        points[i].y = (t->a12 * x + t->a22 * y + t->a32) / denominator;
    }
}