
// Camera_Select_CardDlg.h : 头文件
//

#pragma once


// CCamera_Select_CardDlg 对话框
class CCamera_Select_CardDlg : public CDialogEx
{
// 构造
public:
	CCamera_Select_CardDlg(CWnd* pParent = NULL);	// 标准构造函数
	bool SearchCOM(char *pName,const char *pFindName);

// 对话框数据
	enum { IDD = IDD_CAMERA_SELECT_CARD_DIALOG };

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
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonConnectDobot();
	afx_msg void OnBnClickedButtonCloseDobot();
	afx_msg void OnBnClickedButtonConnectCamer();
	afx_msg void OnBnClickedButtonStartSelection();
	afx_msg void OnBnClickedButtonCenterJust();
	afx_msg void OnBnClickedButtonLeftJust();
	afx_msg void OnBnClickedButtonRightJust();
	void Save3Point(void);
	void Load3Point(void);
	afx_msg void OnBnClickedButtonDobotGoHome();
//	afx_msg void OnClose();
	afx_msg void OnDestroy();
};
