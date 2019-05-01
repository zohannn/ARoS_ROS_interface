
// ARoS_ros_interfaceDlg.h : header file
//

#pragma once

#ifndef DEG_TO_RAD_F
#define DEG_TO_RAD_F 0.017453292519943295769236907684886f
#endif

#ifndef RAD_TO_DEG_F
#define RAD_TO_DEG_F 57.295779513082320876798154814105f
#endif


#include "Node.h"
#include "YarpCommInterface.h"
//#include "afxwin.h"


// CARoS_ros_interfaceDlg dialog
class CARoS_ros_interfaceDlg : public CDialogEx
{
// Construction
public:
	CARoS_ros_interfaceDlg(CWnd* pParent = NULL);	// standard constructor
	~CARoS_ros_interfaceDlg();

// Dialog Data
	enum { IDD = IDD_AROS_ROS_INTERFACE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	bool closing; //true if the application is closing
	bool task_loaded; // true if there is a task loaded;
	boost::atomic<bool> start_joint_update;
	boost::thread update_joints_values_thd;
	boost::atomic<bool> start_vision_update;
	boost::thread update_vision_values_thd;


public:
	void updateJointValues();
	void updateVisionValues();
	void updateJointValuesAsync();
	void start_joint_updating(); // start update the joint values
	void stop_joint_updating(); // stop update the joint values
	void start_vision_updating(); // start update the vision info
	void stop_vision_updating(); // stop update the vision info


// Implementation
protected:
	HICON m_hIcon;
	CNode* ros_node; // ROS NODE
	// YARP modules
	CYarpCommInterface* yarp_upperlimb; // YARP communication with the upperlimb
	CYarpCommInterface* yarp_vision; // YARP communication with the vision
	std::vector<std::vector<float>> look_pos; // looking position sequence of the task.

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	// listen /chatter
	afx_msg void OnBnClickedButtonListen();
	// TALK /chatter1
	afx_msg void OnBnClickedButtonTalk();

	// right home joint 0 
	afx_msg void onBnClickedButtonRightHome0();
	// right home joint 1 
	afx_msg void onBnClickedButtonRightHome1();	
	// right home joint 2 
	afx_msg void onBnClickedButtonRightHome2();	
	// right home joint 3 
	afx_msg void onBnClickedButtonRightHome3();	
	// right home joint 4 
	afx_msg void onBnClickedButtonRightHome4();	
	// right home joint 5 
	afx_msg void onBnClickedButtonRightHome5();	
	// right home joint 6 
	afx_msg void onBnClickedButtonRightHome6();
	// right hand init
	afx_msg void onBnClickedButtonRightHandInit();
	// right hand open
	afx_msg void onBnClickedButtonRightHandOpen();
	//right hand open home
	afx_msg void onBnClickedButtonRightHandOpenPark();
	//right hand close
	afx_msg void onBnClickedButtonRightHandClose();
	// home right upperlimb
	afx_msg void onBnClickedButtonRightUpperLimbHome();
	// park right upperlimb
	afx_msg void onBnClickedButtonRightUpperLimbPark();
	// stop right upperlimb
	afx_msg void onBnClickedButtonRightUpperLimbStop();
	// off-line check
	afx_msg void onBnClickedCheckOffline();
	// load task
	afx_msg void onBnClickedLoadTask();
	// execute task
	afx_msg void onBnClickedExecTask();

	DECLARE_MESSAGE_MAP()

public:
	// logging list variable
	CListBox m_log_list;

	// update the joint values
	bool start_update;

	// add a line in loggings
	void addLogLine(CString line);

	// set the ROS node
	void setROSNode(CNode* r);
	// get the ROS node
	CNode* getROSNode();
	// set the YARP communication module of the upperlimb
	void setYARPUpperlimb(CYarpCommInterface* y);
	// get the YARP communication module of the upperlimb
	CYarpCommInterface* getYARPUpperlimb();
	// set the YARP communication module of the vision
	void setYARPVision(CYarpCommInterface* y);
	// get the YARP communication module of the vision
	CYarpCommInterface* getYARPVision();

	// Appereance methods
	void EnableExecModeGroup(bool b);
	void EnableOffLineModeGroup(bool b);
	void EnableRightUpperLimbGroup(bool b); bool right_enabled;
	void EnableRightArmGroup(bool b);
	void EnableRightHandGroup(bool b);
	void EnableLeftUpperLimbGroup(bool b); bool left_enabled;
	void EnableLeftArmGroup(bool b);
	void EnableLeftHandGroup(bool b);
	void EnableVisionTargetGroup(bool b);
	void EnableVisionObstaclesGroup(bool b);
	void EnableVisionGroup(bool b);

	// load a task
	bool loadTask(CString file_name);
	
	//divide the task in micro steps
	void divideTask(unsigned int n_micro_steps);

	// types of movement
	enum movementType
	{
		PICK,
		PLACE,
		MOVE,
	};
	// types of stages
	enum stageType
	{
		PLAN,
		APPROACH,
		RETREAT,
	};
	struct movement{
		movementType mov_type;
		std::vector<stageType> stages;
		std::vector<std::vector<float>> j_pos_traj; // positions + timesteps to get to these positions
		std::vector<std::vector<float>> j_vel_traj; // velocities 
		std::vector<std::vector<float>> j_acc_traj; //accellerations
		std::vector<float> time_steps; // time steps of the trajectory 
		std::vector<int> stage_sizes; // size of each stage
	};
	std::vector<movement> task; // task to execute 
	std::vector<movement> task_micro; // task with micro steps to execute 

	bool execTask(bool pos); // execute the task
};
