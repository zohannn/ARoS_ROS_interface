
// ARoS_ros_interface.cpp : Defines the class behaviors for the application.
//

#include <iostream>
#include "../include/stdafx.h"
#include "../include/ARoS_ros_interface.h"
#include "../include/ARoS_ros_interfaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CARoS_ros_interfaceApp

BEGIN_MESSAGE_MAP(CARoS_ros_interfaceApp, CWinApp)
	//ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
	ON_COMMAND(ID_ABOUT_AROSINTERFACE, &CARoS_ros_interfaceApp::OnAboutArosinterface)
	ON_COMMAND(ID_ROSCONNECT_GGG, &CARoS_ros_interfaceApp::OnRosconnectGgg)
END_MESSAGE_MAP()


// CARoS_ros_interfaceApp construction

CARoS_ros_interfaceApp::CARoS_ros_interfaceApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CARoS_ros_interfaceApp object

CARoS_ros_interfaceApp theApp;


// CARoS_ros_interfaceApp initialization

BOOL CARoS_ros_interfaceApp::InitInstance()
{
	CWinApp::InitInstance();


	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CARoS_ros_interfaceDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}



void CARoS_ros_interfaceApp::OnAboutArosinterface()
{
	CAbout_AROS_ros_dlg dlg;
	dlg.DoModal();
}


void CARoS_ros_interfaceApp::OnRosconnectGgg()
{
    CROS_Comm_dlg dlg;
	dlg.DoModal();
	//for(int i=0;i<5;i++){
	//theApp.m_pThread =AfxBeginThread(RUNTIME_CLASS(CNode));
	//theApp.m_pThread = AfxBeginThread(RUNTIME_CLASS(CNode));
	//}
	/*
	ros_node = new CNode();
	ros_node->CreateThread(CREATE_SUSPENDED);
	ros_node->m_bAutoDelete=false; // don't delete it automatically
	ros_node->setNodeName(_T("aros"));


	WaitForSingleObject(ros_node->m_hThread,INFINITE);
	delete ros_node;
	*/

}





