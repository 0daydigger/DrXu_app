
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
};
