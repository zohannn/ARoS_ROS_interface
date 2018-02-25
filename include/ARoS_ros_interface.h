
// ARoS_ros_interface.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "About_AROS_ros_interface.h"
#include "ROS_Comm_dlg.h"
#include "Node.h"
#include "ARoS_ros_interfaceDlg.h"

// CARoS_ros_interfaceApp:
// See ARoS_ros_interface.cpp for the implementation of this class
//

class CARoS_ros_interfaceApp : public CWinApp
{
public:
	CARoS_ros_interfaceApp();
	~CARoS_ros_interfaceApp();

// Overrides
public:
	virtual BOOL InitInstance();
	//virtual BOOL ExitInstance();
	//CWinThread* m_pThread;
	CNode* ros_node; // ros node of the network
	CROS_Comm_dlg ros_comm_dlg; // ros communication dialog
	CAbout_AROS_ros_dlg about_dlg; // about dialog
	bool b_connected; // true if the node is connected to the ROS master, false otherwise
	bool b_env_vars; // true if environment variables have been used to connect to ROS
	CString ros_master; // ROS MASTER URI
	CString ros_ip; // ROS IP
	// Main dialog
	CARoS_ros_interfaceDlg main_dlg;

// Implementation

	DECLARE_MESSAGE_MAP()
	afx_msg void OnAboutArosinterface();
	afx_msg void OnRosconnectGgg();
	
};

extern CARoS_ros_interfaceApp theApp;