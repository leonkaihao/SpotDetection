#pragma once
#include "type.h"

#define ROI_DIV 7
#define ROI_NBLOCK  ROI_DIV * ROI_DIV

SC_DEF_VECTOR_(SPoint2i)

typedef struct {
    SC_VECTOR_(SPoint2i) prios[ROI_DIV]; //Ȩֵ��
}SPrioTab;

typedef struct {
    int cols;     //ͼ������
    int rows;     //ͼ������
    int step;     //һ�еĳ���
    int chnn;     //һ������ռ�����ֽ�
    bool revertY; //��ֱ���귴ת
    int thresh;   //�Ҷ�����

    SRect2i rois[ROI_DIV][ROI_DIV]; //����
    SPoint2i hot;//�ȵ�����
    SC_VECTOR_(SPoint2i) findPath;

    SPrioTab priosTab[ROI_DIV][ROI_DIV];//�ȵ�������������ʱ�����������Ȩֵ

    SPoint2i hit; //��һ���ҵ��ĵ�
    SRect2i edge; //���߽�
    SPoint2f center; //�������
}Spot;

void initSpot(Spot* instance, int cols, int rows, int chnn, bool revertY, int thresh);
bool spot_simple(Spot* instance, const void* data);
void clearSpot(Spot* instance);