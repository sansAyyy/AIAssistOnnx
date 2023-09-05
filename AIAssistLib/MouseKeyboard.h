#pragma once

#include <iostream>
#include <string>
#include<math.h>

//#include "mouse.h"
//#include "keyboard.h"

#include "AssistConfig.h"
#include "ImageDetection.h"


enum MouseKeyboardType { MKTYPE_USBDEVICE, MKTYPE_HIDDRIVER, MKTYPE_WINDOWSEVENT };

//����������Ϣ�ṹ��
typedef struct WeaponInfo
{
	int bag; //���Ŀǰʹ���ĸ�������ȱʡ������1��ǹ������2�ѻ�ǹ������3��ǹ����
	int gunType; //ǹе����
	int telescopeType; //��������
}WEAPONINFO;

// �����̲�����װ��
// ���ȼ�˳��Ӳ��USB�豸����δʵ�֣���HIDDriver������windows�¼�
class MouseKeyboard
{
public:
	MouseKeyboard();
	~MouseKeyboard();

	//����ʹ�õ�ģ���������
	static MouseKeyboardType m_type;
	//��������ƶ����
	//����һ��3D��Ϸ�ƶ����ĺ�����x1/y1Ϊ��Ϸ���ĵ����꣬x2/y2Ϊ��⵽���������ĵ����꣬zΪ��ά�����z�����
	//mouseMoveSlow�������ı���
	static void MouseMove(LONG x1, LONG y1, LONG x2, LONG y2, double z, double mouseMoveSlow);
	static void MouseMove(LONG x1, LONG y1);
	//��굥��
	static void MouseLBClick();
	//����������
	static void MouseLBUp();
	//��갴�����
	static void MouseLBDown();


	//�ж��Ƿ��Ѿ���׼Ŀ��
	bool IsInTarget(DETECTRESULTS detectResult, WEAPONINFO weaponInfo);
	//�ж��Ƿ���Ҫ�Զ�����
	void AutoFire(DETECTRESULTS detectResult, WEAPONINFO weaponInfo);
	//�ƶ���굽��������������
	void AutoMove(DETECTRESULTS detectResult, WEAPONINFO weaponInfo);
	//ѹǹ����
	void AutoPush(WEAPONINFO weaponInfo);
	//�Զ���ǹ����Զ�ѹǹ
	void AutoPushAfterFire(WEAPONINFO weaponInfo);

public:
	// Ψһ��ʵ������
	static AssistConfig* m_AssistConfig;

	//HIDDriver������ģ���������
	//static Mouse m_hidMouse;
	//static Keyboard m_hidKeyboard;

};
