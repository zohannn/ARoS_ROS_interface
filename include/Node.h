#pragma once


#include <ros\ros.h>
#include <ros\network.h> 

// CNode

class CNode : public CWinThread
{
	DECLARE_DYNCREATE(CNode)

//protected:
public:
	CNode();           
	virtual ~CNode();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	bool on_init();
	bool on_init(CString master,CString ip);
	bool on_end();
	int Run();
	CString getNodeName();
	void setNodeName(CString name);

protected:
	const char * WinGetEnv(const char * name);
	DECLARE_MESSAGE_MAP()

private:
	CString node_name;

};


