
// Servidor_LucesDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Servidor_Luces.h"
#include "Servidor_LucesDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool encendido = 0;
bool enc_led_arr_izq = 0;
bool enc_led_arr_der = 0;
bool enc_led_deb_izq = 0;
bool enc_led_deb_der = 0;



// CServidorLucesDlg dialog



CServidorLucesDlg::CServidorLucesDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVIDOR_LUCES_DIALOG, pParent)
	, m_port(_T(""))
	, m_testo_dir_freno_nada(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	dir_led_1 = 0;
	dir_led_arr_izq = 0;
	dir_led_arr_der = 0;
	dir_led_deb_izq = 0;
	dir_led_deb_der = 0;
}

void CServidorLucesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LED_1, m_led_1);
	DDX_Control(pDX, IDC_LED_ARR_DER, m_led_arr_der);
	DDX_Control(pDX, IDC_LED_ARR_IZQ, m_led_arr_izq);
	DDX_Control(pDX, IDC_LED_DEB_DERR, m_led_deb_der);
	DDX_Control(pDX, IDC_LED_DEB_IZQ, m_led_deb_izq);
	DDX_Text(pDX, IDC_PORT, m_port);
	DDX_Text(pDX, IDC_FRENO_ADDRES, m_testo_dir_freno_nada);
}

BEGIN_MESSAGE_MAP(CServidorLucesDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CServidorLucesDlg message handlers

BOOL CServidorLucesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	misoc = new CMySocket();
	bool ret = misoc->Create(3510, SOCK_STREAM);
	if (!ret) MessageBox("Error al crear el socket");
	ret = misoc->Listen();
	if (!ret) MessageBox("Error al quedar a la escucha...");

	CBrush m_brush;
	m_testo_dir_freno_nada.Format(_T("Freno. Dir %d"), dir_led_1);

	UpdateData(0);

	SetWindowText(_T("Esclavo ModBus"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServidorLucesDlg::OnPaint()
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
HCURSOR CServidorLucesDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMySocket::OnAccept(int err)
{
	CString cs, cs1;
	UINT port;
	CSocket client;
	Accept(client);  // MySocket acepta el Maestro
	//client.GetSockName(cs, port);
	int id, dir, value;
	unsigned char buf[20];
	while (true) {
		int len = client.Receive(buf, 20);
		if (len == 0 || len == -1) break;  // fin 1: error, 0: cuando desconecta
		else {
			id = buf[6];
			if (id == 0x15) {
				dir = buf[8] * 256;
				dir += buf[9];
				value = buf[10] * 256;
				value += buf[11];
				CServidorLucesDlg* pDlg = (CServidorLucesDlg*)AfxGetMainWnd();
				
				
				pDlg->UpdateData();
				if (dir == 500) {
					CDC* pdc = pDlg->m_led_1.GetDC();
					CRect r;
					pDlg->m_led_1.GetClientRect(r);
					CBrush red(RGB(255, 0, 0));
					CBrush grey(RGB(128, 128, 128));
					if (buf[11] == 1 && encendido == 0) {
						encendido = 1;
						pdc->FillRect(r, &red);
					}
					else if (buf[11] == 0 && encendido == 1) {
						encendido = 0;
						pdc->FillRect(r, &grey);
					}
				}
				else if (dir == 501) {
					CDC* pdc = pDlg->m_led_arr_izq.GetDC();
					CRect r;
					pDlg->m_led_arr_izq.GetClientRect(r);
					CBrush orange(RGB(255, 165, 0));
					CBrush grey(RGB(128, 128, 128));
					if (buf[11] == 1 && enc_led_arr_izq == 0) {
						enc_led_arr_izq = 1;
						pdc->FillRect(r, &orange);
					}
					else if (buf[11] == 0 && enc_led_arr_izq == 1) {
						enc_led_arr_izq = 0;
						pdc->FillRect(r, &grey);
					}
				}
				else if (dir == 502) {
					CDC* pdc = pDlg->m_led_arr_der.GetDC();
					CRect r;
					pDlg->m_led_arr_der.GetClientRect(r);
					CBrush orange(RGB(255, 165, 0));
					CBrush grey(RGB(128, 128, 128));
					if (buf[11] == 1 && enc_led_arr_der == 0) {
						enc_led_arr_der = 1;
						pdc->FillRect(r, &orange);
					}
					else if (buf[11] == 0 && enc_led_arr_der == 1) {
						enc_led_arr_der = 0;
						pdc->FillRect(r, &grey);
					}
				}
				else if (dir == 503) {
					CDC* pdc = pDlg->m_led_deb_izq.GetDC();
					CRect r;
					pDlg->m_led_deb_izq.GetClientRect(r);
					CBrush red(RGB(255, 0, 0));
					CBrush grey(RGB(128, 128, 128));
					if (buf[11] == 1 && enc_led_deb_izq == 0) {
						enc_led_deb_izq = 1;
						pdc->FillRect(r, &red);
					}
					else if (buf[11] == 0 && enc_led_deb_izq == 1) {
						enc_led_deb


				}
				client.Send(buf, 20);
			}
		}
	}
	client.Close();
}



