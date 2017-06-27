#include "stdafx.h"
#include "type.h"
#include "spot.h"
#include "util.h"

double time_findspot, time_calcspotcenter, time_calcprios, time_initspot;

//找到一个亮像素
static bool findSpot(Spot* instance, const void* data, SRect2i roi) {
    const int GRAY_MIN = instance->thresh, CHANN = instance->chnn;
    const PIXSTEP = CHANN * 3; //假设光点宽度大于3个像素，于是每次前进3个像素
    const uint8_t* pData = (uint8_t*)data;
    //spot的外接矩形
    int rowstep = instance->step;
    //当前行起点和结束点
    const uint8_t* pCurRow = pData + roi.y * rowstep;
    int start = roi.x * CHANN;
    int end = (roi.x + roi.width) * CHANN;
    //当前行点的最左边界
    int pos;
    //当前行
    int j;

    for (j = 0; j < roi.height; ++j) {
        for (pos = start; pos < end; pos += PIXSTEP) {
            if (pCurRow[pos] > GRAY_MIN) {
                instance->hit.x = pos / CHANN;
                instance->hit.y = j + roi.y;
                return true;
            }
        }
        pCurRow += rowstep;
    }
    return false;
}

//找spot中心
static void calcSpotCenter(Spot* instance, const void* data) {
    const int GRAY_MIN = instance->thresh, CHANN = instance->chnn;
    const int COLS = instance->cols, ROWS = instance->rows;
    uint8_t* pData = (uint8_t*)data;
    int step = instance->step;
    //当前行起点和结束点
    uint8_t* pCurRow = pData + instance->hit.y * step;
    uint8_t* pCurPos = NULL;
    int start = 0;
    int end = instance->cols * CHANN;
    //当前行点的最左最右的边界
    int left = instance->hit.x * CHANN, right = left, top = 0, bottom = 0;
    SPoint2i result;
    //找左边界
    int x = left - CHANN, y = 0;
    for (; x >= start; x -= CHANN) {
        if (pCurRow[x] <= GRAY_MIN) {
            left = x + CHANN;
            break;
        }
    }
    //找右边界
    for (x = right + CHANN; x < end; x += CHANN) {
        if (pCurRow[x] <= GRAY_MIN) {
            right = x - CHANN;
            break;
        }
    }
    result.x = (left + right) / (CHANN * 2);
    pCurPos = pCurRow + result.x;

    top = instance->hit.y, bottom = top;
    
    //找上边界
    for (y = top - 1; y >= 0; --y) {
        if (pCurRow[x] <= GRAY_MIN) {
            top = y;
            break;
        }
    }
    //找下边界
    for (y = bottom + 1; y < ROWS; --y) {
        if (pCurRow[x] <= GRAY_MIN) {
            bottom = y;
            break;
        }
    }
    result.y = (top + bottom) / 2;;

    instance->center.x = (float)result.x;
    instance->center.y = instance->revertY ? (float)(instance->rows - 1 - result.y) :(float)result.y;

    SRect2i edge;
    edge.width = right - left + 1;
    edge.height = bottom - top + 1;
    edge.x = left;
    edge.y = instance->revertY ? (instance->rows - top - edge.height) : top;
    instance->edge = edge;    
}

//
//static bool detectSpot(Spot* instance, void* data, SRect2i roi) {
//    const int GRAY_MIN = instance->thresh, GRAY_MAX = 255, CHANN = instance->chnn;
//    SRect2i roispot = { 0, 0, 0, 0 };
//    uint8_t* pData = (uint8_t*)data;
//    //spot的外接矩形
//    int top = 0, left = 0, right = 0, bottom = 0;
//    int step = instance->step;
//    //当前行起点和结束点
//    uint8_t* pCurRow = pData + roi.y * step;
//    int start = roi.x * CHANN;
//    int end = (roi.x + roi.width) * CHANN;
//    //当前行点的最左最右的边界
//    int l, r;
//    //当前行
//    int j;
//    bool findSpot = false;
//    for (j = 0; j < roi.height; ++j) {
//        for (l = start; l < end; l += CHANN) {
//            if (pCurRow[l] > GRAY_MIN) {
//                pCurRow[l] = GRAY_MAX;
//                top = bottom = j;
//                r = left = right = l;
//                findSpot = true;
//                break;
//            }
//        }
//        if (findSpot == true) {
//            break;
//        }
//        pCurRow += step;
//        start += step;
//        end += step;
//    }
//
//    if (findSpot == false) {
//        return false;
//    }
//
//    //从一个点开始扩散   
//    for (; j < roi.height; ++j) {
//        //找左边界
//        bool lFind = false;
//        int x = l - CHANN;
//        for (; x >= start; x -= CHANN) {
//            if (pCurRow[x] <= GRAY_MIN) {
//                l = x + CHANN;
//                break;
//            }
//            else {
//                lFind = true; //左延伸
//            }
//        }
//        if (!lFind) {//未找到左边界,则往右找亮点
//            for (x = l; x <= r; x += CHANN) {
//                if (pCurRow[x] > GRAY_MIN) {
//                    lFind = true; //左边界找到，收缩了
//                    l = x;
//                    break;
//                }
//            }
//        }
//
//        //找右边界
//        bool rFind = false;
//        for (int x = r + CHANN; x < end; x += CHANN) {
//            if (pCurRow[x] <= GRAY_MIN) {
//                r = x - CHANN;
//                break;
//            }
//            else {
//                rFind = true; //右延伸
//            }
//        }
//        if (!rFind) {//未找到右边界, 则往左找到第一个亮点
//            for (x = r; x >= l; x -= CHANN) {
//                if (pCurRow[x] > GRAY_MIN) {
//                    rFind = true; //右边界找到，收缩了
//                    r = x + CHANN;
//                    break;
//                }
//            }
//        }
//        if (!lFind && !rFind) {//没有左右边界，查找结束
//            break;
//        }
//
//        //
//        if (l < left) left = l;
//        if (r > right) right = r;
//        bottom = j;
//
//        pCurRow += step;
//        start += step;
//        end += step;
//    }
//    instance->edge.x = left / CHANN;
//    instance->edge.y = top;
//    instance->edge.width = (right + 1 - left) / CHANN;
//    instance->edge.height = bottom - top + 1;
//
//    return findSpot;
//}

void calcPrios(Spot* instance) {
    int i, j;
    for (i = 0; i < ROI_DIV; ++i) {
        for (j = 0; j < ROI_DIV; ++j) {
            SPrioTab* pCur = &instance->priosTab[i][j];
            int x, y, prio;
            for (y = 0; y < ROI_DIV; ++y) {
                SC_VECTOR_INIT_(&pCur->prios[y], 0);
            }
            for (y = 0; y < ROI_DIV; ++y) {
                for (x = 0; x < ROI_DIV; ++x) {
                    SPoint2i pt = { x, y };
                    int dx = j > x ? j - x : x - j;
                    int dy = i > y ? i - y : y - i;
                    prio = dx > dy ? dx : dy;
                    SC_VECTOR_PUSH_(&(pCur->prios[prio]), pt);
                }
            }
        }
    }
}

void initSpot(Spot* instance, int cols, int rows, int chnn, bool revertY, int thresh) {
    int dX = cols / ROI_DIV;
    int dY = rows / ROI_DIV;
    int y, x;

    instance->chnn = chnn;
    instance->revertY = revertY;
    instance->step = (cols * chnn + 3) >> 2 << 2;
    instance->thresh = thresh;
    instance->cols = cols;
    instance->rows = rows;
    for (y = 0; y < ROI_DIV; ++y) {
        for (x = 0; x < ROI_DIV; ++x) {
            int index = y * ROI_DIV + x;
            instance->rois[y][x].x = x * dX;
            instance->rois[y][x].y = y * dY;
            instance->rois[y][x].width = (x == (ROI_DIV - 1)) ? (cols - (ROI_DIV - 1) * dX) : dX;
            instance->rois[y][x].height = (y == (ROI_DIV - 1)) ? (rows - (ROI_DIV - 1) * dY) : dY;;
        }
    }
    instance->center.x = instance->center.y = 0;
    memset(&instance->edge, 0, sizeof(SRect2i));

    calcPrios(instance);
    
    SPoint2i pt = { 4, 3 };
    instance->hot = pt;
    SC_VECTOR_INIT_(&instance->findPath, 0);
}

void clearSpot(Spot* instance) {
    int y, x;
    for (y = 0; y < ROI_DIV; ++y) {
        for (x = 0; x < ROI_DIV; ++x) {
            SPrioTab* pCur = &instance->priosTab[y][x];
            int i = 0;
            for (i = 0; i < ROI_DIV; ++i) {
                SC_VECTOR_UNINIT_(&pCur->prios[i]);
            }
        }
    }
    memset(instance, 0, sizeof(instance));

}


bool spot_simple(Spot* instance, const void* data) {
    int i, j;
    bool find = false;
    SPrioTab* pCurPrio = &instance->priosTab[instance->hot.y][instance->hot.x];
    SC_VECTOR_CLEAR_(&instance->findPath);
    for (i = 0; i < ROI_DIV; ++i) {
        int len = SC_VECTOR_SZ_(&pCurPrio->prios[i]);
        for (j = 0; j < len; ++j) {
            SPoint2i* pt = SC_VECTOR_GET_P_(&pCurPrio->prios[i], j);
            SC_VECTOR_PUSH_(&instance->findPath, *pt);
            if (pt == NULL) return false;
            find = findSpot(instance, data, instance->rois[pt->y][pt->x]);
            if (find) {                
                instance->hot = *pt;
                calcSpotCenter(instance, data);
                return true;
            }
        }
    }
    return false;
}
