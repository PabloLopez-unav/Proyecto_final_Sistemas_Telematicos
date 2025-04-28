
// Servidor_LucesDlg.h : header file
//

#pragma once

class CMySocket : public CSocket
{
public:
	CMySocket() {}
	virtual void OnAccept(int err);
};

// CServidorLucesDlg dialog
class CServidorLucesDlg : public CDialogEx
{
// Construction
public:
	CServidorLucesDlg(CWnd* pParent = nullptr);	// standard constructor

	CMySocket* misoc;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVIDOR_LUCES_DIALOG };
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
	CStatic m_led_1;
	CStatic m_led_arr_der;
	CStatic m_led_arr_izq;
	CStatic m_led_deb_der;
	CStatic m_led_deb_izq;
	CString m_port;
	CString m_testo_dir_freno_nada;
	int dir_led_1;
	int dir_led_arr_izq;
	int dir_led_arr_der;
	int dir_led_deb_izq;
	int dir_led_deb_der;
};
