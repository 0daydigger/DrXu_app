
// DrXu_AppDlg.h : 头文件
//

#pragma once


// CDrXu_AppDlg 对话框
class CDrXu_AppDlg : public CDialogEx
{
// 构造
public:
	CDrXu_AppDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DRXU_APP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	// 加载驱动文件
	bool LoadDriverFromFile(char* sysFileName);
public:
	virtual BOOL DestroyWindow();
private:
	// 停止服务并卸下驱动
	bool StopService(void);
	// 全局设备句柄，保存打开驱动的句柄
	HANDLE g_hDrXuDevice;
public:
	afx_msg void OnBnClickedOk();
	bool SaveDeviceHandle(void);
private:
	// 有的机智的小朋友已经看出来了，没错，这就是一个对DeviceIoControl的封装。
	bool SendBufferToDevice(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize);
	// DeviceIoControl的封装，See http://msdn.microsoft.com/en-us/windows/desktop/aa363216(v=vs.85).aspx For details
	bool ReceiveBufferFromDevice(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned);
	// 创建响应拦截NtLoadDriver内核调用的事件，并向驱动中传递
	bool CreateNtLoadDriverEvent(void);
};
