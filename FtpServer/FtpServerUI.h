
// FtpServerUI.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFtpServerUIApp:
// �йش����ʵ�֣������ FtpServerUI.cpp
//

class CFtpServerUIApp : public CWinApp
{
public:
	CFtpServerUIApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFtpServerUIApp theApp;