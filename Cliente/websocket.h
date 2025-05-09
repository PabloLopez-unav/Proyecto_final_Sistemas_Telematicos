#pragma once

class CClienteDlg; // Forward declaration

class CWebSocket : public CAsyncSocket
{
public:
    CWebSocket();
    virtual ~CWebSocket();

    CClienteDlg* m_pDlg = nullptr; // Aquí se enlaza con el diálogo

    virtual void OnAccept(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
    virtual void OnClose(int nErrorCode);
};

