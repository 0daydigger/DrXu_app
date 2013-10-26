
// DrXu_AppDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DrXu_App.h"
#include "DrXu_AppDlg.h"
#include "afxdialogex.h"
#include <WinSvc.h>


#ifndef _extra_
#define _extra_
#include "extra.h"
#endif 



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/* ����������Ҫ�Ľṹ */
typedef struct _UNICODE_STRING{
    USHORT Length;
    USHORT MaximumLength;
    PVOID Buffer;
}UNICODE_STRING,*PUNICODE_STRING;

typedef struct _SYSTEM_LOAD_AND_CALL_IMAGE{
    UNICODE_STRING ModuleName;
}SYSTEM_LOAD_AND_CALL_IMAGE,*PSYSTEM_LOAD_AND_CALL_IMAGE;

typedef DWORD (CALLBACK* ZWSETSYSTEMINFORMATION)(DWORD,PVOID,LONG);
ZWSETSYSTEMINFORMATION ZwSetSystemInformation;
typedef DWORD (CALLBACK* RTLANSISTRINGTOUNICODESTRING)(PVOID,PVOID,DWORD);
RTLANSISTRINGTOUNICODESTRING RtlAnsiStringToUnicodeString;
typedef VOID (CALLBACK* RTLINITUNICODESTRING)(PUNICODE_STRING,PCWSTR);
RTLINITUNICODESTRING RtlInitUnicodeString;

SYSTEM_LOAD_AND_CALL_IMAGE gregsImage;
/* �ṹ������� */

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDrXu_AppDlg �Ի���




CDrXu_AppDlg::CDrXu_AppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDrXu_AppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDrXu_AppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDrXu_AppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CDrXu_AppDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDrXu_AppDlg ��Ϣ�������

BOOL CDrXu_AppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// ������������
	// �ӵ�ǰĿ¼�¼�����������
	if ( LoadDriverFromFile("DrXu.sys" ) )
	{
		//���豸���
		this->g_hDrXuDevice = CreateFile(SYMBOLIC_LINK_DRXU,
										  GENERIC_ALL,//GENERIC_READ | GENERIC_WRITE,
										  0,
										  NULL,
										  OPEN_EXISTING,
										  FILE_ATTRIBUTE_NORMAL,
										  NULL);
		if ( this->g_hDrXuDevice != INVALID_HANDLE_VALUE )
		{
			::AfxMessageBox(L"�������سɹ�");
			TRACE("[CreateFile] Success,g_hDrXuDevice is %d",this->g_hDrXuDevice);
		}
		else
		{
			::AfxMessageBox(L"�޷������豸����");
			TRACE("[CreateFile] Faild in [InitDialog],Last Error:%d",GetLastError());
			TRACE("[CreateFile] Faild in [InitDialog],g_hDrXuDevice:%d",this->g_hDrXuDevice);
			
			if ( this->StopService() )
				::AfxMessageBox(L"��������ֹͣ");
			ExitProcess(-1);
		}
	}
	else
	{
		::AfxMessageBox(L"��������ʧ��");
		ExitProcess(-1);
	}
	// �ر������豸����
	
	// CloseHandle(this->g_hDrXuDevice); 2013.10.26 0:26 ò�����и�bug

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_MAXIMIZE);


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDrXu_AppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDrXu_AppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDrXu_AppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// ���������ļ�
bool CDrXu_AppDlg::LoadDriverFromFile(char* sysFileName)
{
    char sysPath[1024] = {0};
    char aCurrentDirectory[515] = {0};

    ///�򿪷��������
    SC_HANDLE sh = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(!sh)
    {
     //   printf("Faild to Open Service Manager!\n");
     //   printf("GetLastError:%d\n",GetLastError());
        return false;
    }
    ///��ȡ��ǰexe����·��
    GetCurrentDirectoryA(512,aCurrentDirectory);
    _snprintf(sysPath,1022,"%s\\%s",aCurrentDirectory,sysFileName);
    TRACE("path is:%s\n",sysPath);
    SC_HANDLE rh = CreateServiceA(sh,
                                 DRXU_SERVICE_NAME,
                                 DRXU_SERVICE_NAME,
                                 SERVICE_ALL_ACCESS,
                                 SERVICE_KERNEL_DRIVER,
                                 SERVICE_DEMAND_START,
                                 SERVICE_ERROR_NORMAL,
                                 sysPath,
                                 NULL,NULL,NULL,NULL,NULL
                                 );
    if(!rh)
    {
        TRACE("�Ҳ���RC���\n");
        if(GetLastError()== ERROR_SERVICE_EXISTS)
        {
            rh = OpenServiceA(sh,DRXU_SERVICE_NAME,SERVICE_ALL_ACCESS);
            if(!rh)
            {
                CloseServiceHandle(sh);
                TRACE("���ڷ��񣬵����޷���\n");
                return false;
            }
        }
        else
        {
            TRACE("�޷���������!\n");
            TRACE("GetLastError:%d\n",GetLastError());
            CloseServiceHandle(sh);
            return false;
        }
    }
    //��������
    if(rh)
    {
	  TRACE("[LoadDriverFromFile]:rh %d\n",rh);
	  TRACE("[LoadDriverFromFile] GetLastError:%d\n",GetLastError());
      if(StartService(rh,0,NULL) == 0)
      {
         if(GetLastError()== ERROR_SERVICE_ALREADY_RUNNING)
         {
              return true;
         }
         else
         {
              TRACE("[LoadDriverFromFile]:�޷���������\n");
              TRACE("GetLastError:%d\n",GetLastError());
              CloseServiceHandle(sh);
              CloseServiceHandle(rh);
              return false;
         }
       }
	
    CloseServiceHandle(rh);
    CloseServiceHandle(sh);
    return true;
    }
	return false;
}


BOOL CDrXu_AppDlg::DestroyWindow()
{
	if ( this->StopService() )
	{
		AfxMessageBox(L"[DestroyWindow]�ɹ�ж������");
	}
	else
	{
		AfxMessageBox(L"[DestroyWindow]ж������ʧ�� :(");
	}
	return CDialogEx::DestroyWindow();
}


// ֹͣ����ж������
bool CDrXu_AppDlg::StopService(void)
{ 
	// �ر��ں��豸
	CloseHandle(this->g_hDrXuDevice);
	TRACE("[StopService]:�Ѿ�ж���豸���g_hDrXuDevice");
	// �򿪷���������
    SC_HANDLE hSC = ::OpenSCManager( NULL, 
                        NULL, GENERIC_EXECUTE);
    if( hSC == NULL)
    {
        TRACE( "[StopService]:��SCManagerʧ��");
        return false;
    }
    // �򿪷���
    SC_HANDLE hSvc = ::OpenServiceA( hSC, DRXU_SERVICE_NAME,
        SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
    if( hSvc == NULL)
    {
        TRACE( "[StopService]:�򿪷����쳣��");
        ::CloseServiceHandle( hSC);
        return false;
    }
    // ��÷����״̬
    SERVICE_STATUS status;
    if( ::QueryServiceStatus( hSvc, &status) == FALSE)
    {
        TRACE( "[StopService]:�޷���÷���״̬");
        ::CloseServiceHandle( hSvc);
        ::CloseServiceHandle( hSC);
        return false;
    }
    //�����������״̬��ֹͣ
    if( status.dwCurrentState == SERVICE_RUNNING)
    {
        // ֹͣ����
        if( ::ControlService( hSvc, 
          SERVICE_CONTROL_STOP, &status) == FALSE)
        {
            TRACE( "[StopService]ֹͣ����ʧ��");
            ::CloseServiceHandle( hSvc);
            ::CloseServiceHandle( hSC);
            return false;
        }
        // �ȴ�����ֹͣ
        while( ::QueryServiceStatus( hSvc, &status) == TRUE)
        {
            ::Sleep( status.dwWaitHint);
            if( status.dwCurrentState == SERVICE_STOPPED)
            {
               // AfxMessageBox( "stop success��");
                ::CloseServiceHandle( hSvc);
                ::CloseServiceHandle( hSC);
                return true;
            }
        }
		return false;
    }
	else
	{
		TRACE("[StopService]����δ����,status.dwCurrentState:%d",status.dwCurrentState);
	    ::CloseServiceHandle( hSvc);
	    ::CloseServiceHandle( hSC);
		return true;
	}
}


void CDrXu_AppDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UCHAR InputBuffer[10];
    UCHAR OutputBuffer[10];
    DWORD dwOutput;
    
	::AfxMessageBox(L"Entered");
    memset(InputBuffer, 0xAA, 10);
	BOOL bRet = ::DeviceIoControl(this->g_hDrXuDevice, IOCTL_TEST, InputBuffer, 10, &OutputBuffer, 10, &dwOutput, NULL);
    if (bRet)
    {
        TRACE("Output buffer:%d bytes\n",dwOutput);
        for (int i=0;i<(int)dwOutput;i++)
        {
            TRACE("%02X ",OutputBuffer[i]);
        }
        TRACE("\n");
    }
	else
	{
		TRACE("[DeviceIoControl]bRet faild, value is %d, ",bRet);
		TRACE("[DeviceIoControl]GetLastError() is %d",GetLastError());
		TRACE("g_hDrXuDevice value is %d",this->g_hDrXuDevice);
	}
	//CDialogEx::OnOK();
}
