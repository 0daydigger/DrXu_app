
// DrXu_App.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDrXu_AppApp:
// �йش����ʵ�֣������ DrXu_App.cpp
//

class CDrXu_AppApp : public CWinApp
{
public:
	CDrXu_AppApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDrXu_AppApp theApp;