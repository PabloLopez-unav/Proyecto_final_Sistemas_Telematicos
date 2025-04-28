
// AccionamientosDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Accionamientos.h"
#include "AccionamientosDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool encendido = 0;
bool freno = 0;
bool int_izq = 0;
bool int_der = 0;

bool start = 0;

// CAccionamientosDlg dialog



CAccionamientosDlg::CAccionamientosDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ACCIONAMIENTOS_DIALOG, pParent)
	, m_port(_T(""))
	, m_boton_freno(FALSE)
	, m_boton_int_izq(FALSE)
	, m_boton_int_der(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	dir_freno = 0;
	dir_int_der = 0;
	dir_int_izq = 0;
}

void CAccionamientosDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PORT, m_port);
	DDX_Check(pDX, IDC_BOTON_FRENO, m_boton_freno);
	DDX_Check(pDX, IDC_BOT_INT_IZQ, m_boton_int_izq);
	DDX_Check(pDX, IDC_BOT_INT_DER, m_boton_int_der);
}

BEGIN_MESSAGE_MAP(CAccionamientosDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CAccionamientosDlg::OnBnClickedStart)
END_MESSAGE_MAP()


// CAccionamientosDlg message handlers

BOOL CAccionamientosDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	misoc = new CMySocket();
	bool ret = misoc->Create(5021, SOCK_STREAM);
	if (!ret) MessageBox("Error al crear el socket");
	ret = misoc->Listen();
	if (!ret) MessageBox("Error al quedar a la escucha...");

	CBrush m_brush;
	m_port.Format(_T("5021"));

	UpdateData(0);

	SetWindowText(_T("Accionamientos"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAccionamientosDlg::OnPaint()
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
HCURSOR CAccionamientosDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMySocket::OnAccept(int err)
{
	
	CString cs, cs1;
	CSocket client;
	Accept(client);  // MySocket acepta el Maestro
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
				CAccionamientosDlg* pDlg = (CAccionamientosDlg*)AfxGetMainWnd();
				pDlg->UpdateData();

				if (dir == 400) {
					buf[11] = pDlg->m_boton_freno;
				}
				else if (dir == 401) {
					buf[11] = pDlg->m_boton_int_izq;
				}
				else if (dir == 402) {
					buf[11] = pDlg->m_boton_int_der;
				}
				else if (dir == 570) {
					buf[11] = start;
				}
				client.Send(buf, 20);
			}
		}
	}
	client.Close();
	
	
}
void CAccionamientosDlg::OnBnClickedStart()
{
	start = !start;
}
