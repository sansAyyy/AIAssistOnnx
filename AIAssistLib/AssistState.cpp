#include "pch.h"
#include "AssistState.h"


//��ʼ����̬��Ա����
AssistConfig* AssistState::m_AssistConfig = AssistConfig::GetInstance();

string AssistState::getStatInf() {
	char info[1024];

	memset(info, 0, sizeof(info));

	string fmt("��Ϸ��%s��\r\n��Ϸ���ڣ�%d,%d,%d,%d��\r\n�������%d,%d,%d,%d��\r\n�Զ�׷�٣�%s��\r\n�Զ�����%s��\r\nע�⣺v��������м����Թرջ�ָ��Զ�׷��/����/ѹǹ��\r\n���ش���ʱ��enter���ر����в�����\r\n���飺ÿ�ν���ǰ��v���л���ͼ��");

	snprintf(info, sizeof(info), fmt.c_str(), m_AssistConfig->gameName,
		m_AssistConfig->screenRect.x, m_AssistConfig->screenRect.y, m_AssistConfig->screenRect.width, m_AssistConfig->screenRect.height,
		m_AssistConfig->detectRect.x, m_AssistConfig->detectRect.y, m_AssistConfig->detectRect.width, m_AssistConfig->detectRect.height, 
		m_AssistConfig->autoTrace ? "true" : "false", m_AssistConfig->autoFire ? "true" : "false", 1024);

	string ret(info);

	return ret;
}