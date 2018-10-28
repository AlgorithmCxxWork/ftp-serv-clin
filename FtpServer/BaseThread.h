
#ifndef BASE_THREAD_INCLUDE_H
#define BASE_THREAD_INCLUDE_H

#include <windows.h>
#include "StringUtil.h"

#define WM_RUNTHREAD		WM_USER+0x100
#define WM_COMPLETETHREAD	WM_USER+0x101

unsigned int __stdcall ThreadFunc(LPVOID lParam);
typedef struct
{
	CStringUtil  m_threadName;//�߳�����
	HWND    m_notifyWnd;//�߳�������ֹͣ����Ϣ֪ͨ�ĸ�����
	HANDLE	m_threadHandle;//�̵߳ľ��
	UINT32	m_threadId;//�߳�ID
}Thread_s;

class CBaseThread
{
public:
	CBaseThread() :m_thread(NULL), m_runningFlag(1) {}
	~CBaseThread() { delete m_thread; }
	/**
	*
	*  �����߳�
	*  
	*
	**/
	virtual int  CreateThread(CStringUtil name = TEXT(""));

	/**
	*
	* �߳��������������ִ�гɹ�����1�����򷵻�0
	*
	**/
	virtual int StartThread();

	/**
	*
	* �߳����к���
	*
	**/
	virtual	int RunThread() = 0;

	/***
	*
	* �߳�ֹͣ���������ִ�гɹ�����1�����򷵻�0
	*
	*
	**/
	virtual int StopThread();

	/**
	*
	* �����߳���Ϣ֪ͨ�Ĵ���
	*
	*/
	void   SetNotifyWnd(HWND hwnd);

	virtual void CompleteThread() {}

	void   SetRunningFlage(BOOL prmFlage) { m_runningFlag = prmFlage; }

	Thread_s	*GetThreadStruct() { return m_thread; }

	int		WaitThread();
		
protected:
	/**
	*
	* �ú��������Զ���ֹͣ�̵߳ķ��������鲻Ҫʹ��TerminateThread
	*	��ExitThread��ʵ�֣�������ѭ����������һ�������ж��Ƿ�������̺���
	*	����������Run�����У�ʹ��m_runningFlag��Ϊѭ�����ж�����֮һ����m_runningFlagΪ1ʱ
	*	�̺߳�����һֱ���У���m_runningFlagΪ0ʱ���߳����н���
	*
	**/
	virtual int Stop() { m_runningFlag = 0; return 0; };
	Thread_s  *m_thread;
	UINT32	 m_runningFlag;
};

#endif