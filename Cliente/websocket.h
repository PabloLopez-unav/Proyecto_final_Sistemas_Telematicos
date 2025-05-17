#pragma once
#include <afxsock.h>

class CClienteDlg;  // Declaración adelantada

class CWebSocket : public CSocket
{
public:
    CWebSocket();
    virtual ~CWebSocket();

    CClienteDlg* m_pDlg;  // Referencia al diálogo, si la necesitas para log o UI
};
