#pragma once



// CNode

class CNode : public CWinThread
{
	DECLARE_DYNCREATE(CNode)

protected:
	CNode();           // costruttore protetto utilizzato dalla creazione dinamica
	virtual ~CNode();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};


