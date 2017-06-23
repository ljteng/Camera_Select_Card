
// Camera_Select_CardDlg.h : ͷ�ļ�
//

#pragma once


// CCamera_Select_CardDlg �Ի���
class CCamera_Select_CardDlg : public CDialogEx
{
// ����
public:
	CCamera_Select_CardDlg(CWnd* pParent = NULL);	// ��׼���캯��
	bool SearchCOM(char *pName,const char *pFindName);

// �Ի�������
	enum { IDD = IDD_CAMERA_SELECT_CARD_DIALOG };

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
