#include "pch.h"
#include "MouseKeyboard.h"


//��ʼ����̬��Ա����
AssistConfig* MouseKeyboard::m_AssistConfig = AssistConfig::GetInstance();
MouseKeyboardType MouseKeyboard::m_type = MKTYPE_WINDOWSEVENT;
//Mouse MouseKeyboard::m_hidMouse = Mouse();
//Keyboard MouseKeyboard::m_hidKeyboard = Keyboard();


MouseKeyboard::MouseKeyboard() {

    //����ʹ�õ�ģ���������Ϊwindow�¼�
    m_type = MKTYPE_WINDOWSEVENT;

    /*
    try {
        m_hidMouse.initialize();
        m_hidKeyboard.initialize();

        //���HIDDRIVER������ʼ���ɹ�������ʹ�õ�ģ���������ΪHIDDRIVER����
        if (m_hidMouse.isInitialized() && m_hidKeyboard.isInitialized()) {
            m_type = MKTYPE_HIDDRIVER;
        }
    }
    catch (const std::runtime_error& e) {
        std::cout << std::string("����豸��ʼ��ʧ��: ") + e.what() << std::endl;
    }
    */

	return;
}

MouseKeyboard::~MouseKeyboard() {

	return;
}

void MouseKeyboard::MouseMove(LONG x1, LONG y1, LONG x2, LONG y2, double z, double mouseMoveSlow) {
    //����Z�������Ҫ̫��Ҳ����̫С
    if (z > 6) z = 6;
    if (z < 2) z = 2;

    //����ģ���������ִ������ƶ�
    /*
    if (m_type == MKTYPE_HIDDRIVER) {
        long x = abs(x2 - x1) * mouseMoveSlow / z;
        long y = abs(y2 - y1) * mouseMoveSlow / z;

        CHAR xSpeed = static_cast<CHAR>(m_hidMouse.getSpeedByRange(x));
        xSpeed = (x2 > x1 ? xSpeed : -xSpeed);

        CHAR ySpeed = static_cast<CHAR>(m_hidMouse.getSpeedByRange(y));
        ySpeed = (y2 > y1 ? ySpeed : -ySpeed);

        m_hidMouse.sendMouseReport(xSpeed, ySpeed);
    }
    else if (m_type == MKTYPE_WINDOWSEVENT) {
        long x = (x2 - x1) * mouseMoveSlow / z;
        long y = (y2 - y1) * mouseMoveSlow / z;
        //mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, fx, fy, 0, 0);
        mouse_event(MOUSEEVENTF_MOVE, x, y, 0, 0);
    }
    */

    long x = (x2 - x1) * mouseMoveSlow / z;
    long y = (y2 - y1) * mouseMoveSlow / z;
    //mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, fx, fy, 0, 0);
    mouse_event(MOUSEEVENTF_MOVE, x, y, 0, 0);
}

void  MouseKeyboard::MouseMove(LONG x, LONG y) {
    //����ģ���������ִ������ƶ�
    /*
    if (m_type == MKTYPE_HIDDRIVER) {
        long x1 = abs(x);
        long y1 = abs(y);

        CHAR xSpeed = static_cast<CHAR>(m_hidMouse.getSpeedByRange(x1));
        xSpeed = (x1 > 0 ? xSpeed : -xSpeed);

        CHAR ySpeed = static_cast<CHAR>(m_hidMouse.getSpeedByRange(y1));
        ySpeed = (y1 > 0 ? ySpeed : -ySpeed);

        m_hidMouse.sendMouseReport(xSpeed, ySpeed);
    }
    else if (m_type == MKTYPE_WINDOWSEVENT) {
        //mouse_event(MOUSEEVENTF_MOVE, x, y, 0, 0);
        INPUT inputs[1];
        inputs[0].type = INPUT_MOUSE; //�����Ϣ
        inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE; //MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE; ����ƶ��¼� + ����λ��
        inputs[0].mi.dx = x; //����������Ǳ�׼����ϵ��ֵ
        inputs[0].mi.dy = y;
        SendInput(1, inputs, sizeof(INPUT));
    }
    */

    INPUT inputs[1];
    inputs[0].type = INPUT_MOUSE; //�����Ϣ
    inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE; //MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE; ����ƶ��¼� + ����λ��
    inputs[0].mi.dx = x; //����������Ǳ�׼����ϵ��ֵ
    inputs[0].mi.dy = y;
    SendInput(1, inputs, sizeof(INPUT));
}

void MouseKeyboard::MouseLBClick() {
    //ģ�����ִ�е��
    /*
    if (m_type == MKTYPE_HIDDRIVER) {
        m_hidMouse.leftButtonClick();
    }
    else if (m_type == MKTYPE_WINDOWSEVENT) {
        //mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        INPUT inputs[2] = {};
        ZeroMemory(inputs, sizeof(inputs));

        inputs[0].type = INPUT_MOUSE;
        inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        inputs[1].type = INPUT_MOUSE;
        inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

        UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    }
    */

    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

void MouseKeyboard::MouseLBUp() {
    //ģ������������

    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

void MouseKeyboard::MouseLBDown() {
    //ģ������������
   
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

//�ж��Ƿ��Ѿ���׼Ŀ��
bool MouseKeyboard::IsInTarget(DETECTRESULTS detectResult, WEAPONINFO weaponInfo) {
    bool ret = false;

    if (detectResult.maxPersonConfidencePos >= 0 && detectResult.boxes.size() > 0) {
        //ʹ�ü���õ���Ϸ��Ļ��������
        LONG x1 = m_AssistConfig->detectCentX;
        LONG y1 = m_AssistConfig->detectCentY;

        //������Ա���������꣬����Ϊ���ϵ�λ�ã�������ͷ
        Rect rect = detectResult.boxes.at(detectResult.maxPersonConfidencePos);
        LONG x2 = m_AssistConfig->detectRect.x + rect.x + rect.width / 2;
        LONG y2 = m_AssistConfig->detectRect.y + rect.y + rect.height / 3;

        //���������ѹǹ����Ա�������������ƶ�
        if (m_AssistConfig->autoPush) {
            //ֻ��1��2����ѹǹ
            switch (weaponInfo.bag)
            {
            case 1:
                //����1������������
                y2 += rect.height / 5;
                break;
            case 2:
                //����2��4��������
                y2 += rect.height / 4;
                break;
            default:
                break;
            }
        }

        //ǹ���ƶ�����Ա����ָ��λ�ú��Զ���ǹ
        //������ǹ��������Ƿ�����׼��Χ�ļ�����΢�ϸ�Щ
        //ʵ�ʲ��ԣ���׼�Ϳ�ǹЧ������Щ
        if ((abs(x2 - x1) < rect.width * 3/7) && (abs(y2 - y1) < rect.height / 3)) {
            ret = true;
        }
    }

    return ret;
}

//�Զ�����
void MouseKeyboard::AutoFire(DETECTRESULTS detectResult, WEAPONINFO weaponInfo) {

    MouseLBClick();
    //�Զ�������Զ�ѹǹ
    /*
    if (m_AssistConfig->autoPush) {
        MouseMove(0, 2);
    }
    Sleep(10);
    */

    return;
}

//�Զ��ƶ����
void MouseKeyboard::AutoMove(DETECTRESULTS detectResult, WEAPONINFO weaponInfo) {
    
    if (detectResult.maxPersonConfidencePos >= 0 && detectResult.boxes.size() > 0) {
        //ʹ�ü���õ���Ϸ��Ļ��������
        LONG x1 = m_AssistConfig->detectCentX;
        LONG y1 = m_AssistConfig->detectCentY;

        //������Ա���������꣬�������Ŀ��ϵ�λ�ã�������ͷ
        Rect rect = detectResult.boxes.at(detectResult.maxPersonConfidencePos);
        LONG x2 = m_AssistConfig->detectRect.x + rect.x + rect.width/2;
        LONG y2 = m_AssistConfig->detectRect.y + rect.y + rect.height/3;

        //���������ѹǹ����Ա�������������ƶ�
        if (m_AssistConfig->autoPush) {
            //ֻ��1��2����ѹǹ
            switch (weaponInfo.bag)
            {
            case 1:
                //����1������������
                y2 += rect.height / 5;
                break;
            case 2:
                //����2��4��������
                y2 += rect.height / 4;
                break;
            default:
                break;
            }
        }

        //������3D��Ϸ��λ����3ά���꣬����ԽԶ���ƶ�����Ҫ�˵�ϵ����Խ��
        //��ʱû�кõķ���ͨ��ͼƬ������3ά���꣬��ʹ�ö���Ĵ�С���Լ���z���꣬���ǿ�����Ĵ�С��ʱ�޷�����
        //Ϊ�˴���̫ԶͼƬ�����⣬�ڰ�����log����һ��������ʵ��λ��ԽԶ��������̫���Ч����
        //��������������ƶ����ʱ��Ҫ����������Ϊ3ά�����У��ƶ��������ž���Ŵ�
        double z = 1;
        if (m_AssistConfig->maxModelWidth > 0 && m_AssistConfig->maxModelWidth  > rect.width)
        {
            //log����ȡֵ�������ƶ�����ƫ�࣬�ĳ����Լ���
            //z = log2(m_AssistConfig->maxModelWidth) / (rect.width);
            z = m_AssistConfig->maxModelWidth / (rect.width);
        }
        

        //�ƶ����
        //3D��Ϸ�ƶ����ĺ�����x1/y1Ϊ��Ϸ���ĵ����꣬x2/y2Ϊ��⵽���������ĵ����꣬zΪ��ά�����z�����
        //mouseMoveSlow�������ı���   
        MouseMove(x1, y1, x2, y2, z, m_AssistConfig->mouseMoveSlow);
    }

    return;
}


//�Զ�ѹǹ,pushCount�����Ǳ���������Ѿ�ѹǹ�Ĵ���
void MouseKeyboard::AutoPush(WEAPONINFO weaponInfo) {
     
    //ֻ��1��2����ѹǹ
    switch (weaponInfo.bag)
    {
    case 1:
        //����1������������
        MouseMove(0, m_AssistConfig->pushHeight[0]);

        Sleep(10);
        //timingus(1);

        break;
    case 2:
        //����2��4��������
        MouseMove(0, m_AssistConfig->pushHeight[1]);

        Sleep(10);
        //timingus(1);

        break;
    default:
        Sleep(100);
        break;
    }
    
    return;
}

//�Զ���ǹ����Զ�ѹǹ,������Ϸ��ǹ�ٶ����ƣ����������������û��ʵ�ʿ�ǹ�����Դ����߼����Զ�ѹǹ��̫һ��
void MouseKeyboard::AutoPushAfterFire(WEAPONINFO weaponInfo) {

    //ֻ��1��2����ѹǹ
    switch (weaponInfo.bag)
    {
    case 1:
        //����1������������
        MouseMove(0, m_AssistConfig->pushHeight[0]);

        //Sleep(10);
        //timingus(1);

        break;
    case 2:
        //����2��4��������
        MouseMove(0, m_AssistConfig->pushHeight[1]);

        //Sleep(10);
        //timingus(1);

        break;
    default:
        //Sleep(100);
        break;
    }

    return;
}