
// ARoS_ros_interface.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif



//Boost
#include <boost\bind.hpp>
#include <boost\shared_ptr.hpp>
#include <yarp/os/Network.h>

#include "resource.h"		// main symbols
#include "About_AROS_ros_interface.h"
#include "ROS_Comm_dlg.h"
#include "Node.h"
//#include "YarpCommInterface.h"
#include "ARoS_ros_interfaceDlg.h"

// CARoS_ros_interfaceApp:
// See ARoS_ros_interface.cpp for the implementation of this class
//

class CARoS_ros_interfaceApp : public CWinApp
{
public:
	CARoS_ros_interfaceApp();
	~CARoS_ros_interfaceApp();

private:
	int upperlimb_mode; //0 = RIGHT UPPER LIMB, 1 = LEFT UPPER LIMB, 2 = BOTH UPPER LIMBS
	std::string str_upperlimb_server; // YARP Upperlimb server name
	std::string str_vision_server; // YARP Vision server name
	std::vector<float> look_home; // home looking position 
	boost::thread update_joints_values_thd; // thread to update the values of the joints
	void  updateUpperLimbValues (); // Thread upperlimb values

// Overrides
public:
	virtual BOOL InitInstance();
	virtual BOOL ExitInstance();
	
	// ROS
	CNode* ros_node; // ros node of the network
	CROS_Comm_dlg ros_comm_dlg; // ros communication dialog
	CAbout_AROS_ros_dlg about_dlg; // about dialog
	bool b_ros_connected; // true if the node is connected to the ROS master, false otherwise
	bool b_env_vars; // true if environment variables have been used to connect to ROS
	CString ros_master; // ROS MASTER URI
	CString ros_ip; // ROS IP

	// YARP modules
	CYarpCommInterface* yarp_upperlimb;  // YARP communication with the upperlimb
	CYarpCommInterface* yarp_vision;  // YARP communication with the vision
	boost::atomic<bool> b_yarp_upperlimb_states;
	boost::atomic<bool> b_yarp_vision_states;

	// Main dialog
	CARoS_ros_interfaceDlg main_dlg;
	// Implementation

	afx_msg void OnAboutArosinterface();
	afx_msg void OnRosConnect();
	afx_msg void OnYarpUpperlimb();	
	afx_msg void OnYarpVision();
	DECLARE_MESSAGE_MAP()
	
};

extern CARoS_ros_interfaceApp theApp;