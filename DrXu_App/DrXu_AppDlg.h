
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
};
