#pragma once
#include<string>
#include<iostream>
#include<fstream>
#include<cmath>
#include <windows.h>
#include <Tlhelp32.h>
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc.hpp>

#include "AssistConfig.h"
#include "YOLOv6.h"

using namespace cv;
using namespace std;


//���������ṹ��
/*
typedef struct DetectResults
{
    std::vector< int > classIds;
    std::vector< float > confidences;
    std::vector< Rect > boxes;
    int maxPersonConfidencePos; //���� ���ִ�ж�
} DETECTRESULTS; */

// ����opencv dnn��ͼ������
// ͼ��������װ�࣬ʹ��AIģ�ͽ�ȡ��Ļ������ж�����
class ImageDetectionOnnx
{
public:
    ImageDetectionOnnx();
    ~ImageDetectionOnnx();
    void ReInit();

    void getScreenshot();
    DETECTRESULTS detectImg();
    cv::Mat getImg();

private:
    void initImg(); //��ʼ��ͼ������Դ
    void releaseImg(); //�ͷ�ͼ������Դ

private:
    static AssistConfig* m_AssistConfig;

    //����ͼ�������������
    HDC m_screenDC;
    HDC m_memDC;
    HBITMAP m_hBitmap;
    BITMAPINFO* m_Bitmapinfo = NULL;

    //���ͼ�����ݵ�mat����
    //ע��opencv�ĺ���ʹ���������ʱ���Զ�����create�����������ݴ������
    //�������ط�ʹ��ʱ����Ҫ�ֶ�����create�����ڴ洴�����飻�ظ�ִ��create�������ᵼ���ظ��������ݴ�����飻
    cv::Mat m_mat_src; //���bitmap�л�ȡ��Դͼ
    cv::Mat m_mat_3; //���ת��Ϊ3ͨ����ͼ��

    //AI�����������
    //const string ModelFile = "./model/yolov6n-320x320.onnx";
    //const string ModelFile = "../../Data/model/onnx/yolov6n-320x320.onnx";
    const string ModelFile = "./model/onnx/yolov6n-320x320.onnx";
    //const string ModelFile = "../../Data/model/onnx/yolov6n.onnx";
    //const string ModelFile = "../../Data/model/onnx/yolov6n-0.2.onnx";
    const float MinConfidence = 0.50; //��С���Ŷ�
    const int PersonClassId = 1; //�����ǩ�б��� ���� ��classid
    
    //ʹ��ר�ŵĶ�����ģ����
    onnxlib::YOLOv6* yolov6 = NULL ;

};






