#pragma once
#include <afxsock.h>

class CClienteDlg;  // Declaraci�n adelantada

class CWebSocket : public CSocket
{
public:
    CWebSocket();
    virtual ~CWebSocket();

    CClienteDlg* m_pDlg;  // Referencia al di�logo, si la necesitas para log o UI
};
