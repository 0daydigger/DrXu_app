
// DrXu_AppDlg.h : ͷ�ļ�
//

#pragma once


// CDrXu_AppDlg �Ի���
class CDrXu_AppDlg : public CDialogEx
{
// ����
public:
	CDrXu_AppDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DRXU_APP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	// ���������ļ�
	bool LoadDriverFromFile(char* sysFileName);
public:
	virtual BOOL DestroyWindow();
private:
	// ֹͣ����ж������
	bool StopService(void);
	// ȫ���豸���������������ľ��
	HANDLE g_hDrXuDevice;
public:
	afx_msg void OnBnClickedOk();
	bool SaveDeviceHandle(void);
private:
	// �еĻ��ǵ�С�����Ѿ��������ˣ�û�������һ����DeviceIoControl�ķ�װ��
	bool SendBufferToDevice(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize);
	// DeviceIoControl�ķ�װ��See http://msdn.microsoft.com/en-us/windows/desktop/aa363216(v=vs.85).aspx For details
	bool ReceiveBufferFromDevice(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPDWORD lpBytesReturned);
	// ������Ӧ����NtLoadDriver�ں˵��õ��¼������������д���
	bool CreateNtLoadDriverEvent(void);
};
