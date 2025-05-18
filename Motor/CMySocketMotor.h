#pragma once
class CMotorDlg;

class CMySocketMotor : public CSocket
{
public:
	CMySocketMotor(CMotorDlg* pDlg);
	CMySocketMotor();

	CMotorDlg* pDlg;

	virtual ~CMySocketMotor();
	virtual void OnAccept(int nErrorCode);
};