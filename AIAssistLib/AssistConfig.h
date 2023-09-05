#pragma once
#include<string>
#include<iostream>
#include<windows.h>
#include<winuser.h>
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

//���������ṹ��
typedef struct DetectResults
{
    std::vector< int > classIds;
    std::vector< float > confidences;
    std::vector< cv::Rect > boxes;
    int maxPersonConfidencePos; //���� ������Ŷ����ڵ�λ��

    std::vector< float > xvals; //�����ĵ�ľ�������
    std::vector< float > yvals;
} DETECTRESULTS;

// ��������ģ���࣬��UI������Ϣ��Ӧ������ģʽ̫�鷳��ֱ�Ӷ���һ����̬����
class AssistConfig
{
public:
    // ��ȡ��ʵ��
    static AssistConfig* GetInstance();

    //�����������������
    //����ʵ�ʼ������ֻ�����׼���ĵ�һС�����򣬱��ϼ���ٶ�
    void ReCalDetectionRect();

    //��ȡ��Ļ���ű���
    double static getScreensZoom();

    //������Ϸ��������������Ϸ�������
    void changGameName(string newGameName);


    //��������ģ��
    //ѡ�����Ϸ���̣���ͬ����Ϸ������������һ��
    //ע������������ڱ����Ǵ�һ���ո�ģ��е��
    int gameLength = 5; //�����������鳤��,C++û�л�ȡ���鳤�ȵĺ������ñ��취ֱ������һ��ֵ
    string gameNames[5] = { "SUPER PEOPLE  ", "�������� ", "��ս", "��Խ����", "������Ϸ"};
    int maxModelWidths[5] = { 300, 300, 260, 260, 260 };
    int playerCentXs[5] = { screenRect.width / 2 - detectRect.x, screenRect.width * 860 / 1920 - detectRect.x, 
        screenRect.width / 2 - detectRect.x, screenRect.width / 2 - detectRect.x, screenRect.width / 2 - detectRect.x };
    //ѹǹ�������ݣ���������1�ͱ���2��ѹǹ����
    int pushHeights[5][2] = { {11,20}, {20,10}, {0,0}, {0,0}, {0,0} };
    //��ǰ��Ϸ��ѹǹ�������ݣ���������1�ͱ���2��ѹǹ���ݣ���ѡ����Ϸʱ�������������ȡԤ��ֵ
    int pushHeight[2] = { 0, 0 };

    int gameIndex = 0; //��Ϸ����ID
    string gameName = "��ѡ����Ϸ"; //��Ϸ��������
    int maxModelWidth = maxModelWidths[0]; //��Ϸģ�������
    int playerCentX = playerCentXs[0]; //��Ϸ��ұ���ͼ���ټ�������ڵ�λ��,ȱʡ������������Ϸ����

    //�Ƿ�����ͼ����
    bool detectImg = true;
    //�û����õ�ʵ�ʼ���Ⱥ͸߶�
    int detectWidth = 480, detectHeight = 320;
    //��Ϸ����ռ�õ�ԭʼ��Ļ����
    cv::Rect screenRect = {0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN) };
    //�ü����ʵ��ͼ��������
    cv::Rect detectRect = { 0,0,detectWidth,detectHeight };
    //����������ĵ�
    int detectCentX = detectRect.x + detectRect.width / 2, detectCentY = detectRect.y + detectRect.height / 2;
    //������Ļ���ź�ģ��ü����ʵ��ͼ��������
    cv::Rect detectZoomRect = { 0,0,detectWidth,detectHeight };

    //ͼ�������������ĳЩ�����£����Զ�����ʱʵ���Զ�ѹǹ����ʱ����㣨ÿ�μ��Լ20ms��
    long detectCount = 0;
    //�ϴ��Զ�����ʱ�ļ�����
    long preDetectCount = 0;


    //�Ƿ��Զ�׷��
    bool autoTrace = false;
    //�Զ�׷�����ͣ�1�ǳ���׷�١�2������Ҽ���׼����׷��
    double mouseMoveSlow = 1.6; //��������ƶ��ٶȣ�ֵ����С��1
    int autoTraceType = 1;
    int autoTraceTimeOut = 0; //����Ҽ���׼����׷�ٵ���ʱ��ȱʡ��30������

    //�Ƿ��Զ�����
    bool autoFire = false;
    //ǹе�������,1�ǵ��㡢2��3���㡢3��6���㡢4�ǳ�����ǹ
    int gunType = 1;
    //������ǹʱ��
    int autoFireTime = 600;

    //�Ƿ��Զ�ѹǹ
    bool autoPush = false;
    //��ǹ���ѹǹ����
    int maxBuPushCount = 100;
    //�ѻ�ǹ���ѹǹ����
    int maxJuPushCount = maxBuPushCount;

    //�Ƿ����׼��
    bool drawAim = true;

    //�û�ԭʼ����ֵ
    bool autoTraceUserSet = false;
    bool autoFireUserSet = false;
    bool autoPushUserSet = false;


private:
    AssistConfig();
    ~AssistConfig();
    void test();

private:
    // Ψһ��ʵ������
    static AssistConfig* m_config;
};

