
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
bool estadoFreno = false;
bool encendido = false;
bool intermitenteIzq = false;
bool intermitenteDer = false;
bool estadoParpadeoIzq = false;
bool estadoParpadeoDer = false;
UINT_PTR timerIzq = 3;
UINT_PTR timerDer = 3;

int revoluciones = 0;
int temperatura = 0;

bool start = 0;
bool start2 = 0;
bool start3 = 0;

int pintar_temp = 0;
int pintar_revoluciones = 0;
int pos_rojo = 0;
int pos_rojo_2 = 0;


bool cont = 0;
bool cont1 = 0, cont2 = 0;
// CClienteDlg dialog

CClienteDlg::CClienteDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIENTE_DIALOG, pParent)
	, m_ip(_T("127.0.0.1"))
	, m_port(5020)
	, m_tiempo(100)
	, m_ipacc(_T("127.0.0.1"))
	, m_portacc(5021)
	, m_ipmot(_T("127.0.0.1"))
	, m_portmot(5022)
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
	DDX_Control(pDX, IDC_LED_MOT, m_led_mot);
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


	int port = 8088;
	CString cs;
	cs.Format("WebServer iniciado en puerto %d", port);
	EscribirLog(cs);

	s_listen.Create(port, SOCK_STREAM);
	s_listen.Listen();
	// Enlazamos sockets con el diálogo
	s_listen.m_pDlg = this;
	s_conected.m_pDlg = this;
	m_isConnected = false;

	return TRUE;
}

// If you add a minimize button to your dialog, you will need the code below3
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

		cont = 0;
		cont1 = 0;
		cont2 = 0;
	}
}


void CALLBACK TimerParpadeoIzq(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	estadoParpadeoIzq = !estadoParpadeoIzq;
	CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();
	if (pDlg && intermitenteIzq) {
		CDC* pdc = pDlg->m_izq.GetDC();
		CRect r;
		pDlg->m_izq.GetClientRect(r);
		CBrush brush(estadoParpadeoIzq ? RGB(255, 165, 0) : RGB(128, 128, 128));
		pdc->FillRect(r, &brush);
	}
}

void CALLBACK TimerParpadeoDer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
	estadoParpadeoDer = !estadoParpadeoDer;
	CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();
	if (pDlg && intermitenteDer) {
		CDC* pdc = pDlg->m_der.GetDC();
		CRect r;
		pDlg->m_der.GetClientRect(r);
		CBrush brush(estadoParpadeoDer ? RGB(255, 165, 0) : RGB(128, 128, 128));
		pdc->FillRect(r, &brush);
	}
}

void CClienteDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1 && pollingActivo)
	{
		if (StartLuces() == 0) {
			CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();

			CDC* pdc = pDlg->m_ledluces.GetDC();
			CRect r;
			pDlg->m_ledluces.GetClientRect(r);
			CBrush red(RGB(255, 0, 0));
			pdc->FillRect(r, &red);

			if (cont == 1) {
				EscribirLog("Luces XxNOxX");
				cont = 0;
			}
		}
		else {

			CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();

			CDC* pdc = pDlg->m_ledluces.GetDC();
			CRect r;
			pDlg->m_ledluces.GetClientRect(r);
			CBrush green(RGB(0, 255, 0));
			pdc->FillRect(r, &green);


			if (cont == 0) {
				EscribirLog("Luces OK");
				cont = 1;
			}
		}

		if (StartAccionador() == 0) {
			CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();

			CDC* pdc = pDlg->m_ledacc.GetDC();
			CRect r;
			pDlg->m_ledacc.GetClientRect(r);
			CBrush red(RGB(255, 0, 0));
			pdc->FillRect(r, &red);

			if (cont1 == 1) {
				EscribirLog("Accionador XxNOxX");
				cont1 = 0;
			}
		}
		else {
			CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();
			CDC* pdc = pDlg->m_ledacc.GetDC();
			CRect r;
			pDlg->m_ledacc.GetClientRect(r);
			CBrush green(RGB(0, 255, 0));
			pdc->FillRect(r, &green);

			if (cont1 == 0) {
				EscribirLog("Accionador OK");
				cont1 = 1;
			}
		}

		if (StartAccionador() == true && StartLuces() == true)
		{
			PollingLuces_Freno();
			PollingLuces_Int_Der();
			PollingLuces_Int_Izq();
		}

		if (StartMotor() == true)
		{
			CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();
			CDC* pdc = pDlg->m_led_mot.GetDC();
			CRect r;
			pDlg->m_led_mot.GetClientRect(r);
			CBrush green(RGB(0, 255, 0));
			pdc->FillRect(r, &green);

			if (cont2 == 0) {
				EscribirLog("Motor OK");
				cont2 = 1;
			}

			pos_rojo = PollingMotor();

			PaintSpeedometer();

			pos_rojo_2 = pintar_revoluciones;

			Paint_REVO_Speedometer();
		}
		else {
			CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();

			CDC* pdc = pDlg->m_led_mot.GetDC();
			CRect r;
			pDlg->m_led_mot.GetClientRect(r);
			CBrush red(RGB(255, 0, 0));
			pdc->FillRect(r, &red);

			if (cont2 == 1) {
				EscribirLog("Motor XxNOxX");
				cont2 = 0;
			}
		}

	}
	CString nuevoJson;
	nuevoJson.Format("{\"rpm\": %d, \"temp\": %d, \"freno\": \"%s\", \"izq\": \"%s\", \"der\": \"%s\"}",
		pintar_revoluciones, pintar_temp,
		estadoFreno ? "ON" : "OFF",
		intermitenteIzq ? "ON" : "OFF",
		intermitenteDer ? "ON" : "OFF");

	if (nuevoJson != lastJson) {
		lastJson = nuevoJson;
		lastUpdate = COleDateTime::GetCurrentTime();
	}

	// PARPADEO IZQUIERDO
	if (intermitenteIzq) {
		estadoParpadeoIzq = !estadoParpadeoIzq;
		CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();
		CDC* pdc = pDlg->m_izq.GetDC();
		CRect r; pDlg->m_izq.GetClientRect(r);
		CBrush brush(estadoParpadeoIzq ? RGB(255, 165, 0) : RGB(128, 128, 128)); // Amarillo o gris
		pdc->FillRect(r, &brush);
	}
	else {
		CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();
		CDC* pdc = pDlg->m_izq.GetDC();
		CRect r; pDlg->m_izq.GetClientRect(r);
		CBrush brush(RGB(128, 128, 128));
		pdc->FillRect(r, &brush);
	}

	// PARPADEO DERECHO
	if (intermitenteDer) {
		estadoParpadeoDer = !estadoParpadeoDer;
		CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();
		CDC* pdc = pDlg->m_der.GetDC();
		CRect r; pDlg->m_der.GetClientRect(r);
		CBrush brush(estadoParpadeoDer ? RGB(255, 165, 0) : RGB(128, 128, 128)); // Amarillo o gris
		pdc->FillRect(r, &brush);
	}
	else {
		CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();
		CDC* pdc = pDlg->m_der.GetDC();
		CRect r; pDlg->m_der.GetClientRect(r);
		CBrush brush(RGB(128, 128, 128));
		pdc->FillRect(r, &brush);
	}

	CDialogEx::OnTimer(nIDEvent);
}

bool CClienteDlg::StartLuces() {
	UpdateData();
	// Intento crear el Socket con el Slave
	CSocket misoc;
	if (!misoc.Create() || !misoc.Connect(m_ip, m_port)) {
		misoc.Close();
		MessageBox(_T("Fallo en creacion.."));
		start = 0;
		return start;
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
	buf[8] = 566 / 256;
	buf[9] = 566 % 256;
	buf[10] = start / 256;
	buf[11] = start % 256;
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
	start = resp[10] * 256 + resp[11];


	return start;

}

bool CClienteDlg::StartAccionador() {
	UpdateData();
	// Intento crear el Socket con el Slave
	CSocket misoc;
	if (!misoc.Create() || !misoc.Connect(m_ip, m_portacc)) {
		misoc.Close();
		MessageBox(_T("Fallo en creacion.."));
		start = 0;
		return start2;
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
	buf[8] = 570 / 256;
	buf[9] = 570 % 256;
	buf[10] = start2 / 256;
	buf[11] = start2 % 256;
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
	start2 = resp[10] * 256 + resp[11];


	return start2;
}

bool CClienteDlg::StartMotor() {

	UpdateData();
	// Intento crear el Socket con el Slave
	CSocket misoc;
	if (!misoc.Create() || !misoc.Connect(m_ip, m_portmot)) {
		misoc.Close();
		MessageBox(_T("Fallo en creacion Socket motor.."));
		start = 0;
		return start3;
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
	buf[8] = 570 / 256;
	buf[9] = 570 % 256;
	buf[10] = start3 / 256;
	buf[11] = start3 % 256;
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
	start3 = resp[10] * 256 + resp[11];


	return start3;
}


//-------------------------------------------------------------------------------
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
	buf[10] = estadoFreno / 256;
	buf[11] = estadoFreno % 256;
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

	// leo estadoFreno de respuesta, estadoFreno es un booleano

	estadoFreno = resp[10] * 256 + resp[11];


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
	buf[10] = estadoFreno / 256;
	buf[11] = estadoFreno % 256;
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


	if (estadoFreno == 1) {
		CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();
		CDC* pdc = pDlg->m_freno.GetDC();
		CRect r;
		pDlg->m_freno.GetClientRect(r);
		CBrush red(RGB(255, 0, 0));
		pdc->FillRect(r, &red);
	}
	else {
		CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();
		CDC* pdc = pDlg->m_freno.GetDC();
		CRect r;
		pDlg->m_freno.GetClientRect(r);
		CBrush grey(RGB(192, 192, 192));
		pdc->FillRect(r, &grey);
	}
}

void CClienteDlg::PollingLuces_Int_Izq() {
	UpdateData();
	CSocket misoc;
	if (!misoc.Create() || !misoc.Connect(m_ip, m_portacc)) {
		misoc.Close();
		MessageBox(_T("Fallo en conexión para leer intermitente izq.."));
		return;
	}

	// Leer estado real del esclavo (registro 401)
	unsigned char buf[20];
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 6;
	buf[6] = 0x15;
	buf[7] = 0x04;
	buf[8] = 401 / 256;
	buf[9] = 401 % 256;
	buf[10] = 0;
	buf[11] = 1;
	UpdateData(0);
	misoc.Send(buf, 20);

	unsigned char resp[20];
	misoc.Receive(resp, 20);
	short TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox(_T("Respuesta no recibida.."));
	}

	intermitenteIzq = (resp[10] * 256 + resp[11]) != 0;
	KillTimer(timerIzq);
	if (intermitenteIzq) {
		SetTimer(timerIzq, 500, TimerParpadeoIzq);
	}
	else {
		CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();
		if (pDlg) {
			CDC* pdc = pDlg->m_izq.GetDC();
			CRect r;
			pDlg->m_izq.GetClientRect(r);
			CBrush grey(RGB(128, 128, 128));
			pdc->FillRect(r, &grey);
		}
	}
	misoc.Close();

	// Propagar a luces delanteras (501) y traseras (503)
	int addrs[] = { 501, 503 };
	for (int i = 0; i < 2; ++i) {
		int addr = addrs[i];
		if (!misoc.Create() || !misoc.Connect(m_ip, m_port)) {
			misoc.Close();
			MessageBox(_T("Fallo al conectar a luces para izq.."));
			return;
		}
		buf[0] = Trans / 256;
		buf[1] = Trans++ % 256;
		buf[2] = 0;
		buf[3] = 0;
		buf[4] = 0;
		buf[5] = 6;
		buf[6] = 0x15;
		buf[7] = 0x06;
		buf[8] = addr / 256;
		buf[9] = addr % 256;
		buf[10] = intermitenteIzq ? 0 : 0;
		buf[11] = intermitenteIzq ? 1 : 0;
		misoc.Send(buf, 20);
		misoc.Receive(resp, 20);
		TransResp = resp[0] * 256 + resp[1] + 1;
		if (TransResp != Trans) MessageBox(_T("Sin respuesta de luz izq.."));
		misoc.Close();
	}
}

void CClienteDlg::PollingLuces_Int_Der() {
	UpdateData();
	CSocket misoc;
	if (!misoc.Create() || !misoc.Connect(m_ip, m_portacc)) {
		misoc.Close();
		MessageBox(_T("Fallo en conexión para leer intermitente der.."));
		return;
	}

	// Leer estado real del esclavo (registro 402)
	unsigned char buf[20];
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 6;
	buf[6] = 0x15;
	buf[7] = 0x04;
	buf[8] = 402 / 256;
	buf[9] = 402 % 256;
	buf[10] = 0;
	buf[11] = 1;
	UpdateData(0);
	misoc.Send(buf, 20);

	unsigned char resp[20];
	misoc.Receive(resp, 20);
	short TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox(_T("Respuesta no recibida.."));
	}

	intermitenteDer = (resp[10] * 256 + resp[11]) != 0;
	KillTimer(timerDer);
	if (intermitenteDer) {
		SetTimer(timerDer, 500, TimerParpadeoDer);
	}
	else {
		CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();
		if (pDlg) {
			CDC* pdc = pDlg->m_der.GetDC();
			CRect r;
			pDlg->m_der.GetClientRect(r);
			CBrush grey(RGB(128, 128, 128));
			pdc->FillRect(r, &grey);
		}
	}
	misoc.Close();

	// Propagar a luces delanteras (502) y traseras (504)
	int addrs[] = { 502, 504 };
	for (int i = 0; i < 2; ++i) {
		int addr = addrs[i];
		if (!misoc.Create() || !misoc.Connect(m_ip, m_port)) {
			misoc.Close();
			MessageBox(_T("Fallo al conectar a luces para der.."));
			return;
		}
		buf[0] = Trans / 256;
		buf[1] = Trans++ % 256;
		buf[2] = 0;
		buf[3] = 0;
		buf[4] = 0;
		buf[5] = 6;
		buf[6] = 0x15;
		buf[7] = 0x06;
		buf[8] = addr / 256;
		buf[9] = addr % 256;
		buf[10] = intermitenteDer ? 0 : 0;
		buf[11] = intermitenteDer ? 1 : 0;
		misoc.Send(buf, 20);
		misoc.Receive(resp, 20);
		TransResp = resp[0] * 256 + resp[1] + 1;
		if (TransResp != Trans) MessageBox(_T("Sin respuesta de luz der.."));
		misoc.Close();
	}
}

int CClienteDlg::PollingMotor() {

	UpdateData();
	// Intento crear el Socket con el Slave
	CSocket misoc;
	if (!misoc.Create() || !misoc.Connect(m_ip, m_portmot)) {
		misoc.Close();
		MessageBox("Fallo en creacion..");
		return temperatura;
	}

	//Mando datos
	temperatura = 0;
	unsigned char buf[20];
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 6;
	buf[6] = 0x15;
	buf[7] = 0x04;
	buf[8] = 400 / 256;
	buf[9] = 400 % 256;
	buf[10] = 0 / 256;
	buf[11] = 0 % 256;
	UpdateData(0);
	misoc.Send(buf, 20);

	// Verifico que me responde
	unsigned char resp[20];
	misoc.Receive(resp, 20);

	temperatura = resp[10] * 256;
	temperatura += resp[11];
	short TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox("Respuesta no recibida..");
	}
	else if (resp[10] == 10 && resp[11] == 10)
		MessageBox("Sensor apagado..");
	else {
		pintar_temp = temperatura;
		UpdateData(0);
	}

	// Cierro el socket
	misoc.Close();

	UpdateData();
	// Intento crear el Socket con el Slave
	if (!misoc.Create() || !misoc.Connect(m_ip, m_portmot)) {
		misoc.Close();
		MessageBox("Fallo en creacion..");
		return temperatura;
	}

	//Mando datos
	revoluciones = 0;
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 6;
	buf[6] = 0x15;
	buf[7] = 0x04;
	buf[8] = 401 / 256;
	buf[9] = 401 % 256;
	buf[10] = 0 / 256;
	buf[11] = 0 % 256;
	UpdateData(0);
	misoc.Send(buf, 20);

	// Verifico que me responde
	misoc.Receive(resp, 20);

	revoluciones = resp[10] * 256;
	revoluciones += resp[11];
	TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox("Respuesta no recibida..");
	}
	else if (resp[10] == 10 && resp[11] == 10)
		MessageBox("Sensor apagado..");
	else {
		pintar_revoluciones = revoluciones;
		UpdateData(0);
	}

	// Cierro el socket
	misoc.Close();

	return pintar_temp;
}

//-------------------------------------------------------------------------------





// AUN NO LO USO
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
	timestamp.Format("[%02d:%02d:%02d] %s \r\n", now.GetHour(), now.GetMinute(), now.GetSecond(),texto);

	CString logsActuales;
	m_logs.GetWindowText(logsActuales);
	logsActuales += timestamp;
	m_logs.SetWindowText(logsActuales);
	m_logs.LineScroll(m_logs.GetLineCount());

}

void CClienteDlg::OnBnClickedClear()
{
	m_logs.SetWindowText(_T(""));

}



void CClienteDlg::PaintSpeedometer()
{
	CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();

	CDC* pdc = pDlg->m_velocidad.GetDC();

	// Get the dimensions of the control
	CRect rect;
	pDlg->m_velocidad.GetClientRect(rect);

	// Clear background with white
	CBrush brushBackground(RGB(255, 255, 255));
	pdc->FillRect(rect, &brushBackground);

	// Draw the semicircle (speedometer background)
	CPen penBlack(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = pdc->SelectObject(&penBlack);

	// Calculate the center point and radius
	int centerX = rect.Width() / 2;
	int centerY = rect.bottom;
	int radius = min(rect.Width() / 2, rect.Height());

	// Draw the semicircle (arc)
	pdc->Arc(centerX - radius, centerY - radius,
		centerX + radius, centerY + radius,
		centerX + radius, centerY,
		centerX - radius, centerY);


	// Calculate the angle for the needle based on pos_rojo
	// 0 degrees is at 3 o'clock, we need to go from 180 to 0 degrees
	const double minAngle = 3.14159 * 0.05; // ~5° in radians
	const double maxAngle = 3.14159 * 0.99; // ~175° in radians

	// Map pos_rojo (0-300) to the restricted angle range
	double angle = maxAngle + (pos_rojo * (maxAngle - minAngle) / 300.0);

	if (angle < 2) {
		angle = angle++;
	}
	else {

	}

	// Draw the needle (red line)
	CPen penRed(PS_SOLID, 2, RGB(255, 0, 0));
	pdc->SelectObject(&penRed);

	// Calculate end point of needle
	int needleEndX = centerX + static_cast<int>(cos(angle) * (radius - 10));
	int needleEndY = centerY + static_cast<int>(sin(angle) * (radius - 10));

	if (needleEndY > 55)
	{
		needleEndY = needleEndY - 3;
	}

	// Draw the needle
	pdc->MoveTo(centerX, centerY);
	pdc->LineTo(needleEndX, needleEndY);

	// Clean up
	pdc->SelectObject(pOldPen);
	ReleaseDC(pdc);
}

void CClienteDlg::Paint_REVO_Speedometer() {

	CClienteDlg* pDlg = (CClienteDlg*)AfxGetMainWnd();

	CDC* pdc = pDlg->m_revoluciones.GetDC();

	// Get the dimensions of the control
	CRect rect;
	pDlg->m_revoluciones.GetClientRect(rect);

	// Clear background with white
	CBrush brushBackground(RGB(255, 255, 255));
	pdc->FillRect(rect, &brushBackground);

	// Draw the semicircle (speedometer background)
	CPen penBlack(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = pdc->SelectObject(&penBlack);

	// Calculate the center point and radius
	int centerX = rect.Width() / 2;
	int centerY = rect.bottom;
	int radius = min(rect.Width() / 2, rect.Height());

	// Draw the semicircle (arc)
	pdc->Arc(centerX - radius, centerY - radius,
		centerX + radius, centerY + radius,
		centerX + radius, centerY,
		centerX - radius, centerY);


	// Calculate the angle for the needle based on pos_rojo
	// 0 degrees is at 3 o'clock, we need to go from 180 to 0 degrees
	const double minAngle = 3.14159 * 0.05; // ~5° in radians
	const double maxAngle = 3.14159 * 0.99; // ~175° in radians

	// Map pos_rojo (0-300) to the restricted angle range
	double angle = maxAngle + (pos_rojo_2 * (maxAngle - minAngle) / 7000.0);

	if (angle < 2) {
		angle = angle++;
	}
	else {

	}

	// Draw the needle (red line)
	CPen penRed(PS_SOLID, 2, RGB(255, 0, 0));
	pdc->SelectObject(&penRed);

	// Calculate end point of needle
	int needleEndX = centerX + static_cast<int>(cos(angle) * (radius - 10));
	int needleEndY = centerY + static_cast<int>(sin(angle) * (radius - 10));

	if (needleEndY > 55)
	{
		needleEndY = needleEndY - 3;
	}

	// Draw the needle
	pdc->MoveTo(centerX, centerY);
	pdc->LineTo(needleEndX, needleEndY);

	// Clean up
	pdc->SelectObject(pOldPen);
	ReleaseDC(pdc);

}

//NUEVO
void CClienteDlg::OnAccept()
{
	CString cs, strIP;
	UINT port;

	if (m_isConnected)
		s_conected.Detach();

	if (s_listen.Accept(s_conected)) {
		s_conected.GetSockName(strIP, port);
		cs.Format("Cliente conectado: IP=%s puerto=%d", strIP.GetBuffer(), port);
		EscribirLog(cs);
		m_isConnected = true;
	}
	else {
		EscribirLog("Error: no se acepta más conexiones");
	}
}
void CClienteDlg::OnReceive()
{
	char bufweb[10240];
	int len = s_conected.Receive(bufweb, sizeof(bufweb) - 1);
	if (len <= 0) {
		s_conected.Close();
		m_isConnected = false;
		return;
	}

	bufweb[len] = 0;
	CString msg = GetMsg(bufweb, len);
	CString response;

	if (msg.Find("/estado") != -1)
	{
		CString json = lastJson;

		const char* header = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
		s_conected.Send(header, strlen(header));
		s_conected.Send(json.GetBuffer(), json.GetLength());
	}
	else
	{
		CString html = getPage();
		const char* header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
		s_conected.Send(header, strlen(header));
		s_conected.Send(html.GetBuffer(), html.GetLength());
	}

	s_conected.Close();
	m_isConnected = false;
}



CString CClienteDlg::getPage()
{
	CString html;
	html += "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Estado Vehículo</title>";
	html += "<style>";
	html += "body { font-family: Arial; background-color: #1e1e2f; color: white; text-align: center; }";
	html += ".container { margin: 50px auto; width: 500px; padding: 20px; background-color: #2e2e3e; border-radius: 10px; box-shadow: 0px 0px 10px rgba(255,255,255,0.1); }";
	html += ".title { font-size: 24px; margin-bottom: 20px; }";
	html += "canvas { background-color: #1e1e2f; display: block; margin: 20px auto; }";
	html += ".label { font-weight: bold; color: #ccc; margin-top: 10px; }";
	html += ".value { font-size: 18px; margin-bottom: 10px; }";
	html += "</style>";
	html += "<script>";
	html += "let rpm = 0, temp = 0;";
	html += "function drawGauge(canvasId, value, max, label) {";
	html += "  const canvas = document.getElementById(canvasId);";
	html += "  const ctx = canvas.getContext('2d');";
	html += "  const w = canvas.width, h = canvas.height, cx = w/2, cy = h;";
	html += "  ctx.clearRect(0, 0, w, h);";
	html += "  ctx.beginPath(); ctx.arc(cx, cy, cx-10, Math.PI, 2*Math.PI); ctx.lineWidth = 10; ctx.strokeStyle = '#444'; ctx.stroke();";
	html += "  let angle = Math.PI + (value / max) * Math.PI;";
	html += "  const x = cx + Math.cos(angle) * (cx - 20);";
	html += "  const y = cy + Math.sin(angle) * (cx - 20);";
	html += "  ctx.beginPath(); ctx.moveTo(cx, cy); ctx.lineTo(x, y); ctx.strokeStyle = 'red'; ctx.lineWidth = 4; ctx.stroke();";
	html += "  ctx.fillStyle = 'white'; ctx.font = '16px Arial'; ctx.fillText(label + ': ' + value, cx - 40, cy - 10);";
	html += "}";
	html += "function actualizar() {";
	html += "  fetch('/estado').then(res => res.json()).then(data => {";
	html += "    rpm = data.rpm; temp = data.temp;";
	html += "    document.getElementById('freno').textContent = data.freno;";
	html += "    document.getElementById('izq').textContent = data.izq;";
	html += "    document.getElementById('der').textContent = data.der;";
	html += "    drawGauge('rpmCanvas', rpm, 7000, 'RPM');";
	html += "    drawGauge('tempCanvas', temp, 300, 'TEMP');";
	html += "    setTimeout(actualizar, 1000);";
	html += "  }).catch(() => setTimeout(actualizar, 1000));";
	html += "}";
	html += "window.onload = actualizar;";
	html += "</script>";
	html += "</head><body><div class='container'><div class='title'>Estado del Sistema</div>";
	html += "<canvas id='rpmCanvas' width='300' height='150'></canvas>";
	html += "<canvas id='tempCanvas' width='300' height='150'></canvas>";
	html += "<div class='label'>Freno:</div><div id='freno' class='value'>...</div>";
	html += "<div class='label'>Intermitente Izq:</div><div id='izq' class='value'>...</div>";
	html += "<div class='label'>Intermitente Der:</div><div id='der' class='value'>...</div>";
	html += "</div></body></html>";
	return html;
}







CString CClienteDlg::GetMsg(char* buf, int n)
{
	CString cs(buf), str;
	int pos = cs.Find(" HTTP");
	if (pos != -1) {
		str = cs.Left(pos);
	}
	return str;
}

void CClienteDlg::OnClose()
{
	s_conected.Close();
	m_isConnected = false;
	EscribirLog(_T("Conexión cerrada por el cliente"));
}