#pragma once

class CClienteDlg; // Forward declaration

class CWebSocket : public CAsyncSocket
{
public:
    CWebSocket();
    virtual ~CWebSocket();

    CClienteDlg* m_pDlg = nullptr; // Aqu� se enlaza con el di�logo

    virtual void OnAccept(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
    virtual void OnClose(int nErrorCode);
};

