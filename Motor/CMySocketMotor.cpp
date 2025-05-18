// MySocket.cpp : implementation file
//
#include "pch.h"
#include "Motor.h"
#include "CMySocketMotor.h"
#include "MotorDlg.h"

// CMySocket

CMySocketMotor::CMySocketMotor(CMotorDlg* p)
{
	pDlg = p;
}
CMySocketMotor::CMySocketMotor()
{
	pDlg = nullptr;
}
CMySocketMotor::~CMySocketMotor()
{
}

// CMySocket member functions
void CMySocketMotor::OnAccept(int err)
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
				CMotorDlg* pDlg = (CMotorDlg*)AfxGetMainWnd();
				pDlg->UpdateData();

				if (dir == 400) {
					buf[10] = pDlg->m_slider_temp_val / 256;
					buf[11] = pDlg->m_slider_temp_val % 256;
				}
				else if (dir == 401) {
					buf[10] = pDlg->m_slider_rpm_val / 256;
					buf[11] = pDlg->m_slider_rpm_val % 256;
				}
				else if (dir == 570) {
					buf[11] = pDlg->start;
				}
				client.Send(buf, 20);
			}
		}
	}
	client.Close();
}
