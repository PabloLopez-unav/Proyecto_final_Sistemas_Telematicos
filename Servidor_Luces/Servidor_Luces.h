
// Servidor_Luces.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CServidorLucesApp:
// See Servidor_Luces.cpp for the implementation of this class
//

class CServidorLucesApp : public CWinApp
{
public:
	CServidorLucesApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CServidorLucesApp theApp;
