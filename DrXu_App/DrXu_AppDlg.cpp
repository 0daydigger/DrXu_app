
// DrXu_AppDlg.cpp : 实现文件
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


/* 加载驱动需要的结构 */
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
/* 结构定义完毕 */

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CDrXu_AppDlg 对话框




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


// CDrXu_AppDlg 消息处理程序

BOOL CDrXu_AppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 加载驱动程序
	// 从当前目录下加载驱动程序
	if ( LoadDriverFromFile("DrXu.sys" ) )
	{
		//打开设备句柄
		this->g_hDrXuDevice = CreateFile(SYMBOLIC_LINK_DRXU,
										  GENERIC_ALL,//GENERIC_READ | GENERIC_WRITE,
										  0,
										  NULL,
										  OPEN_EXISTING,
										  FILE_ATTRIBUTE_NORMAL,
										  NULL);
		if ( this->g_hDrXuDevice != INVALID_HANDLE_VALUE )
		{
			::AfxMessageBox(L"驱动加载成功");
			TRACE("[CreateFile] Success,g_hDrXuDevice is %d",this->g_hDrXuDevice);
		}
		else
		{
			::AfxMessageBox(L"无法访问设备对象");
			TRACE("[CreateFile] Faild in [InitDialog],Last Error:%d",GetLastError());
			TRACE("[CreateFile] Faild in [InitDialog],g_hDrXuDevice:%d",this->g_hDrXuDevice);
			
			if ( this->StopService() )
				::AfxMessageBox(L"驱动服务停止");
			ExitProcess(-1);
		}
	}
	else
	{
		::AfxMessageBox(L"驱动加载失败");
		ExitProcess(-1);
	}
	// 关闭驱动设备对象
	
	// CloseHandle(this->g_hDrXuDevice); 2013.10.26 0:26 貌似这有个bug

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MAXIMIZE);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDrXu_AppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDrXu_AppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// 加载驱动文件
bool CDrXu_AppDlg::LoadDriverFromFile(char* sysFileName)
{
    char sysPath[1024] = {0};
    char aCurrentDirectory[515] = {0};

    ///打开服务管理器
    SC_HANDLE sh = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(!sh)
    {
     //   printf("Faild to Open Service Manager!\n");
     //   printf("GetLastError:%d\n",GetLastError());
        return false;
    }
    ///获取当前exe所在路径
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
        TRACE("找不到RC句柄\n");
        if(GetLastError()== ERROR_SERVICE_EXISTS)
        {
            rh = OpenServiceA(sh,DRXU_SERVICE_NAME,SERVICE_ALL_ACCESS);
            if(!rh)
            {
                CloseServiceHandle(sh);
                TRACE("存在服务，但是无法打开\n");
                return false;
            }
        }
        else
        {
            TRACE("无法创建服务!\n");
            TRACE("GetLastError:%d\n",GetLastError());
            CloseServiceHandle(sh);
            return false;
        }
    }
    //服务启动
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
              TRACE("[LoadDriverFromFile]:无法启动服务\n");
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
		AfxMessageBox(L"[DestroyWindow]成功卸载驱动");
	}
	else
	{
		AfxMessageBox(L"[DestroyWindow]卸载驱动失败 :(");
	}
	return CDialogEx::DestroyWindow();
}


// 停止服务并卸下驱动
bool CDrXu_AppDlg::StopService(void)
{ 
	// 关闭内核设备
	CloseHandle(this->g_hDrXuDevice);
	TRACE("[StopService]:已经卸下设备句柄g_hDrXuDevice");
	// 打开服务管理对象
    SC_HANDLE hSC = ::OpenSCManager( NULL, 
                        NULL, GENERIC_EXECUTE);
    if( hSC == NULL)
    {
        TRACE( "[StopService]:打开SCManager失败");
        return false;
    }
    // 打开服务。
    SC_HANDLE hSvc = ::OpenServiceA( hSC, DRXU_SERVICE_NAME,
        SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
    if( hSvc == NULL)
    {
        TRACE( "[StopService]:打开服务异常。");
        ::CloseServiceHandle( hSC);
        return false;
    }
    // 获得服务的状态
    SERVICE_STATUS status;
    if( ::QueryServiceStatus( hSvc, &status) == FALSE)
    {
        TRACE( "[StopService]:无法获得服务状态");
        ::CloseServiceHandle( hSvc);
        ::CloseServiceHandle( hSC);
        return false;
    }
    //如果处于运行状态就停止
    if( status.dwCurrentState == SERVICE_RUNNING)
    {
        // 停止服务
        if( ::ControlService( hSvc, 
          SERVICE_CONTROL_STOP, &status) == FALSE)
        {
            TRACE( "[StopService]停止服务失败");
            ::CloseServiceHandle( hSvc);
            ::CloseServiceHandle( hSC);
            return false;
        }
        // 等待服务停止
        while( ::QueryServiceStatus( hSvc, &status) == TRUE)
        {
            ::Sleep( status.dwWaitHint);
            if( status.dwCurrentState == SERVICE_STOPPED)
            {
               // AfxMessageBox( "stop success。");
                ::CloseServiceHandle( hSvc);
                ::CloseServiceHandle( hSC);
                return true;
            }
        }
		return false;
    }
	else
	{
		TRACE("[StopService]服务并未运行,status.dwCurrentState:%d",status.dwCurrentState);
	    ::CloseServiceHandle( hSvc);
	    ::CloseServiceHandle( hSC);
		return true;
	}
}


void CDrXu_AppDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
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
