#include "pch.h"
#include "Cliente.h"
#include "ClienteDlg.h"
#include "websocket.h"


CWebSocket::CWebSocket() {}
CWebSocket::~CWebSocket() {}

void CWebSocket::OnAccept(int nErrorCode)
{
    if (m_pDlg) {
        m_pDlg->OnAccept(); // delega al di�logo
    }
    CAsyncSocket::OnAccept(nErrorCode);
}

void CWebSocket::OnReceive(int nErrorCode)
{
    if (m_pDlg) {
        m_pDlg->OnReceive(); // delega al di�logo
    }
    CAsyncSocket::OnReceive(nErrorCode);
}

void CWebSocket::OnClose(int nErrorCode)
{
    if (m_pDlg) {
        m_pDlg->OnClose(); // delega al di�logo
    }
    CAsyncSocket::OnClose(nErrorCode);
}
