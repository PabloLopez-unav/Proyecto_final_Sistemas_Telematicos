#pragma once
#include <afxsock.h>

class CClienteDlg;  

class CWebSocket : public CSocket
{
public:
    CWebSocket();
    virtual ~CWebSocket();

    CClienteDlg* m_pDlg; 
};
