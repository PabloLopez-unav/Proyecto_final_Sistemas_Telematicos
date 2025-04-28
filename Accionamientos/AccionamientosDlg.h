
// AccionamientosDlg.h : header file
// 

#pragma once

class CMySocket : public CSocket
{
public:
	CMySocket() {}
	virtual void OnAccept(int err);
};

// CAccionamientosDlg dialog
class CAccionamientosDlg : public CDialogEx
{
// Construction
public:
	CAccionamientosDlg(CWnd* pParent = nullptr);	// standard constructor
	
	CMySocket* misoc;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACCIONAMIENTOS_DIALOG };
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
	BOOL m_boton_freno;
	BOOL m_boton_int_izq;
	BOOL m_boton_int_der;
	int dir_freno;
	int dir_int_der;
	int dir_int_izq;
	afx_msg void OnBnClickedStart();
};
