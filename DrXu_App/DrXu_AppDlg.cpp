
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

//����ʵ�ڲ���д��Ԫ�����ˡ�������¼����������֪ͨӦ����NtLoadDriver���õ�
HANDLE		g_hEventNtLoadDriver;
/* �ṹ������� */


/* �������������� */
void ThreadWaitForNtLoadDriverCall(void *dummyParameter)
{
	TRACE("�̺߳���[ThreadWaitForNtLoadDriverCall]:Entered!Whileѭ�����Ͽ�ʼ");
	while(true)
	{
		DWORD dwWaitStatus = 0;
		dwWaitStatus = WaitForSingleObject(g_hEventNtLoadDriver,INFINITE);
		if ( dwWaitStatus != WAIT_OBJECT_0 )
		{
			TRACE("�̺߳���[ThreadWaitForNtLoadDriverCall]��dwWaitStatusʧ�ܣ�");
			TRACE("dwWaitStatus:%d",dwWaitStatus);
			TRACE("GetLastError:%d",GetLastError());
		}
		else
		{
			::AfxMessageBox(L"g_hEventNtLoadDriver�����ѣ�");
		}
		ResetEvent(g_hEventNtLoadDriver);
	}
}




/* �����ඨ���� */
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
		if ( !this->SaveDeviceHandle() )
		{
			::AfxMessageBox(L"���豸ʧ��");
			TRACE("[OnInitDialog] Faild with [SaveDeviceHandle],GetLastError:%d",GetLastError());
			ExitProcess(-1);
		}
		else
		{
			::AfxMessageBox(L"�ɹ���������");

			// TODO:�����¼������ݸ�����������������
			TRACE("[OnInitDialog]�����¼�");
			if( this->CreateNtLoadDriverEvent() )
			{
				::AfxMessageBox(L"�ɹ������ں��¼�NtLoadDriverEvent!");
				TRACE("[OnInitDialog]:�������߳�");
				_beginthread(ThreadWaitForNtLoadDriverCall,1024,NULL);
				//TODO:֪ͨ�ں˽����¼�ͬ��
				/* if ( this->SendBufferToDevice(this->g_hDrXuDevice, //�豸���
										IOCTL_DRXU_OPEN_EVENT_NtLoadDriver, //������Ϣ
										NULL,0) ) */
				DWORD bytesReturned = 0;
				if (DeviceIoControl(this->g_hDrXuDevice, 
						 IOCTL_DRXU_OPEN_EVENT_NtLoadDriver, 
						 NULL, 0, 
						 NULL, 0, 
				 		 &bytesReturned, 
						 NULL) )
				{
					//TODO:�������̹߳��𣬵ȴ�����
					TRACE("[OnInitDialog]���ں˴����¼���ɣ�");
				}
				else
				{
					::AfxMessageBox(L"�ں˻�ȡ�¼�ʧ��");
					TRACE("[OnInitDialog]:�豸��g_hEventNtLoadDriverʧ��");
					TRACE("GetLastError:%d",GetLastError());
					ExitProcess(-1);
				}	
			}
			else
			{
				::AfxMessageBox(L"���ں�ͨ��ʧ��");
				TRACE("[OnInitDialog] Faild with [SaveDeviceHandle],GetLastError:%d",GetLastError());
				ExitProcess(-1);
			}
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
     	TRACE("[LoadDriverFromFile]Faild to Open Service Manager!\n");
        TRACE("[LoadDriverFromFile]GetLastError:%d\n",GetLastError());
        return false;
    }
    ///��ȡ��ǰexe����·��
    GetCurrentDirectoryA(512,aCurrentDirectory);
    _snprintf(sysPath,1022,"%s\\%s",aCurrentDirectory,sysFileName);
    TRACE("[LoadDriverFromFile]:path is:%s\n",sysPath);
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
        TRACE("[LoadDriverFromFile]�Ҳ���RC���\n");
        if(GetLastError()== ERROR_SERVICE_EXISTS)
        {
            rh = OpenServiceA(sh,DRXU_SERVICE_NAME,SERVICE_ALL_ACCESS);
            if(!rh)
            {
                CloseServiceHandle(sh);
                TRACE("[LoadDriverFromFile]���ڷ��񣬵����޷���");
				TRACE("GetLastError:%d",GetLastError());
                return false;
            }
        }
        else
        {
            TRACE("[LoadDriverFromFile]�޷���������!\n");
            TRACE("[LoadDriverFromFile]GetLastError:%d\n",GetLastError());
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
		AfxMessageBox(L"[DestroyWindow]ж������ʧ�� ");
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
		TRACE("GetLastError:%d",GetLastError());
        return false;
    }
    // �򿪷�����BUG�ǿ϶������⣡��Ϊ�õ���OpenServiceA!
    SC_HANDLE hSvc = ::OpenServiceA( hSC, DRXU_SERVICE_NAME,
        SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
    if( hSvc == NULL)
    {
        TRACE( "[StopService]:�򿪷����쳣��");
		TRACE("GetLastError:%d",GetLastError());
        ::CloseServiceHandle( hSC);
        return false;
    }
    // ��÷����״̬
    SERVICE_STATUS status;
    if( ::QueryServiceStatus( hSvc, &status) == FALSE)
    {
        TRACE( "[StopService]:�޷���÷���״̬");
		TRACE("GetLastError:%d",GetLastError());
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
			TRACE("GetLastError:%d",GetLastError());
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
		TRACE("GetLastError:%d",GetLastError());
	    ::CloseServiceHandle( hSvc);
	    ::CloseServiceHandle( hSC);
		return true;
	}
}


void CDrXu_AppDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UCHAR InputBuffer[10] = {0};
	UCHAR OutputBuffer[50] = {0};
	DWORD dwBytesReturned = 0;

	memcpy(InputBuffer,"Testment",sizeof("Testment"));

	this->SendBufferToDevice(this->g_hDrXuDevice,IOCTL_DRXU_TEST_WRITE,InputBuffer,sizeof(InputBuffer));
	this->ReceiveBufferFromDevice(this->g_hDrXuDevice,IOCTL_DRXU_TEST_READ,OutputBuffer,sizeof(OutputBuffer),&dwBytesReturned);
	TRACE("�������ݴ�С:%d",dwBytesReturned);
	TRACE("����������Ϣ:%s",OutputBuffer);
	//CDialogEx::OnOK();
}


bool CDrXu_AppDlg::SaveDeviceHandle(void)
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
			//::AfxMessageBox(L"�������سɹ�");
			TRACE("[SaveDeviceHandle_CreateFile] Success,g_hDrXuDevice is %d",this->g_hDrXuDevice);
			return true;
		}
		else
		{
			//::AfxMessageBox(L"�޷������豸����");
			TRACE("[CreateFile] Faild in [SaveDeviceHandle],Last Error:%d",GetLastError());
			TRACE("[CreateFile] Faild in [SaveDeviceHandle],g_hDrXuDevice:%d",this->g_hDrXuDevice);
			
			if ( this->StopService() )
				::AfxMessageBox(L"��������ֹͣ");
			return false;
		}
}


// �еĻ��ǵ�С�����Ѿ��������ˣ�û�������һ����DeviceIoControl�ķ�װ��
bool CDrXu_AppDlg::SendBufferToDevice(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize)
{
	UCHAR OutputBuffer[10];
    DWORD dwOutput;

	TRACE("[SendBuffer]:Entered");
	//����sizeofҲռ���˶���CPUʱ��
	memset(OutputBuffer,0,sizeof(OutputBuffer));

	bool bRet = DeviceIoControl(hDevice,dwIoControlCode,lpInBuffer,nInBufferSize,&OutputBuffer,sizeof(OutputBuffer),&dwOutput,NULL);
	return bRet;
}


// DeviceIoControl�ķ�װ��See http://msdn.microsoft.com/en-us/windows/desktop/aa363216(v=vs.85).aspx For details
bool CDrXu_AppDlg::ReceiveBufferFromDevice(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned)
{
	TRACE("[ReceiveBufferFromDevice]:Entered");

	bool bRet = DeviceIoControl(hDevice,dwIoControlCode,NULL,0,lpOutBuffer,nOutBufferSize,lpBytesReturned,NULL);
	return bRet;
}


// ������Ӧ����NtLoadDriver�ں˵��õ��¼������������д���
bool CDrXu_AppDlg::CreateNtLoadDriverEvent(void)
{			
	if ( g_hEventNtLoadDriver == 0 )
	{
		//ע����������ǵ��õ�CreateEventW!
		TRACE("[CreateNtLoadDriverEvent]��ʼ�����¼����");
		g_hEventNtLoadDriver = CreateEvent(NULL, //��ȫ��������
										  TRUE,  //�ֶ�����
										  FALSE, //��ʼֵ��eventΪFalse
										  NtLoadDriverEventName); //�¼�����
		if( g_hEventNtLoadDriver == INVALID_HANDLE_VALUE )
		{
			TRACE("[CreateNtLoadDriverEvent]:����g_hEventNtLoadDriverʧ��");
			TRACE("GetLastError:%d",GetLastError());
			return false;
		}
		else
		{
			return true;												
		}
	}
	else
	{
		TRACE("[CreateNtLoadDriverEvent]�¼�����Ѿ�������g_hEventNtLoadDriver:%d",g_hEventNtLoadDriver);
		return true;
	}
}
