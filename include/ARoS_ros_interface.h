
// ARoS_ros_interface.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "About_AROS_ros_interface.h"
#include "ROS_Comm_dlg.h"

// CARoS_ros_interfaceApp:
// See ARoS_ros_interface.cpp for the implementation of this class
//

class CARoS_ros_interfaceApp : public CWinApp
{
public:
	CARoS_ros_interfaceApp();

// Overrides
public:
	virtual BOOL InitInstance();
	CWinThread* m_pThread[5];

// Implementation

	DECLARE_MESSAGE_MAP()
	afx_msg void OnAboutArosinterface();
	afx_msg void OnRosconnectGgg();
	
};

extern CARoS_ros_interfaceApp theApp;