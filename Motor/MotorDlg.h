
// MotorDlg.h : header file
//

#pragma once

class CMySocket : public CSocket
{
public:
	CMySocket() {}
	virtual void OnAccept(int err);
};

// CMotorDlg dialog
class CMotorDlg : public CDialogEx
{
// Construction
public:
	CMotorDlg(CWnd* pParent = nullptr);	// standard constructor
	CMySocket* misoc;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOTOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_port;
	int dir_rpm;
	int dir_temperatura;
	afx_msg void OnBnClickedStart();
};
