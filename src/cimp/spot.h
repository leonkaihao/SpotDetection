#pragma once
#include "type.h"

#define ROI_DIV 7
#define ROI_NBLOCK  ROI_DIV * ROI_DIV

SC_DEF_VECTOR_(SPoint2i)

typedef struct {
    SC_VECTOR_(SPoint2i) prios[ROI_DIV]; //权值表
}SPrioTab;

typedef struct {
    int cols;     //图像行数
    int rows;     //图像列数
    int step;     //一行的长度
    int chnn;     //一个像素占多少字节
    bool revertY; //垂直坐标反转
    int thresh;   //灰度下限

    SRect2i rois[ROI_DIV][ROI_DIV]; //区块
    SPoint2i hot;//热点区块
    SC_VECTOR_(SPoint2i) findPath;

    SPrioTab priosTab[ROI_DIV][ROI_DIV];//热点在任意区块上时，整个矩阵的权值

    SPoint2i hit; //第一个找到的点
    SRect2i edge; //光点边界
    SPoint2f center; //光点中心
}Spot;

void initSpot(Spot* instance, int cols, int rows, int chnn, bool revertY, int thresh);
bool spot_simple(Spot* instance, const void* data);
void clearSpot(Spot* instance);