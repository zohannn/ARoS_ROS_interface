#pragma once


#include <ros\ros.h>
#include <ros\network.h> 

// CNode

class CNode : public CWinThread
{
	DECLARE_DYNCREATE(CNode)

protected:
//public:
	CNode();           // costruttore protetto utilizzato dalla creazione dinamica
	virtual ~CNode();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	int on_init();
	int Run();
	CString getNodeName();
	void setNodeName(CString name);

protected:
	DECLARE_MESSAGE_MAP()

private:
	CString node_name;

};


