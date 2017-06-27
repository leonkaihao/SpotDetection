// cimp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "type.h"
#include "util.h"
#include "warp.h"
#include "spot.h"

//���ƹ���λ��
void drawSpotImg(uint8_t* data, bitmap_info_header_t ih, int x, int y) {
    const int PLOT_WIDTH = 7;
    int row = ih.height - PLOT_WIDTH/2 - y - 1;
    int start = ((ih.width * row) + x - PLOT_WIDTH / 2) * (ih.bitspp >> 3);
    int i = 0;
    memset(data, 0, ih.bmp_bytesz);
    for (i = 0; i < PLOT_WIDTH; ++i) {
        memset(&data[start], 255, PLOT_WIDTH * (ih.bitspp >> 3));
        start += (ih.width * (ih.bitspp >> 3));
    }
}

double calc_transform_time = 0, spot_time = 0, transform_time = 0;
SPoint2f g_ptSpot;
bool getSpot = false;
void FormatBenchmark(char* buf) {
    sprintf(buf,
        "*%s**************\n"
        "Spot location: [%.2f, %.2f]\n"
        "Generating transform: %.2fms, Locating spot: %.2fms, Transform: %.2fms"
        , getSpot ? " Find spot " : " Lose spot ", g_ptSpot.x, g_ptSpot.y, calc_transform_time, spot_time, transform_time);
}


int main(int argc, char* argv[])
{
    const int SCR_WIDTH = 1024;
    const int SCR_HEIGHT = 768;
    bitmap_info_header_t ih;
    uint8_t *in_bitmap_data = NULL;
    uint8_t* out_bitmap_data = NULL;
    STransform transform;
    char fileName[64];
    int startNum, stopNum;
    int i, num;

    //��һ��������bmpͼƬ��·��
    if (argc != 3) {
        printf("Usage: %s [start number] [end number]\n", argv[0]);
        return 1;
    }
    //����ͼƬ��ע��bmpͼƬ�ģ�0��0���������½ǣ����������Ͻ�

    startNum = atoi(argv[1]);
    stopNum = atoi(argv[2]);
    Spot spot;
    for (num = startNum; num <= stopNum; ++num) {
        sprintf(fileName, "%d.bmp", num);
        printf("===%s===\n", fileName);
        in_bitmap_data = load_bmp(fileName, &ih);
        if (in_bitmap_data == NULL) {
            fprintf(stderr, "main: BMP image %s not loaded.\n", fileName);
            return 1;
        }
        printf("Info: %d x %d x %d\n", ih.width, ih.height, ih.bitspp);
        if (num == startNum) {
            initSpot(&spot, ih.width, ih.height, ih.bitspp >> 3, true, 192);
        }

        //�㷨��ʼ
        //����ת������
        calc_transform_time = StartTime();
        transform = quadrilateralToQuadrilateral(
            157, 41, // top left  
            814, 51, // top right  
            758, 485,// bottom left  
            213, 450,
            0, 0,
            (float)(SCR_WIDTH - 1), 0,
            (float)(SCR_WIDTH - 1), (float)(SCR_HEIGHT - 1),
            0, (float)(SCR_HEIGHT - 1)
        );
        calc_transform_time = EndTime(calc_transform_time);

        //���ͼ���ϵĹ������
        spot_time = StartTime();
        getSpot = spot_simple(&spot, in_bitmap_data);
        spot_time = EndTime(spot_time);

        transform_time = StartTime();
        //ת������Ļ����
        transformPoints(&transform, 1, &spot.center);
        transform_time = EndTime(transform_time);
        //�㷨����
        char buf[256];
        g_ptSpot = spot.center;
        FormatBenchmark(buf);
        printf("%s\n", buf);
        int pathlen = SC_VECTOR_SZ_(&spot.findPath);
        for (i = 0; i < pathlen; ++i) {
            SPoint2i *pt = SC_VECTOR_GET_P_(&spot.findPath, i);
            printf("->[%d,%d]", pt->x, pt->y);
        }
        printf("\n************\n\n");

        //����
        ih.width = SCR_WIDTH;
        ih.height = SCR_HEIGHT;
        ih.bitspp = 24;
        ih.bmp_bytesz = SCR_WIDTH*SCR_HEIGHT*(ih.bitspp >> 3);
        out_bitmap_data = (uint8_t*)malloc(ih.bmp_bytesz);
        drawSpotImg(out_bitmap_data, ih, (int)spot.center.x, (int)spot.center.y);

        //����
        sprintf(fileName, "out_%d.bmp", num);
        if (save_bmp(fileName, &ih, out_bitmap_data)) {
            fprintf(stderr, "main: BMP image not saved.\n");
            return 1;
        }
        free(in_bitmap_data);
    }
    clearSpot(&spot);

    return 0;
}

