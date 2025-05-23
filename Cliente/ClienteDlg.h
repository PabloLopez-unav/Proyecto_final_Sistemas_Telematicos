
// ClienteDlg.h : header file
//

#pragma once

#include "websocket.h"
// CClienteDlg dialog
class CClienteDlg : public CDialogEx
{
// Construction
public:
	CClienteDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENTE_DIALOG };
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
	CString m_ip;
	int m_port;
	CStatic m_ledluces;
	int m_tiempo;
	afx_msg void OnBnClickedStartstop();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	CSocket misoc;
	unsigned char buf[20];
	unsigned char res[260];
	short trans = 0;
	bool pollingActivo = false;
	CString ip = "127.0.0.1"; // O la IP real de tu servidor de luces
	int port = 5020;

	bool StartLuces();
	bool StartAccionador();
	bool StartMotor();


	void PollingLuces_Freno();
	void PollingLuces_Int_Der();
	void PollingLuces_Int_Izq();
	int PollingMotor();

	void PaintSpeedometer();
	void Paint_REVO_Speedometer();


	void ProcesarRespuestaLuces();
	void EscribirLog(const CString& texto);
public:
	CString m_ipacc;
	int m_portacc;
	CStatic m_ledacc;
	CStatic m_izq;
	CStatic m_der;
	CStatic m_freno;
	CString m_ipmot;
	int m_portmot;
	CStatic m_velocidad;
	CStatic m_revoluciones;
	CEdit m_logs;
	afx_msg void OnBnClickedClear();
	CStatic m_led_mot;

	// Web server socket
	bool m_isConnected;

	// Funciones
	void OnClose();
	CString getPage();
	CString GetMsg(char* buf, int n);
	CString lastJson; //ultimo estado enviado al navegador
	COleDateTime lastUpdate; //ultimo instante en que hubo cambio de estado
	static UINT ThreadWebServer(LPVOID pParam);
	static UINT ThreadParpadeoIzq(LPVOID pParam);
	static UINT ThreadParpadeoDer(LPVOID pParam);

	CWinThread* handleThreadIzq = nullptr;
	CWinThread* handleThreadDer = nullptr;

	bool pararParpadeoIzq = false;
	bool pararParpadeoDer = false;
};


