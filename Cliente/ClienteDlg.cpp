
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
bool estadoParpadeoDer = false;
bool estadoParpadeoIzq = false;
bool intermitenteIzq_activo = false;
bool intermitenteDer_activo = false;

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

	// Enlazamos sockets con el diálogo
	m_isConnected = false;
	AfxBeginThread(ThreadWebServer, this);

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

		// --- Forzar reactivación de intermitentes en primera iteración ---
		extern bool intermitenteIzq_activo;
		extern bool intermitenteDer_activo;

		intermitenteIzq_activo = false;
		intermitenteDer_activo = false;

		// IMPORTANTE: forzar que se detecte cambio en el polling
		static bool previoIzq = false;
		static bool previoDer = false;

		previoIzq = false;
		previoDer = false;

	}
	else
	{
		KillTimer(1);
		KillTimer(2);
		KillTimer(3);
		pollingActivo = false;
		EscribirLog("Polling detenido");

		CString nuevoJson;
		nuevoJson.Format("{\"rpm\": %d, \"temp\": %d, \"freno\": \"OFF\", \"izq\": \"OFF\", \"der\": \"OFF\"}",
			pintar_revoluciones, pintar_temp);
		lastJson = nuevoJson;
		lastUpdate = COleDateTime::GetCurrentTime();



		cont = 0;
		cont1 = 0;
		cont2 = 0;

		// Reset de luces visuales
		CClientDC dcIzq(&m_izq), dcDer(&m_der), dcFreno(&m_freno);
		CRect rectIzq, rectDer, rectFreno;
		m_izq.GetClientRect(&rectIzq);
		m_der.GetClientRect(&rectDer);
		m_freno.GetClientRect(&rectFreno);

		dcIzq.FillSolidRect(&rectIzq, RGB(128, 128, 128));
		dcDer.FillSolidRect(&rectDer, RGB(128, 128, 128));
		dcFreno.FillSolidRect(&rectFreno, RGB(192, 192, 192));

		// Reset de estados
		estadoFreno = false;
		intermitenteIzq = false;
		intermitenteDer = false;
		intermitenteIzq_activo = false;
		intermitenteDer_activo = false;
		estadoParpadeoIzq = false;
		estadoParpadeoDer = false;

		// Apagar luces del servidor (501, 502, 503, 504, 500)
		int addrs[] = { 500, 501, 502, 503, 504 };
		for (int i = 0; i < 5; ++i) {
			CSocket sock;
			if (!sock.Create()) {
				MessageBox(_T("Error creación socket para apagar luces de Servidor_Luces"));
				return;
			}
			if (!sock.Connect(m_ip, m_port)) {
				MessageBox(_T("No conecta con el esclavo Servidor_Luces"));
				sock.Close();
				return;
			}
			unsigned char buf[20];
			buf[0] = Trans / 256;
			buf[1] = Trans++ % 256;
			buf[2] = 0; buf[3] = 0;
			buf[4] = 0; buf[5] = 6;
			buf[6] = 0x15;
			buf[7] = 0x06;
			buf[8] = addrs[i] / 256;
			buf[9] = addrs[i] % 256;
			buf[10] = 0;  // apagar
			buf[11] = 0;

			sock.Send(buf, 12);
			unsigned char resp[20];
			sock.Receive(resp, 20);
			sock.Close();
		}
		EscribirLog("Luces y estados apagados");
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

		if (start2 == 1 && start == 1)
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

		CDialogEx::OnTimer(nIDEvent);
		return;
	}
	if (nIDEvent == 2) {
		estadoParpadeoIzq = !estadoParpadeoIzq;

		COLORREF color;
		if (estadoParpadeoIzq)
			color = RGB(255, 165, 0);  // Naranja
		else
			color = RGB(128, 128, 128);  // Gris

		CClientDC dc(&m_izq);
		CRect rect;
		m_izq.GetClientRect(&rect);
		dc.FillSolidRect(rect, color);

		// --- Enviar parpadeo a servidor_luces (501 y 503) ---
		int addrs[] = { 501, 503 };
		for (int i = 0; i < 2; ++i) {
			CSocket sockLuz;
			if (!sockLuz.Create()) {
				MessageBox(_T("Error al crear el socket a servidor_luces"));
				return;
			}
			if (!sockLuz.Connect(m_ip, m_port)) {
				MessageBox(_T("No conecta con el servidor_luces para parpadeo IZQ"));
				return;
			}
			unsigned char buf[20];
			buf[0] = Trans / 256;
			buf[1] = Trans++ % 256;
			buf[2] = 0; buf[3] = 0;
			buf[4] = 0; buf[5] = 6;
			buf[6] = 0x15;
			buf[7] = 0x06; // Write Single Register
			buf[8] = addrs[i] / 256;
			buf[9] = addrs[i] % 256;
			buf[10] = 0;
			buf[11] = estadoParpadeoIzq ? 1 : 0;

			sockLuz.Send(buf, 12);
			unsigned char resp[20];
			sockLuz.Receive(resp, 20);
			sockLuz.Close();
		}
		return;
	}
	if (nIDEvent == 3) {
		estadoParpadeoDer = !estadoParpadeoDer;

		COLORREF color;
		if (estadoParpadeoDer)
			color = RGB(255, 165, 0);  // Naranja
		else
			color = RGB(128, 128, 128);  // Gris

		CClientDC dc(&m_der);
		CRect rect;
		m_der.GetClientRect(&rect);
		dc.FillSolidRect(rect, color);

		int addrs[] = { 502,504 };
		for (int i = 0; i < 2; ++i) {
			CSocket sockLuz;
			if (!sockLuz.Create()) {
				MessageBox(_T("Error al crear el socket a servidor_luces"));
				return;
			}
			if (!sockLuz.Connect(m_ip, m_port)) {
				MessageBox(_T("No conecta con el servidor_luces para parpadeo DER"));
				return;
			}
			unsigned char buf[20];
			buf[0] = Trans / 256;
			buf[1] = Trans++ % 256;
			buf[2] = 0; buf[3] = 0;
			buf[4] = 0; buf[5] = 6;
			buf[6] = 0x15;
			buf[7] = 0x06; // Write Single Register
			buf[8] = addrs[i] / 256;
			buf[9] = addrs[i] % 256;
			buf[10] = 0;
			buf[11] = estadoParpadeoDer ? 1 : 0;

			sockLuz.Send(buf, 12);
			unsigned char resp[20];
			sockLuz.Receive(resp, 20);
			sockLuz.Close();
		}
		return;
	}
}

bool CClienteDlg::StartLuces() {
	UpdateData();
	// Intento crear el Socket con el Slave
	CSocket sockLuz;
	if (!sockLuz.Create()) {
		MessageBox(_T("Error al crear el socket a servidor_luces"));
		return 0;
	}
	if (!sockLuz.Connect(m_ip, m_port)) {
		MessageBox(_T("No conecta con el servidor_luces"));
		sockLuz.Close();
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
	sockLuz.Send(buf, 20);

	// Verifico que me responde
	unsigned char resp[20];
	sockLuz.Receive(resp, 20);
	short TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox(_T("Respuesta no recibida.."));
	}

	// Cierro el socket
	sockLuz.Close();

	// leo encendido de respuesta, encendido es un booleano
	start = resp[10] * 256 + resp[11];

	return start;
}

bool CClienteDlg::StartAccionador() {
	UpdateData();
	// Intento crear el Socket con el Slave
	CSocket sockAcc;
	if (!sockAcc.Create()) {
		MessageBox(_T("Error al crear el socket con el esclavo Accionamiento"));
		return 0;
	}
	if (!sockAcc.Connect(m_ipacc, m_portacc)) {
		MessageBox(_T("Fallo en conexion a esclavo Accionamientos"));
		sockAcc.Close();
		start2 = 0;
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
	sockAcc.Send(buf, 20);

	// Verifico que me responde
	unsigned char resp[20];
	sockAcc.Receive(resp, 20);
	short TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox(_T("Respuesta no recibida.."));
	}

	// Cierro el socket
	sockAcc.Close();

	// leo encendido de respuesta, encendido es un booleano
	start2 = resp[10] * 256 + resp[11];

	if (start2 == 0) {
		KillTimer(2);
		KillTimer(3);

		intermitenteIzq_activo = false;
		intermitenteDer_activo = false;
		intermitenteIzq = false;
		intermitenteDer = false;

		// Apagar visuales
		CClientDC dcIzq(&m_izq), dcDer(&m_der);
		CRect rectIzq, rectDer;
		m_izq.GetClientRect(&rectIzq);
		m_der.GetClientRect(&rectDer);
		dcIzq.FillSolidRect(&rectIzq, RGB(128, 128, 128));
		dcDer.FillSolidRect(&rectDer, RGB(128, 128, 128));
	}

	return start2;
}

bool CClienteDlg::StartMotor() {

	UpdateData();
	// Intento crear el Socket con el Slave
	CSocket sockMot;
	if (!sockMot.Create()) {
		MessageBox(_T("Error al crear socket con Motor"));
		return 0;
	}
	if (!sockMot.Connect(m_ipmot, m_portmot)) {
		MessageBox(_T("Fallo al conectar con Socket motor"));
		sockMot.Close();
		start3 = 0;
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
	sockMot.Send(buf, 20);

	// Verifico que me responde
	unsigned char resp[20];
	sockMot.Receive(resp, 20);
	short TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox(_T("Respuesta no recibida.."));
	}

	// Cierro el socket
	sockMot.Close();

	// leo encendido de respuesta, encendido es un booleano
	start3 = resp[10] * 256 + resp[11];


	return start3;
}


//-------------------------------------------------------------------------------
void CClienteDlg::PollingLuces_Freno()
{
	UpdateData();
	// Intento crear el Socket con el Slave
	CSocket sockAcc;
	if (!sockAcc.Create()) {
		MessageBox(_T("Error al crear el socket con Accionamientos para freno"));
		return;
	}
	if (!sockAcc.Connect(m_ipacc, m_portacc)) {
		MessageBox(_T("Fallo en creacion del socket a Accionamientos para el freno"));
		sockAcc.Close();
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
	sockAcc.Send(buf, 20);

	// Verifico que me responde
	unsigned char resp[20];
	sockAcc.Receive(resp, 20);
	short TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox("Respuesta no recibida..");
	}

	// Cierro el socket
	sockAcc.Close();

	// leo estadoFreno de respuesta, estadoFreno es un booleano

	estadoFreno = resp[10] * 256 + resp[11];

	//-------------------------------------------------------------------------------
	// No hace falta definir el mismo nombre otra vez que da error
	CSocket sockLuzFreno;
	if (!sockLuzFreno.Create()) {
		MessageBox(_T("Error al crear el socket con Accionamientos para freno"));
		return;
	}
	if (!sockLuzFreno.Connect(m_ip, m_port)) {
		MessageBox(_T("Fallo en creacion del socket a Accionamientos para el freno"));
		sockLuzFreno.Close();
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
	sockLuzFreno.Send(buf, 20);

	// Verifico que me responde
	// unsigned char resp[20];
	sockLuzFreno.Receive(resp, 20);
	// short  // no la vuelvo a redefinir
	TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox("Respuesta no recibida..");
	}

	// Cierro el socket
	sockLuzFreno.Close();


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

void CClienteDlg::PollingLuces_Int_Izq()
{
	UpdateData();

	// Leer valor del accionador (registro 401)
	CSocket sockAcc;
	if (!sockAcc.Create()) {
		MessageBox(_T("Error al crear el socket con Accionamientos para intermitente IZQ"));
		return;
	}
	if (!sockAcc.Connect(m_ipacc, m_portacc)) {
		MessageBox(_T("Fallo en creacion del socket a Accionamientos para el intermitente IZQ"));
		sockAcc.Close();
		return;
	}
	
	// Solicitud Modbus: leer un registro
	unsigned char buf[20];
	buf[0] = Trans / 256;
	buf[1] = Trans++ % 256;
	buf[2] = 0; buf[3] = 0;
	buf[4] = 0; buf[5] = 6;
	buf[6] = 0x15; // Unit ID
	buf[7] = 0x04; // Function code: Read input register
	buf[8] = 401 / 256;
	buf[9] = 401 % 256;
	buf[10] = 0;
	buf[11] = 1;

	UpdateData(FALSE);
	sockAcc.Send(buf, 12);

	unsigned char resp[20];
	sockAcc.Receive(resp, 20);
	sockAcc.Close();

	short TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		EscribirLog(_T("Respuesta inválida desde accionador IZQ"));
		return;
	}

	int valorLeido = resp[10] * 256 + resp[11];
	bool activo = (valorLeido != 0);
	intermitenteIzq = activo;  // Actualizar estado global

	// Estado anterior estático
	bool previoActivo = intermitenteIzq_activo;

	if (start2 == 0) {
		previoActivo = activo;
		return;
	}

	// Detectar cambio de estado
	if (activo != previoActivo) {
		if (activo) {
			SetTimer(2, 500, NULL);
			intermitenteIzq_activo = true;
			EscribirLog(_T("Intermitente IZQ ACTIVADO"));
		}
		else {
			KillTimer(2);
			intermitenteIzq_activo = false;
			EscribirLog(_T("Intermitente IZQ APAGADO"));

			// Apagar visual
			CClientDC dc(&m_izq);
			CRect rect;
			m_izq.GetClientRect(&rect);
			dc.FillSolidRect(&rect, RGB(128, 128, 128)); // gris

			int addrs[] = { 501, 503 };  // luces izq del y tras
			for (int i = 0; i < 2; ++i) {
				CSocket sockLuz;
				if (!sockLuz.Create()) continue;
				if (!sockLuz.Connect(m_ip, m_port)) {
					sockLuz.Close(); continue;
				}
				buf[0] = Trans / 256;
				buf[1] = Trans++ % 256;
				buf[2] = 0; buf[3] = 0;
				buf[4] = 0; buf[5] = 6;
				buf[6] = 0x15;
				buf[7] = 0x06;
				buf[8] = addrs[i] / 256;
				buf[9] = addrs[i] % 256;
				buf[10] = 0;
				buf[11] = 0;  // apagar

				sockLuz.Send(buf, 12);
				sockLuz.Receive(resp, 20);
				sockLuz.Close();
			}

		}

		previoActivo = activo;
	}
}



void CClienteDlg::PollingLuces_Int_Der() {
	UpdateData();
	CSocket sockAcc;
	if (!sockAcc.Create()) {
		MessageBox(_T("Error al crear el socket con Accionamientos para intermitente DER"));
		return;
	}
	if (!sockAcc.Connect(m_ipacc, m_portacc)) {
		MessageBox(_T("Fallo en creacion del socket a Accionamientos para el intermitente DER"));
		sockAcc.Close();
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
	sockAcc.Send(buf, 20);

	unsigned char resp[20];
	sockAcc.Receive(resp, 20);
	short TransResp = resp[0] * 256 + resp[1] + 1;
	if (TransResp != Trans) {
		MessageBox(_T("Respuesta no recibida.."));
	}

	int valorLeidoDer = resp[10] * 256 + resp[11];
	bool activo = (valorLeidoDer != 0);
	intermitenteDer = activo;

	bool previoActivo = intermitenteDer_activo;

	if (start2 == 0) {
		previoActivo = activo;
		return;
	}

	if (activo != previoActivo) {
		if (activo) {
			SetTimer(3, 500, NULL);
			intermitenteDer_activo = true;
			EscribirLog(_T("Intermitente DER ACTIVADO"));
		}
		else {
			KillTimer(3);
			intermitenteDer_activo = false;
			EscribirLog(_T("Intermitente DER APAGADO"));

			CClientDC dc(&m_der);
			CRect rect;
			m_der.GetClientRect(&rect);
			dc.FillSolidRect(&rect, RGB(128, 128, 128));

			int addrs[] = { 502, 504 };  // luces izq del y tras
			for (int i = 0; i < 2; ++i) {
				CSocket sockLuz;
				if (!sockLuz.Create()) continue;
				if (!sockLuz.Connect(m_ip, m_port)) {
					sockLuz.Close(); continue;
				}
				buf[0] = Trans / 256;
				buf[1] = Trans++ % 256;
				buf[2] = 0; buf[3] = 0;
				buf[4] = 0; buf[5] = 6;
				buf[6] = 0x15;
				buf[7] = 0x06;
				buf[8] = addrs[i] / 256;
				buf[9] = addrs[i] % 256;
				buf[10] = 0;
				buf[11] = 0;  // apagar

				sockLuz.Send(buf, 12);
				sockLuz.Receive(resp, 20);
				sockLuz.Close();
			}
		}
		previoActivo = activo;
	}
}


int CClienteDlg::PollingMotor() {

	UpdateData();
	// Intento crear el Socket con el Slave
	CSocket sockMot;
	if (!sockMot.Create()) {
		MessageBox(_T("Error al crear socket con Motor para cambios en TEMP"));
		return temperatura;
	}
	if (!sockMot.Connect(m_ipmot, m_portmot)) {
		MessageBox(_T("Fallo en conexion con socket Motor para cambios en TEMP"));
		sockMot.Close();
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
	sockMot.Send(buf, 20);

	// Verifico que me responde
	unsigned char resp[20];
	sockMot.Receive(resp, 20);

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
	sockMot.Close();

	UpdateData();
	// Intento crear el Socket con el Slave
	if (!sockMot.Create()) {
		MessageBox(_T("Error al crear socket con Motor para cambios en RPM"));
		return revoluciones;
	}
	if (!sockMot.Connect(m_ipmot, m_portmot)) {
		MessageBox(_T("Fallo en conexion con socket Motor para cambios en RPM"));
		sockMot.Close();
		return revoluciones;
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
	sockMot.Send(buf, 20);

	// Verifico que me responde
	sockMot.Receive(resp, 20);

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
	sockMot.Close();

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
	timestamp.Format("[%02d:%02d:%02d] %s \r\n", now.GetHour(), now.GetMinute(), now.GetSecond(), texto);

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

CString CClienteDlg::getPage()
{
	EscribirLog("getPage llamada");
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



UINT CClienteDlg::ThreadWebServer(LPVOID pParam)
{
	CClienteDlg* pDlg = (CClienteDlg*)pParam;
	int port = 8088;

	CWebSocket listenSocket;
	listenSocket.Create(port);
	listenSocket.Listen();
	listenSocket.m_pDlg = pDlg;

	CWebSocket clientSocket;

	while (true) {
		if (listenSocket.Accept(clientSocket)) {
			clientSocket.m_pDlg = pDlg;

			char bufweb[10240];
			int len = clientSocket.Receive(bufweb, sizeof(bufweb) - 1);
			if (len <= 0) {
				clientSocket.Close();
				continue;
			}

			bufweb[len] = 0;
			CString msg = pDlg->GetMsg(bufweb, len);
			CString response, header;

			if (msg.Find("/estado") != -1) {
				header = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
				response = pDlg->lastJson;
			}
			else {
				header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
				response = pDlg->getPage();
			}

			clientSocket.Send((LPCTSTR)header, header.GetLength());
			clientSocket.Send((LPCTSTR)response, response.GetLength());
			clientSocket.Close();
		}
		else {
			Sleep(100); // Evita CPU 100%
		}
	}
	return 0;
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
	EscribirLog(_T("Conexión cerrada por el cliente"));
	CDialogEx::OnClose();
}

