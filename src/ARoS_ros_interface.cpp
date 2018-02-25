
// ARoS_ros_interface.cpp : Defines the class behaviors for the application.
//

#include "../include/stdafx.h"
#include "../include/ARoS_ros_interface.h"

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
	ros_node = new CNode();
	ros_node->CreateThread(CREATE_SUSPENDED);
	ros_node->m_bAutoDelete=false;
	//ros_node = (CNode*)AfxBeginThread(RUNTIME_CLASS(CNode));

	ros_comm_dlg.setROSNode(ros_node);

	b_connected = false;
	b_env_vars = true;
	ros_master = _T("Enter the ROS_MASTER_URI");
	ros_ip = _T("IP address of this PC");

}

CARoS_ros_interfaceApp::~CARoS_ros_interfaceApp()
{

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

	m_pMainWnd = &main_dlg;
	INT_PTR nResponse = main_dlg.DoModal();
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

	if(ros_node!=NULL){
		delete(ros_node);
		//delete ros_node;
		//ros_node = NULL;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

/*
BOOL CARoS_ros_interfaceApp::ExitInstance()
{
	CWinApp::ExitInstance();

	return FALSE;

}
*/



void CARoS_ros_interfaceApp::OnAboutArosinterface()
{
	
	about_dlg.DoModal();
}


void CARoS_ros_interfaceApp::OnRosconnectGgg()
{
	if(b_connected){
		ros_comm_dlg.strStatus=_T("connected");
		ros_comm_dlg.strButtonConnect=_T("Disconnect");
	}else{
		ros_comm_dlg.strStatus=_T("disconnected");
		ros_comm_dlg.strButtonConnect=_T("Connect");
	}
	ros_comm_dlg.setEnvVars(b_env_vars);
	ros_comm_dlg.setMasterURI(ros_master);
	ros_comm_dlg.setROSIP(ros_ip);
	ros_comm_dlg.DoModal();
	b_connected = ros_comm_dlg.getROSConn();
	b_env_vars = ros_comm_dlg.getEnvVars();
	ros_master = ros_comm_dlg.getMasterURI();
	ros_ip =ros_comm_dlg.getROSIP();

	if(b_connected)
	{
		main_dlg.m_log_list.AddString(_T("Node is connected to the ROS Master"));
	}else
	{
		main_dlg.m_log_list.AddString(_T("Node is NOT connected to the ROS Master"));
	}
}





