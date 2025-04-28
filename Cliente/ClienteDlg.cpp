
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

short Trans = 10;
bool encendido = 0;
bool encendido2 = 0;
bool encendido3 = 0;

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

	SetWindowText(_T("Centralita"));


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
		PollingLuces_Freno();
		PollingLuces_Int_Der();
		PollingLuces_Int_Izq();



	}

	CDialogEx::OnTimer(nIDEvent);
}

void CClienteDlg::PollingLuces_Freno()
{
	UpdateData();
	// Intento crear el Socket con el Slave
	CSocket misoc;
	if (!misoc.Create() || !misoc.Connect(m_ip, m_portacc)) {
		misoc.Close();
		MessageBox("Fallo en creacion..");
		return;
	}

	//Mando datos
	unsigned char buf[20];
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 6;
	buf[6] = 0x15;
	buf[7] = 06;
	buf[8] = 400 / 256;
	buf[9] = 400 % 256;
	buf[10] = encendido / 256;
	buf[11] = encendido % 256;
	UpdateData(0);
	misoc.Send(buf, 20);

	// Verifico que me responde
	unsigned char resp[20];
	misoc.Receive(resp, 20);
	short TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox("Respuesta no recibida..");
	}

	// Cierro el socket
	misoc.Close();

	// leo encendido de respuesta, encendido es un booleano

	encendido = resp[10] * 256 + resp[11];


	//-------------------------------------------------------------------------------
	// No hace falta definir el mismo nombre otra vez que da error
	if (!misoc.Create() || !misoc.Connect(m_ip, m_port)) {
		misoc.Close();
		MessageBox("Fallo en creacion..");
		return;
	}

	//Mando datos
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 6;
	buf[6] = 0x15;
	buf[7] = 06;
	buf[8] = 500 / 256;
	buf[9] = 500 % 256;
	buf[10] = encendido / 256;
	buf[11] = encendido % 256;
	UpdateData(0);
	misoc.Send(buf, 20);

	// Verifico que me responde
	// unsigned char resp[20];
	misoc.Receive(resp, 20);
	// short  // no la vuelvo a redefinir
	TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox("Respuesta no recibida..");
	}

	// Cierro el socket
	misoc.Close();
}

void CClienteDlg::PollingLuces_Int_Izq() {
	UpdateData();
	// Intento crear el Socket con el Slave
	CSocket misoc;
	if (!misoc.Create() || !misoc.Connect(m_ip, m_portacc)) {
		misoc.Close();
		MessageBox(_T("Fallo en creacion.."));
		return;
	}

	//Mando datos
	encendido2 = 1;
	unsigned char buf[20];
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 6;
	buf[6] = 0x15;
	buf[7] = 06;
	buf[8] = 401 / 256;
	buf[9] = 401 % 256;
	buf[10] = encendido2 / 256;
	buf[11] = encendido2 % 256;
	UpdateData(0);
	misoc.Send(buf, 20);

	// Verifico que me responde
	unsigned char resp[20];
	misoc.Receive(resp, 20);
	short TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox(_T("Respuesta no recibida.."));
	}

	// Cierro el socket
	misoc.Close();

	// leo encendido de respuesta, encendido es un booleano

	encendido2 = resp[10] * 256 + resp[11];


	//---------------------intermitente delante--------------------------------------
	// No hace falta definir el mismo nombre otra vez que da error
	if (!misoc.Create() || !misoc.Connect(m_ip, m_port)) {
		misoc.Close();
		MessageBox("Fallo en creacion..");
		return;
	}

	//Mando datos
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 6;
	buf[6] = 0x15;
	buf[7] = 06;
	buf[8] = 501 / 256;
	buf[9] = 501 % 256;
	buf[10] = encendido2 / 256;
	buf[11] = encendido2 % 256;
	UpdateData(0);
	misoc.Send(buf, 20);

	// Verifico que me responde
	// unsigned char resp[20];
	misoc.Receive(resp, 20);
	// short  // no la vuelvo a redefinir
	TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox("Respuesta no recibida..");
	}

	// Cierro el socket
	misoc.Close();

	// intermitente detras

	if (!misoc.Create() || !misoc.Connect(m_ip, m_port)) {
		misoc.Close();
		MessageBox("Fallo en creacion..");
		return;
	}

	//Mando datos
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 6;
	buf[6] = 0x15;
	buf[7] = 06;
	buf[8] = 503 / 256;
	buf[9] = 503 % 256;
	buf[10] = encendido2 / 256;
	buf[11] = encendido2 % 256;
	UpdateData(0);
	misoc.Send(buf, 20);

	// Verifico que me responde
	// unsigned char resp[20];
	misoc.Receive(resp, 20);
	// short  // no la vuelvo a redefinir
	TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox("Respuesta no recibida..");
	}

	// Cierro el socket
	misoc.Close();
}

void CClienteDlg::PollingLuces_Int_Der() {
	UpdateData();
	// Intento crear el Socket con el Slave
	CSocket misoc;
	if (!misoc.Create() || !misoc.Connect(m_ip, m_portacc)) {
		misoc.Close();
		MessageBox(_T("Fallo en creacion.."));
		return;
	}

	//Mando datos
	encendido2 = 1;
	unsigned char buf[20];
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 6;
	buf[6] = 0x15;
	buf[7] = 06;
	buf[8] = 402 / 256;
	buf[9] = 402 % 256;
	buf[10] = encendido2 / 256;
	buf[11] = encendido2 % 256;
	UpdateData(0);
	misoc.Send(buf, 20);

	// Verifico que me responde
	unsigned char resp[20];
	misoc.Receive(resp, 20);
	short TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox(_T("Respuesta no recibida.."));
	}

	// Cierro el socket
	misoc.Close();

	// leo encendido de respuesta, encendido es un booleano

	encendido2 = resp[10] * 256 + resp[11];


	//---------------------intermitente delante--------------------------------------
	// No hace falta definir el mismo nombre otra vez que da error
	if (!misoc.Create() || !misoc.Connect(m_ip, m_port)) {
		misoc.Close();
		MessageBox("Fallo en creacion..");
		return;
	}

	//Mando datos
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 6;
	buf[6] = 0x15;
	buf[7] = 06;
	buf[8] = 502 / 256;
	buf[9] = 502 % 256;
	buf[10] = encendido2 / 256;
	buf[11] = encendido2 % 256;
	UpdateData(0);
	misoc.Send(buf, 20);

	// Verifico que me responde
	// unsigned char resp[20];
	misoc.Receive(resp, 20);
	// short  // no la vuelvo a redefinir
	TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox("Respuesta no recibida..");
	}

	// Cierro el socket
	misoc.Close();

	// intermitente detras

	if (!misoc.Create() || !misoc.Connect(m_ip, m_port)) {
		misoc.Close();
		MessageBox("Fallo en creacion..");
		return;
	}

	//Mando datos
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 6;
	buf[6] = 0x15;
	buf[7] = 06;
	buf[8] = 504 / 256;
	buf[9] = 504 % 256;
	buf[10] = encendido2 / 256;
	buf[11] = encendido2 % 256;
	UpdateData(0);
	misoc.Send(buf, 20);

	// Verifico que me responde
	// unsigned char resp[20];
	misoc.Receive(resp, 20);
	// short  // no la vuelvo a redefinir
	TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox("Respuesta no recibida..");
	}

	// Cierro el socket
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
