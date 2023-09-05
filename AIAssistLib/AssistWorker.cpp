#include "pch.h"
#include "AssistWorker.h"


//��ʼ����̬��Ա����
AssistConfig* AssistWorker::m_AssistConfig = AssistConfig::GetInstance();
std::condition_variable AssistWorker::m_pushCondition = std::condition_variable();
std::atomic_bool AssistWorker::m_startPush = false;   ///�Ƿ�����ѹǹ������־
std::atomic_int AssistWorker::m_pushCount = 0;
WEAPONINFO AssistWorker::m_weaponInfo = { 1,1,1 };

std::atomic_bool AssistWorker::m_startFire = false;   ///�Ƿ����ڿ�ǹ�����������˹���ǹʱ��ִ���Զ���ǹ����



//������깳�Ӻ���
LRESULT CALLBACK MouseHookProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam;
    BOOLEAN injected = p->flags & LLMHF_INJECTED || p->flags & LLMHF_LOWER_IL_INJECTED; // Checks if click was injected and not from a mouse
    if (nCode == HC_ACTION && !injected)
    {
        if (wParam == WM_LBUTTONDOWN) {
            //�������ڿ�ǹ��־
            AssistWorker::m_startFire = true;

            //�ж��û��Ƿ��������Զ�ѹǹ
            if (AssistWorker::m_AssistConfig->autoPush) {
                //��ʼѹǹ
                AssistWorker::m_startPush = true;
                AssistWorker::m_pushCount = 0;
                AssistWorker::m_pushCondition.notify_all();       
            }
        }
        else if (wParam == WM_LBUTTONUP) {
            //�������ڿ�ǹ��־
            AssistWorker::m_startFire = false;

            //�ж��û��Ƿ��������Զ�ѹǹ
            if (AssistWorker::m_AssistConfig->autoPush) {
                //������̧������ѹǹ
                AssistWorker::m_startPush = false;
                AssistWorker::m_pushCount = 0;
                AssistWorker::m_pushCondition.notify_all();
            }
        }
        else if (wParam == WM_RBUTTONDOWN) {
        }
        else if (wParam == WM_RBUTTONUP) {
        }
        else if (wParam == WM_MBUTTONDOWN) {
        }
        else if (wParam == WM_MBUTTONUP) {
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

//������̹��Ӻ���
LRESULT CALLBACK KeyboardHookProcedure(int nCode, WPARAM wParam, LPARAM lParam)
{
    // WH_KEYBOARD_LL uses the LowLevelKeyboardProc Call Back
    // wParam and lParam parameters contain information about the message.
    auto* p = (KBDLLHOOKSTRUCT*)lParam;
    if (nCode == HC_ACTION)
    {
        if (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN)
        {
            //����Ƿ��л�����
            switch (p->vkCode) {
                case 49: 
                    AssistWorker::m_weaponInfo.bag = 1;
                    break;
                case 50:
                    AssistWorker::m_weaponInfo.bag = 2;
                    break;
                case 51:
                    AssistWorker::m_weaponInfo.bag = 3;
                    break;
                case 52:
                    AssistWorker::m_weaponInfo.bag = 4;
                    break;
                case 53:
                    AssistWorker::m_weaponInfo.bag = 5;
                    break;

                case VK_NUMPAD1:
                    MouseKeyboard::m_AssistConfig->maxJuPushCount = 6;
                    break;
                case VK_NUMPAD2:
                    MouseKeyboard::m_AssistConfig->maxJuPushCount = 12;
                    break;
                case VK_NUMPAD3:
                    MouseKeyboard::m_AssistConfig->maxJuPushCount = 18;
                    break;
                case VK_NUMPAD0:
                    MouseKeyboard::m_AssistConfig->maxJuPushCount = MouseKeyboard::m_AssistConfig->maxBuPushCount;
                    break;

                case 0x56:
                    //ʹ��v���ر�/�����Զ�׷�١�����ѹǹ
                    if (AssistWorker::m_AssistConfig->autoTrace || AssistWorker::m_AssistConfig->autoFire || AssistWorker::m_AssistConfig->autoPush) {
                        AssistWorker::m_AssistConfig->autoTrace = false;
                        AssistWorker::m_AssistConfig->autoFire = false;
                        AssistWorker::m_AssistConfig->autoPush = false;
                    }
                    else {
                        //�ָ��û����õ�ֵ
                        AssistWorker::m_AssistConfig->autoTrace = AssistWorker::m_AssistConfig->autoTraceUserSet;
                        AssistWorker::m_AssistConfig->autoFire = AssistWorker::m_AssistConfig->autoFireUserSet;
                        AssistWorker::m_AssistConfig->autoPush = AssistWorker::m_AssistConfig->autoPushUserSet;

                    }
                    break;

                case VK_RETURN:
                    //ʹ��enter���ر��Զ�׷�١�����ѹǹ
                    if (AssistWorker::m_AssistConfig->autoTrace || AssistWorker::m_AssistConfig->autoFire || AssistWorker::m_AssistConfig->autoPush) {

                        AssistWorker::m_AssistConfig->autoTrace = false;
                        AssistWorker::m_AssistConfig->autoFire = false;
                        AssistWorker::m_AssistConfig->autoPush = false;

                    }
                   
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}


AssistWorker::AssistWorker()
{
    //�����¼����������������
    m_AssistConfig->ReCalDetectionRect();

    //��������
    //���������г�������Ϊ1��Ŀ����ֻ�������¼������
    fireQueue = new BlockQueue<DETECTRESULTS>(1);
    moveQueue = new BlockQueue<DETECTRESULTS>(1);

    drawQueue = new BlockQueue<DRAWRESULTS>(10);
    outDrawQueue = new BlockQueue<Mat>(10);

    //���������߳�
    //ͼƬ����߳�
    detectThread = new thread(std::bind(&AssistWorker::DetectWork, this));
    detectThread->detach();
    //�Զ������߳�
    fireThread = new thread(std::bind(&AssistWorker::FireWork, this));
    fireThread->detach();
    //�Զ���׼
    moveThread = new thread(std::bind(&AssistWorker::MoveWork, this));
    moveThread->detach();
    //��ͼ�߳�
    drawThread = new thread(std::bind(&AssistWorker::DrawWork, this));
    drawThread->detach();

    //drawAimThread = new thread(std::bind(&AssistWorker::DrawAimWork, this));
    //drawAimThread->detach();
    
    //������hook�߳�,
    mouseKeyboardHookThread = new thread(std::bind(&AssistWorker::MouseKeyboardHookWork, this));
    mouseKeyboardHookThread->detach();
    
    //ѹǹ�߳�
    pushThread = new thread(std::bind(&AssistWorker::PushWork, this));
    pushThread->detach();

    //����ͼƬ��������������
    //imageDetection = new ImageDetection();
    //imageDetection = new ImageDetectionTensorflow();
    imageDetection = NULL;
    mouseKeyboard = new MouseKeyboard();
    drawImage = new DrawImage();

    srand(time(nullptr)); // �õ�ǰʱ����Ϊ����

    return;
}

AssistWorker::~AssistWorker()
{
    m_stopFlag = true;
    
    if (imageDetection != NULL)
        delete imageDetection;
    if (mouseKeyboard != NULL)
        delete mouseKeyboard;
    if (drawImage != NULL)
        delete drawImage;

    if (drawQueue != NULL)
        delete drawQueue;
    if (fireQueue != NULL)
        delete fireQueue;
    if (moveQueue != NULL)
        delete moveQueue;

    if (detectThread != NULL)
        delete detectThread;
    if (fireThread != NULL)
        delete fireThread;
    if (moveThread != NULL)
        delete moveThread;
    if (drawThread != NULL)
        delete drawThread;
    if (drawAimThread != NULL)
        delete drawAimThread;
    if (mouseKeyboardHookThread != NULL)
        delete mouseKeyboardHookThread;
    if (pushThread != NULL)
        delete pushThread;

    if (m_mouseHook) { 
        UnhookWindowsHookEx(m_mouseHook); 
        m_mouseHook = NULL;
    }
    if (m_keyboardHook) { 
        UnhookWindowsHookEx(m_keyboardHook); 
        m_keyboardHook = NULL;
    }

    return;
}

//�޸����ú���Ҫ���³�ʼ��һЩ����
void AssistWorker::ReInit() {
    
    //��ֹͣ���й����߳�
    Pause();
    Sleep(1000);

    //�����¼����������������
    m_AssistConfig->ReCalDetectionRect();

    //������ж���
    drawQueue->Clear();
    outDrawQueue->Clear();
    fireQueue->Clear();
    moveQueue->Clear();

    //�ؽ���Ҫ�ؽ��Ķ���
    if (m_AssistConfig->detectImg) {
        if (imageDetection != NULL) {
            imageDetection->ReInit();
        }
        else {
            //�½�����
            imageDetection = new ImageDetectionOnnx();
        }
    }

    //�����̱߳�־���������߳�
    Start();

    return;
}

void AssistWorker::DetectWork()
{
    while (!m_stopFlag)
    {
        if (m_detectPauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_detectMutex);
            while (m_detectPauseFlag)
            {
                m_detectCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else if(imageDetection != NULL){
            //ͼ����
            double duration;
            clock_t start, finish;
            start = clock();

            //��Ļ����ͼ����
            imageDetection->getScreenshot();
            DETECTRESULTS detectResult = imageDetection->detectImg();

            finish = clock();
            duration = (double)(finish - start) * 1000 / CLOCKS_PER_SEC;

            //��¼ͼ�������
            AssistWorker::m_AssistConfig->detectCount += 1;
            if (AssistWorker::m_AssistConfig->detectCount >= INT_MAX) {
                AssistWorker::m_AssistConfig->detectCount = 0;
            }

            if (detectResult.classIds.size() > 0) {
                //�м�鵽���࣬����ŵ�������,��֪ͨ�����߳����Ѽ����
                //����ѿ�ǹ������ƶ��������ڲ�ͬ�̣߳��ᵼ�²������ѣ������������ȷŻ�ͬһ���̴߳���
                //fireQueue->PushBackForce(detectResult);
                moveQueue->PushBackForce(detectResult);
            }

            //Ȼ����mat��������ǰ����ʾ
            Mat mat = imageDetection->getImg();

            DRAWRESULTS  drawResult{detectResult, mat, duration};
            bool push = drawQueue->PushBack(drawResult);

            //���������û�����ͳɹ������ֹ��ͷ�clone��mat����
            if (!push) {
                mat.release();
                mat = NULL;
            }
            
        }
        else {
            Sleep(500);
        }
    }

    return;
}

void AssistWorker::FireWork()
{
    while (!m_stopFlag)
    {
        if (m_firePauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_fireMutex);
            while (m_firePauseFlag)
            {
                m_fireCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            
            //��ȡ�����д�ŵļ����
            DETECTRESULTS detectResult;
            bool ret = fireQueue->PopFront(detectResult);
            if (ret) {
                //ִ���Զ���ǹ����
                //�ȼ���Ƿ��������Զ���ǹ��־
                if (m_AssistConfig->autoFire && !AssistWorker::m_startFire) {
                    //�ڼ���Ƿ��Ѿ���׼��
                    bool isInTarget = mouseKeyboard->IsInTarget(detectResult, m_weaponInfo);
                    //����Ѿ���׼��ִ���Զ���ǹ����
                    if (isInTarget) {
                        //��ǹ������ƶ��������ڲ�ͬ�̣߳����²������ѣ��ȷŻ�ͬһ���̴߳���
                        //mouseKeyboard->AutoFire(detectResult);
                    }
                }
            }
            
        }
    }

    return;
}

void AssistWorker::MoveWork()
{
    while (!m_stopFlag)
    {
        if (m_movePauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_moveMutex);
            while (m_movePauseFlag)
            {
                m_moveCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //��ȡ�����д�ŵļ����
            DETECTRESULTS detectResult;
            bool ret = moveQueue->PopFront(detectResult);
            if (ret) {
                //ִ��������
                //std::cout << to_string(detectResult.classIds.size());
                //�ȼ���Ƿ��������Զ�׷��
                //����������ֻ��ʹ�ñ���1��2(ʹ�ò�ǹ�;ѻ�ǹ��ʱ��)���Ž���׷�٣�ʹ������������׷��
                if (m_AssistConfig->autoTrace && (m_weaponInfo.bag==1 || m_weaponInfo.bag == 2)) {
                    //�ڼ���Ƿ��Ѿ���׼��
                    bool isInTarget = mouseKeyboard->IsInTarget(detectResult, m_weaponInfo);
                    //û����׼������£���ִ�����׷�ٲ���
                    if (isInTarget) {
                        //��ǹ������ƶ��������ڲ�ͬ�̣߳����²������ѣ��ȷŻ�ͬһ���̴߳���
                        //����һ��������û���˹�����������������£���ִ���Զ���ǹ
                        if (m_AssistConfig->autoFire && !AssistWorker::m_startFire) {

                            mouseKeyboard->AutoFire(detectResult, m_weaponInfo);

                            //�Զ�������Զ�ѹǹ����ͼ�������������ʱ��
                            if (m_AssistConfig->autoPush) {
                                //���100msһǹ�����һ��20ms�����Դ����Ǽ��5������ѹһ��ǹ��
                                if ((m_AssistConfig->detectCount - m_AssistConfig->preDetectCount) % 5 == 0) {
                                    m_AssistConfig->preDetectCount = m_AssistConfig->detectCount;
                                    mouseKeyboard->AutoPushAfterFire(m_weaponInfo);
                                }
                            }

                            //����û���ϸ�Ĳ������ƣ��Զ�������ֶ�������ʱ���ͻ
                            //�Զ��������һ�������������⵽�ֶ������־���������������ѹ
                            if (AssistWorker::m_startFire) {
                                mouseKeyboard->MouseLBDown();
                            }
                        }
                    }
                    else {
                        mouseKeyboard->AutoMove(detectResult, m_weaponInfo);
                    }
                }
            }
        }
    }

    return;
}

void AssistWorker::DrawWork()
{
    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    //cv::startWindowThread();

    while (!m_stopFlag)
    {
        if (m_drawPauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_drawMutex);
            while (m_drawPauseFlag)
            {
                m_drawCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //��ȡ�����д�ŵļ����
            DrawResults drawResult;
            bool ret = drawQueue->PopFront(drawResult);
            if (ret) {
                //ִ�л�ͼ����
                DETECTRESULTS out = drawResult.out;
                Mat mat = drawResult.mat;
                double duration = drawResult.duration;
                if (!mat.empty()) {
                    string times = format("%.2f ms", duration);
                    putText(mat, times, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 250), 1);

                    //ע����Ϸ��Ļ���ģ��ͼ�����������λ�ò�һ�������������������΢����1/10
                    Rect center = { mat.cols/2 -5,mat.rows / 2 + mat.rows /10 - 5,10,10 };
                    rectangle(mat, center, Scalar(0, 0, 250), 2);

                    //����ѡ����Ŀ��
                    if (out.classIds.size() > 0) {
                        Rect rect = out.boxes[out.maxPersonConfidencePos];
                        rectangle(mat, { rect.x + rect.width/2 -4, rect.y + rect.height / 2 - 4, 8, 8 }, Scalar(255, 178, 50), 2);
                    }

                    for (int i = 0; i < out.classIds.size(); i++) {
                        rectangle(mat, out.boxes[i], Scalar(255, 178, 50), 2);

                        //Get the label for the class name and its confidence
                        string label = format("%.2f", out.confidences[i]);
                        //label = m_classLabels[classIds[i]-1] + ":" + label;
                        //label = "" + to_string(out.classIds[i]) + ", " + label;        
                        label = "" + label + "," + format("%.2f", out.xvals[i]);

                        //Display the label at the top of the bounding box
                        int baseLine;
                        Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 2, &baseLine);
                        int top = max(out.boxes[i].y, labelSize.height);
                        putText(mat, label, Point(out.boxes[i].x, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 250), 1);
                    }
                    
                    //�Ѵ�����õ�mat��������ⲿʹ�õĶ�����
                    bool push = outDrawQueue->PushBack(mat);
                    //�����������ʧ�ܣ��ֹ�����clone��mat����
                    if (!push) {
                        mat.release();
                        mat = NULL;
                    }
                   
                    //cv::imshow("opencvwindows", mat);
                    //waitKey(10);
                }
            }
        }
    }

    return;
}

void AssistWorker::DrawAimWork()
{
    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    //cv::startWindowThread();

    while (!m_stopFlag)
    {
        if (m_drawAimPauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_drawAimMutex);
            while (m_drawAimPauseFlag)
            {
                m_drawAimCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //��ȡ�����д�ŵļ����
            if (m_AssistConfig->drawAim) {
                //����׼��
                drawImage->drawAim();
                Sleep(10);
            }
            else {
                Sleep(500);
            }
        }
    }

    return;
}

void AssistWorker::MouseKeyboardHookWork()
{
    //�����߳�ʱ��ȡwindows���߳�id
    m_hookThreadId = GetCurrentThreadId();
    while (!m_stopFlag)
    {
        //��Щ��Ϸ������ʱ������hook����������ͨ��m_hookPauseFlag��־����hook�������¹���
        if (!m_hookPauseFlag) {
            //����hook���ĸ��̹߳��ص�hook�������Ǹ��߳���ִ�лص�����
            if (!m_mouseHook) {
                m_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProcedure, GetModuleHandle(nullptr), NULL);
            }
            if (!m_keyboardHook) {
                m_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProcedure, GetModuleHandle(nullptr), NULL);
            }

            //����GetMessage����غ���ʱ���Զ�������Ϣ����
            MSG Msg;
            while (!m_stopFlag && GetMessage(&Msg, nullptr, 0, 0) > 0)
            {
                //ͨ��������߳���Ϣ���з���WM_AIASSISTPAUSE��Ϣ�������˳���Ϣ����ѭ��
                if (Msg.message == WM_AIASSISTPAUSE) {
                    break;
                }
                TranslateMessage(&Msg);
                DispatchMessage(&Msg);
            }

            if (m_mouseHook) {
                UnhookWindowsHookEx(m_mouseHook);
                m_mouseHook = NULL;
            }
            if (m_keyboardHook) {
                UnhookWindowsHookEx(m_keyboardHook);
                m_keyboardHook = NULL;
            }
        }
        else {
            Sleep(500);
        }
    }
    //�˳��߳�ʱ����windows���߳�idΪ0
    m_hookThreadId = 0;

    return;
}


void AssistWorker::PushWork()
{
    int randomMax1 = m_AssistConfig->maxJuPushCount;//����1ѹǹ����������ֵ��ȱʡ�ž�
    int randomMax2 = m_AssistConfig->maxBuPushCount;//����2ѹǹ����������ֵ��ȱʡ�Ų�ǹ

    while (!m_stopFlag)
    {
        if (m_pushPauseFlag || !m_startPush)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_pushMutex);
            while (m_pushPauseFlag || !m_startPush)
            {
                m_pushCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();

            //ÿ�δ���ѹǹǰ������һ�������
            randomMax1 = m_AssistConfig->maxJuPushCount;//����1ѹǹ����������ֵ��ȱʡ�ž�

            //int min = m_AssistConfig->maxBuPushCount, max = m_AssistConfig->maxBuPushCount * 2;
            //randomMax2 = (rand() % (max - min)) + min;//����2ѹǹ����������ֵ��ȱʡ�Ų�ǹ
            randomMax2 = m_AssistConfig->maxBuPushCount;//����2ѹǹ����������ֵ��ȱʡ�Ų�ǹ
        }
        else {

            //���ѹǹ����������ֵ���������������
             //ֻ��1��2��������
            if ((m_weaponInfo.bag == 1 && m_pushCount > randomMax1) || (m_weaponInfo.bag == 2 && m_pushCount > randomMax2)) {
                m_pushCount = 0;
               
                mouseKeyboard->MouseLBUp();
                mouseKeyboard->AutoPush(m_weaponInfo);
                Sleep(150);
                if(m_startPush)
                    mouseKeyboard->MouseLBDown();
            }
            else {
                //ִ��ѹǹ����
                mouseKeyboard->AutoPush(m_weaponInfo);
                m_pushCount++;
            }
        }
    }

    return;
}

Mat AssistWorker::PopDrawMat() {
    Mat mat;
    outDrawQueue->PopFront(mat);

    //ע�ⷵ�ؿ�¡���󣬲��ܰ�ȫ�ش���ͼ������
    Mat mat2 = mat.clone();

    //�ͷ���mat
    mat.release();
    mat = NULL;

    return mat2;
}

void AssistWorker::Start()
{
    m_detectPauseFlag = false;
    m_detectCondition.notify_all();
    
    m_firePauseFlag = false;
    m_fireCondition.notify_all();

    m_movePauseFlag = false;
    m_moveCondition.notify_all();

    m_drawPauseFlag = false;
    m_drawCondition.notify_all();

    m_drawAimPauseFlag = false;
    m_drawAimCondition.notify_all();

    m_pushPauseFlag = false;
    m_pushCondition.notify_all();

    m_hookPauseFlag = false;

    return;
}

void AssistWorker::Pause()
{
    m_detectPauseFlag = true;
    m_detectCondition.notify_all();

    m_firePauseFlag = true;
    m_fireCondition.notify_all();

    m_movePauseFlag = true;
    m_moveCondition.notify_all();

    m_drawPauseFlag = true;
    m_drawCondition.notify_all();

    m_drawAimPauseFlag = true;
    m_drawAimCondition.notify_all();

    m_pushPauseFlag = true;
    m_pushCondition.notify_all();

    m_hookPauseFlag = true;
    if (mouseKeyboardHookThread != NULL && m_hookThreadId>0) {
        //��hook�̷߳����˳���Ϣ
        PostThreadMessage(m_hookThreadId, WM_AIASSISTPAUSE, 0, 0);
    }
    return;
}
