
// ClienteDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Cliente.h"
#include "ClienteDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClienteDlg dialog

CClienteDlg::CClienteDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENTE_DIALOG, pParent)
	, m_ip(_T(""))
	, m_port(0)
	, m_tiempo(0)
	, m_ipacc(_T(""))
	, m_portacc(0)
	, m_ipmot(_T(""))
	, m_portmot(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClienteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IP, m_ip);
	DDX_Text(pDX, IDC_PORT, m_port);
	DDX_Control(pDX, IDC_LEDLUCES, m_ledluces);
	DDX_Text(pDX, IDC_TIEMPO, m_tiempo);
	DDX_Text(pDX, IDC_IPACC, m_ipacc);
	DDX_Text(pDX, IDC_PORTACC, m_portacc);
	DDX_Control(pDX, IDC_LEDACC, m_ledacc);
	DDX_Control(pDX, IDC_IZQ, m_izq);
	DDX_Control(pDX, IDC_DER, m_der);
	DDX_Control(pDX, IDC_FRENO, m_freno);
	DDX_Text(pDX, IDC_IPMOT, m_ipmot);
	DDX_Text(pDX, IDC_PORTMOT, m_portmot);
	DDX_Control(pDX, IDC_VELOCIDAD, m_velocidad);
	DDX_Control(pDX, IDC_REVOLUCIONES, m_revoluciones);
	DDX_Control(pDX, IDC_LOGS, m_logs);
}

BEGIN_MESSAGE_MAP(CClienteDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_STARTSTOP, &CClienteDlg::OnBnClickedStartstop)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CLEAR, &CClienteDlg::OnBnClickedClear)
END_MESSAGE_MAP()


// CClienteDlg message handlers

BOOL CClienteDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClienteDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClienteDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CClienteDlg::OnBnClickedStartstop()
{
	UpdateData(TRUE); // Leer cuadros de texto
	pollingActivo = !pollingActivo;

	if (pollingActivo)
	{
		SetTimer(1, m_tiempo, NULL); // m_tiempo viene del cuadro de edición "Tiempo (ms)"
		EscribirLog("Polling iniciado");
	}
	else
	{
		KillTimer(1);
		EscribirLog("Polling detenido");
	}
}




void CClienteDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1 && pollingActivo)
	{
		PollingLuces();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CClienteDlg::PollingLuces()
{
	UpdateData(TRUE); // Leer IP y puerto de luces
	if (m_ip.IsEmpty() || m_port == 0) {
		EscribirLog("IP o Puerto de Luces no válido");
		return;
	}

	if (!misoc.Create()) {
		EscribirLog("Error creando socket");
		return;
	}

	if (!misoc.Connect(m_ip, m_port)) {
		EscribirLog("No se pudo conectar al servidor de luces");
		misoc.Close();
		return;
	}

	buf[0] = trans >> 8;
	buf[1] = trans & 0xFF;
	trans++;

	buf[2] = 0; buf[3] = 0;
	buf[4] = 0; buf[5] = 6;

	buf[6] = 1;         // Unit ID
	buf[7] = 0x03;      // Function Code 0x03: Read Holding Registers

	buf[8] = 0x01;
	buf[9] = 0xF4;      // Address 500 (0x01F4)

	buf[10] = 0x00;
	buf[11] = 0x05;     // Leer 5 registros

	misoc.Send(buf, 12);

	int len_res = misoc.Receive(res, 260);
	if (len_res > 0) {
		ProcesarRespuestaLuces();
	}
	else {
		EscribirLog("No se recibió respuesta de luces");
	}

	misoc.Close();
}
void CClienteDlg::ProcesarRespuestaLuces()
{
	if (res[7] == 0x03)
	{
		int freno = (res[9] << 8) | res[10];
		int izq_del = (res[11] << 8) | res[12];
		int der_del = (res[13] << 8) | res[14];
		int izq_tras = (res[15] << 8) | res[16];
		int der_tras = (res[17] << 8) | res[18];

		CString estado;
		estado.Format("Freno:%d IzqDel:%d DerDel:%d IzqTras:%d DerTras:%d",
			freno, izq_del, der_del, izq_tras, der_tras);

		EscribirLog(estado);

		// Aquí puedes cambiar colores o textos de tus controles IDC_FRENO, IDC_IZQ, etc.
		// Ejemplo:
		m_freno.SetWindowText(freno ? _T("ON") : _T("OFF"));
		m_izq.SetWindowText(izq_del ? _T("ON") : _T("OFF"));

		// Actualizar LED general de luces
		m_ledluces.SetWindowText(_T("OK")); // Mejor hacer cambiar color, pero como mínimo poner texto
	}
	else
	{
		EscribirLog("Error en función recibida");
	}
}

void CClienteDlg::EscribirLog(const CString& texto)
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	CString timestamp;
	timestamp.Format("[%02d:%02d:%02d] %s\r\n",
		now.GetHour(), now.GetMinute(), now.GetSecond(),
		texto);

	CString logsActuales;
	m_logs.GetWindowText(logsActuales);
	logsActuales += timestamp;
	m_logs.SetWindowText(logsActuales);
}

void CClienteDlg::OnBnClickedClear()
{
	m_logs.SetWindowText(_T(""));;
}
