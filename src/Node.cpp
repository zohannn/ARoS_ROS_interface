// Node.cpp : file di implementazione
//

#include "../include/stdafx.h"
#include "../include/ARoS_ROS_interface.h"
#include "../include/Node.h"


// CNode

IMPLEMENT_DYNCREATE(CNode, CWinThread)

CNode::CNode()
{
}

CNode::~CNode()
{
}

BOOL CNode::InitInstance()
{
	//  eseguire l'inizializzazione dei singoli thread
	m_pMainWnd = new CROS_Comm_dlg;
	m_pMainWnd->SetForegroundWindow();
	m_pMainWnd->ShowWindow(SW_SHOW);
	return TRUE;
}

int CNode::ExitInstance()
{
	// TODO:  eseguire la pulitura dei singoli thread
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CNode, CWinThread)
END_MESSAGE_MAP()


// gestori di messaggi CNode
