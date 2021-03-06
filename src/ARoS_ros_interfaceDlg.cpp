
// ARoS_ros_interfaceDlg.cpp : implementation file
//


#include "../include/stdafx.h"
#include "../include/ARoS_ros_interface.h"
#include "../include/ARoS_ros_interfaceDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CARoS_ros_interfaceDlg dialog


CARoS_ros_interfaceDlg::CARoS_ros_interfaceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CARoS_ros_interfaceDlg::IDD, pParent)
{
#ifndef _DEBUG
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); // debug assertation failed!
#endif

}

CARoS_ros_interfaceDlg::~CARoS_ros_interfaceDlg()
{
	this->stop_joint_updating();
	this->stop_vision_updating();
}

void CARoS_ros_interfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_log_list);
}

BEGIN_MESSAGE_MAP(CARoS_ros_interfaceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_BN_CLICKED(IDC_BUTTON_LISTEN, &CARoS_ros_interfaceDlg::OnBnClickedButtonListen)
	//ON_BN_CLICKED(IDC_BUTTON_TALK, &CARoS_ros_interfaceDlg::OnBnClickedButtonTalk)
	ON_BN_CLICKED(IDC_BUTTON_HOME_RIGHT_JOINT0, &CARoS_ros_interfaceDlg::onBnClickedButtonRightHome0)
	ON_BN_CLICKED(IDC_BUTTON_HOME_RIGHT_JOINT1, &CARoS_ros_interfaceDlg::onBnClickedButtonRightHome1)
	ON_BN_CLICKED(IDC_BUTTON_HOME_RIGHT_JOINT2, &CARoS_ros_interfaceDlg::onBnClickedButtonRightHome2)
	ON_BN_CLICKED(IDC_BUTTON_HOME_RIGHT_JOINT3, &CARoS_ros_interfaceDlg::onBnClickedButtonRightHome3)
	ON_BN_CLICKED(IDC_BUTTON_HOME_RIGHT_JOINT4, &CARoS_ros_interfaceDlg::onBnClickedButtonRightHome4)
	ON_BN_CLICKED(IDC_BUTTON_HOME_RIGHT_JOINT5, &CARoS_ros_interfaceDlg::onBnClickedButtonRightHome5)
	ON_BN_CLICKED(IDC_BUTTON_HOME_RIGHT_JOINT6, &CARoS_ros_interfaceDlg::onBnClickedButtonRightHome6)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT_HAND_INIT, &CARoS_ros_interfaceDlg::onBnClickedButtonRightHandInit)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT_HAND_OPEN, &CARoS_ros_interfaceDlg::onBnClickedButtonRightHandOpen)	
	ON_BN_CLICKED(IDC_BUTTON_RIGHT_HAND_OPEN_PARK, &CARoS_ros_interfaceDlg::onBnClickedButtonRightHandOpenPark)		
	ON_BN_CLICKED(IDC_BUTTON_RIGHT_HAND_CLOSE, &CARoS_ros_interfaceDlg::onBnClickedButtonRightHandClose)	
	ON_BN_CLICKED(IDC_BUTTON_RIGHT_HOME, &CARoS_ros_interfaceDlg::onBnClickedButtonRightUpperLimbHome)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT_PARK, &CARoS_ros_interfaceDlg::onBnClickedButtonRightUpperLimbPark)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT_STOP, &CARoS_ros_interfaceDlg::onBnClickedButtonRightUpperLimbStop)
	ON_BN_CLICKED(IDC_CHECK_OFFLINE, &CARoS_ros_interfaceDlg::onBnClickedCheckOffline)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_TASK, &CARoS_ros_interfaceDlg::onBnClickedLoadTask)
	ON_BN_CLICKED(IDC_BUTTON_EXEC_TASK, &CARoS_ros_interfaceDlg::onBnClickedExecTask)
END_MESSAGE_MAP()


// CARoS_ros_interfaceDlg message handlers

BOOL CARoS_ros_interfaceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	closing = false;
	task_loaded = false;
	EnableRightUpperLimbGroup(false); right_enabled = false;
	EnableLeftUpperLimbGroup(false); left_enabled = false;
	EnableExecModeGroup(false);
	EnableVisionGroup(false);
	EnableJointsLowPassFilterGroup(false);
	CheckDlgButton(IDC_RADIO_POS,1);
	CheckDlgButton(IDC_CHECK_OFFLINE,1);
	SetDlgItemText(IDC_EDIT_MICRO,_T("1"));
	SetDlgItemText(IDC_EDIT_CUTOFF_FREQ_POS,_T("0.1"));
	SetDlgItemText(IDC_EDIT_DELTA_TIME_POS,_T("1"));
	SetDlgItemText(IDC_EDIT_CUTOFF_FREQ_OR,_T("0.05"));
	SetDlgItemText(IDC_EDIT_DELTA_TIME_OR,_T("1"));
	SetDlgItemText(IDC_EDIT_JOINTS_CUTOFF_FREQ,_T("1"));
	SetDlgItemText(IDC_EDIT_JOINTS_DELTA_TIME,_T("0.05"));
	this->topic_set_joints = "/motion_manager/set_real_joints";

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CARoS_ros_interfaceDlg::EnableExecModeGroup(bool b)
{
	GetDlgItem(IDC_STATIC_EXEC_MODE)->EnableWindow(b);
	GetDlgItem(IDC_RADIO_POS)->EnableWindow(b);
	GetDlgItem(IDC_RADIO_VEL)->EnableWindow(b);
	GetDlgItem(IDC_CHECK_OFFLINE)->EnableWindow(b);
	EnableOffLineModeGroup(b);
}

void CARoS_ros_interfaceDlg::EnableOffLineModeGroup(bool b)
{

	GetDlgItem(IDC_STATIC_OFFLINE_MODE)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_LOAD_TASK)->EnableWindow(b);
	if(task_loaded)
	{
		GetDlgItem(IDC_BUTTON_EXEC_TASK)->EnableWindow(b);
	}else{
		GetDlgItem(IDC_BUTTON_EXEC_TASK)->EnableWindow(false);
	}
}

void CARoS_ros_interfaceDlg::EnableRightHandGroup(bool b)
{

	GetDlgItem(IDC_STATIC_RIGHT_HAND)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_HAND_POS)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_HAND_VEL)->EnableWindow(b);

	GetDlgItem(IDC_STATIC_RIGHT_JOINT7)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_JOINT8)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_JOINT9)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_JOINT10)->EnableWindow(b);

	GetDlgItem(IDC_POS_RIGHT_JOINT7)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT8)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT9)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT10)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT7_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT8_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT9_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT10_DEG)->EnableWindow(b);

	GetDlgItem(IDC_VEL_RIGHT_JOINT7)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT8)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT9)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT10)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT7_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT8_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT9_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT10_DEG)->EnableWindow(b);

	GetDlgItem(IDC_BUTTON_RIGHT_HAND_INIT)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_RIGHT_HAND_OPEN)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_RIGHT_HAND_OPEN_HOME)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_RIGHT_HAND_CLOSE)->EnableWindow(b);
}

void CARoS_ros_interfaceDlg::EnableLeftHandGroup(bool b)
{

	GetDlgItem(IDC_STATIC_LEFT_HAND)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_HAND_POS)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_HAND_VEL)->EnableWindow(b);

	GetDlgItem(IDC_STATIC_LEFT_JOINT7)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_JOINT8)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_JOINT9)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_JOINT10)->EnableWindow(b);

	GetDlgItem(IDC_POS_LEFT_JOINT7)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT8)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT9)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT10)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT7_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT8_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT9_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT10_DEG)->EnableWindow(b);

	GetDlgItem(IDC_VEL_LEFT_JOINT7)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT8)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT9)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT10)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT7_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT8_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT9_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT10_DEG)->EnableWindow(b);

	GetDlgItem(IDC_BUTTON_LEFT_HAND_INIT)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_LEFT_HAND_OPEN)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_LEFT_HAND_OPEN_HOME)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_LEFT_HAND_CLOSE)->EnableWindow(b);
}

void CARoS_ros_interfaceDlg::EnableRightArmGroup(bool b)
{
	GetDlgItem(IDC_STATIC_RIGHT_ARM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_ARM_POS)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_ARM_VEL)->EnableWindow(b);

	GetDlgItem(IDC_STATIC_RIGHT_JOINT0)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_JOINT1)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_JOINT2)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_JOINT3)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_JOINT4)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_JOINT5)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_RIGHT_JOINT6)->EnableWindow(b);

	GetDlgItem(IDC_POS_RIGHT_JOINT0)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT1)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT2)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT3)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT4)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT5)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT6)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT0_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT1_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT2_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT3_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT4_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT5_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_RIGHT_JOINT6_DEG)->EnableWindow(b);

	GetDlgItem(IDC_VEL_RIGHT_JOINT0)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT1)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT2)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT3)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT4)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT5)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT6)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT0_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT1_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT2_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT3_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT4_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT5_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_RIGHT_JOINT6_DEG)->EnableWindow(b);

	GetDlgItem(IDC_BUTTON_HOME_RIGHT_JOINT0)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_HOME_RIGHT_JOINT1)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_HOME_RIGHT_JOINT2)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_HOME_RIGHT_JOINT3)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_HOME_RIGHT_JOINT4)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_HOME_RIGHT_JOINT5)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_HOME_RIGHT_JOINT6)->EnableWindow(b);
}

void CARoS_ros_interfaceDlg::EnableLeftArmGroup(bool b)
{
	GetDlgItem(IDC_STATIC_LEFT_ARM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_ARM_POS)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_ARM_VEL)->EnableWindow(b);

	GetDlgItem(IDC_STATIC_LEFT_JOINT0)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_JOINT1)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_JOINT2)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_JOINT3)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_JOINT4)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_JOINT5)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_LEFT_JOINT6)->EnableWindow(b);

	GetDlgItem(IDC_POS_LEFT_JOINT0)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT1)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT2)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT3)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT4)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT5)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT6)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT0_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT1_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT2_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT3_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT4_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT5_DEG)->EnableWindow(b);
	GetDlgItem(IDC_POS_LEFT_JOINT6_DEG)->EnableWindow(b);

	GetDlgItem(IDC_VEL_LEFT_JOINT0)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT1)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT2)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT3)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT4)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT5)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT6)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT0_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT1_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT2_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT3_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT4_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT5_DEG)->EnableWindow(b);
	GetDlgItem(IDC_VEL_LEFT_JOINT6_DEG)->EnableWindow(b);

	GetDlgItem(IDC_BUTTON_HOME_LEFT_JOINT0)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_HOME_LEFT_JOINT1)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_HOME_LEFT_JOINT2)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_HOME_LEFT_JOINT3)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_HOME_LEFT_JOINT4)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_HOME_LEFT_JOINT5)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_HOME_LEFT_JOINT6)->EnableWindow(b);
}

void CARoS_ros_interfaceDlg::EnableJointsLowPassFilterGroup(bool b)
{
	GetDlgItem(IDC_STATIC_JOINTS_LOWPASSFILTER)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_JOINTS_CUTOFF_FREQ)->EnableWindow(b);
	GetDlgItem(IDC_EDIT_JOINTS_CUTOFF_FREQ)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_JOINTS_DELTA_TIME)->EnableWindow(b);
	GetDlgItem(IDC_EDIT_JOINTS_DELTA_TIME)->EnableWindow(b);	
}

void CARoS_ros_interfaceDlg::EnableRightUpperLimbGroup(bool b)
{
	GetDlgItem(IDC_STATIC_RIGHT_UPPERLIMB)->EnableWindow(b);
	EnableRightArmGroup(b);
	EnableRightHandGroup(b);
	GetDlgItem(IDC_BUTTON_RIGHT_HOME)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_RIGHT_PARK)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_RIGHT_STOP)->EnableWindow(b);
	right_enabled = b;
}

void CARoS_ros_interfaceDlg::EnableLeftUpperLimbGroup(bool b)
{
	GetDlgItem(IDC_STATIC_LEFT_UPPERLIMB)->EnableWindow(b);
	EnableLeftArmGroup(b);
	EnableLeftHandGroup(b);
	GetDlgItem(IDC_BUTTON_LEFT_HOME)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_LEFT_PARK)->EnableWindow(b);
	GetDlgItem(IDC_BUTTON_LEFT_STOP)->EnableWindow(b);
	left_enabled = b;
}

void CARoS_ros_interfaceDlg::EnableVisionGroup(bool b)
{
	GetDlgItem(IDC_STATIC_VISION)->EnableWindow(b);
	EnableVisionTargetGroup(b);
	EnableVisionObstaclesGroup(b);
	EnableVisionLowPassFilterGroup(b);
}

void CARoS_ros_interfaceDlg::EnableVisionLowPassFilterGroup(bool b)
{
	GetDlgItem(IDC_STATIC_OBJ_LOW_PASS_FILTER)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_POS)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_CUTOFF_FREQ_POS)->EnableWindow(b);
	GetDlgItem(IDC_EDIT_CUTOFF_FREQ_POS)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_DELTA_TIME_POS)->EnableWindow(b);
	GetDlgItem(IDC_EDIT_DELTA_TIME_POS)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_OR)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_CUTOFF_FREQ_OR)->EnableWindow(b);
	GetDlgItem(IDC_EDIT_CUTOFF_FREQ_OR)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_DELTA_TIME_OR)->EnableWindow(b);
	GetDlgItem(IDC_EDIT_DELTA_TIME_OR)->EnableWindow(b);	
}
void CARoS_ros_interfaceDlg::EnableVisionTargetGroup(bool b)
{
	GetDlgItem(IDC_STATIC_TARGET)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_X_TAR)->EnableWindow(b);
	GetDlgItem(IDC_X_POS_TAR)->EnableWindow(b);
	GetDlgItem(IDC_X_POS_TAR_MM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Y_TAR)->EnableWindow(b);
	GetDlgItem(IDC_y_POS_TAR)->EnableWindow(b);
	GetDlgItem(IDC_Y_POS_TAR_MM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Z_TAR)->EnableWindow(b);
	GetDlgItem(IDC_Z_POS_TAR)->EnableWindow(b);
	GetDlgItem(IDC_Z_POS_TAR_MM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Roll_TAR)->EnableWindow(b);
	GetDlgItem(IDC_Roll_TAR)->EnableWindow(b);
	GetDlgItem(IDC_Roll_TAR_DEG)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Pitch_TAR)->EnableWindow(b);
	GetDlgItem(IDC_Pitch_TAR)->EnableWindow(b);
	GetDlgItem(IDC_Pitch_TAR_DEG)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Yaw_TAR)->EnableWindow(b);
	GetDlgItem(IDC_Yaw_TAR)->EnableWindow(b);
	GetDlgItem(IDC_YAW_TAR_DEG)->EnableWindow(b);

	GetDlgItem(IDC_STATIC_OBJ_OR)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QX_RC)->EnableWindow(b);
	GetDlgItem(IDC_QX_RC)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QY_RC)->EnableWindow(b);
	GetDlgItem(IDC_QY_RC)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QZ_RC)->EnableWindow(b);
	GetDlgItem(IDC_QZ_RC)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QW_RC)->EnableWindow(b);
	GetDlgItem(IDC_QW_RC)->EnableWindow(b);

	GetDlgItem(IDC_STATIC_TAR_OR)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QX_RC_TAR)->EnableWindow(b);
	GetDlgItem(IDC_QX_RC_TAR)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QY_RC_TAR)->EnableWindow(b);
	GetDlgItem(IDC_QY_RC_TAR)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QZ_RC_TAR)->EnableWindow(b);
	GetDlgItem(IDC_QZ_RC_TAR)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QW_RC_TAR)->EnableWindow(b);
	GetDlgItem(IDC_QW_RC_TAR)->EnableWindow(b);
	
	GetDlgItem(IDC_STATIC_OBJ_ROT_MAT)->EnableWindow(b);
	GetDlgItem(IDC_OBJ_ROT_00)->EnableWindow(b);
	GetDlgItem(IDC_OBJ_ROT_01)->EnableWindow(b);
	GetDlgItem(IDC_OBJ_ROT_02)->EnableWindow(b);
	GetDlgItem(IDC_OBJ_ROT_10)->EnableWindow(b);
	GetDlgItem(IDC_OBJ_ROT_11)->EnableWindow(b);
	GetDlgItem(IDC_OBJ_ROT_12)->EnableWindow(b);
	GetDlgItem(IDC_OBJ_ROT_20)->EnableWindow(b);
	GetDlgItem(IDC_OBJ_ROT_21)->EnableWindow(b);
	GetDlgItem(IDC_OBJ_ROT_22)->EnableWindow(b);

	GetDlgItem(IDC_STATIC_TAR_ROT_MAT)->EnableWindow(b);
	GetDlgItem(IDC_TAR_ROT_00)->EnableWindow(b);
	GetDlgItem(IDC_TAR_ROT_01)->EnableWindow(b);
	GetDlgItem(IDC_TAR_ROT_02)->EnableWindow(b);
	GetDlgItem(IDC_TAR_ROT_10)->EnableWindow(b);
	GetDlgItem(IDC_TAR_ROT_11)->EnableWindow(b);
	GetDlgItem(IDC_TAR_ROT_12)->EnableWindow(b);
	GetDlgItem(IDC_TAR_ROT_20)->EnableWindow(b);
	GetDlgItem(IDC_TAR_ROT_21)->EnableWindow(b);
	GetDlgItem(IDC_TAR_ROT_22)->EnableWindow(b);

}

void CARoS_ros_interfaceDlg::EnableVisionObstaclesGroup(bool b)
{
	GetDlgItem(IDC_STATIC_OBSTACLES)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_GREEN_COLUMN)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_X_GC)->EnableWindow(b);
	GetDlgItem(IDC_X_POS_GC)->EnableWindow(b);
	GetDlgItem(IDC_X_POS_GC_MM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Y_GC)->EnableWindow(b);
	GetDlgItem(IDC_Y_POS_GC)->EnableWindow(b);
	GetDlgItem(IDC_Y_POS_GC_MM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Z_GC)->EnableWindow(b);
	GetDlgItem(IDC_Z_POS_GC)->EnableWindow(b);
	GetDlgItem(IDC_Z_POS_GC_MM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QX_GC)->EnableWindow(b);
	GetDlgItem(IDC_QX_GC)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QY_GC)->EnableWindow(b);
	GetDlgItem(IDC_QY_GC)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QZ_GC)->EnableWindow(b);
	GetDlgItem(IDC_QZ_GC)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QW_GC)->EnableWindow(b);
	GetDlgItem(IDC_QW_GC)->EnableWindow(b);

	GetDlgItem(IDC_STATIC_MAGENTA_COLUMN)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_X_MC)->EnableWindow(b);
	GetDlgItem(IDC_X_POS_MC)->EnableWindow(b);
	GetDlgItem(IDC_X_POS_MC_MM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Y_MC)->EnableWindow(b);
	GetDlgItem(IDC_Y_POS_MC)->EnableWindow(b);
	GetDlgItem(IDC_Y_POS_MC_MM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Z_MC)->EnableWindow(b);
	GetDlgItem(IDC_Z_POS_MC)->EnableWindow(b);
	GetDlgItem(IDC_Z_POS_MC_MM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QX_MC)->EnableWindow(b);
	GetDlgItem(IDC_QX_MC)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QY_MC)->EnableWindow(b);
	GetDlgItem(IDC_QY_MC)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QZ_MC)->EnableWindow(b);
	GetDlgItem(IDC_QZ_MC)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QW_MC)->EnableWindow(b);
	GetDlgItem(IDC_QW_MC)->EnableWindow(b);

	GetDlgItem(IDC_STATIC_BLUE_COLUMN)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_X_BC)->EnableWindow(b);
	GetDlgItem(IDC_X_POS_BC)->EnableWindow(b);
	GetDlgItem(IDC_X_POS_BC_MM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Y_BC)->EnableWindow(b);
	GetDlgItem(IDC_Y_POS_BC)->EnableWindow(b);
	GetDlgItem(IDC_Y_POS_BC_MM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_Z_BC)->EnableWindow(b);
	GetDlgItem(IDC_Z_POS_BC)->EnableWindow(b);
	GetDlgItem(IDC_Z_POS_BC_MM)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QX_BC)->EnableWindow(b);
	GetDlgItem(IDC_QX_BC)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QY_BC)->EnableWindow(b);
	GetDlgItem(IDC_QY_BC)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QZ_BC)->EnableWindow(b);
	GetDlgItem(IDC_QZ_BC)->EnableWindow(b);
	GetDlgItem(IDC_STATIC_QW_BC)->EnableWindow(b);
	GetDlgItem(IDC_QW_BC)->EnableWindow(b);

}

void CARoS_ros_interfaceDlg::updateJointValuesAsync(Joint_States& jstate)
{
	if(right_enabled && !left_enabled){
		std::vector<float>jpos = jstate.position;
		std::vector<float>jvel = jstate.velocity;
		CString s; int precision = 2;
		
		s.Format(_T("%.*f"),precision,jpos.at(0)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT0)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jpos.at(1)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT1)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jpos.at(2)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT2)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jpos.at(3)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT3)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jpos.at(4)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT4)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jpos.at(5)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT5)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jpos.at(6)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT6)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jpos.at(7)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT7)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jpos.at(8)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT8)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jpos.at(9)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT9)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jpos.at(10)*RAD_TO_DEG_F);GetDlgItem(IDC_POS_RIGHT_JOINT10)->SetWindowTextW(s);

		s.Format(_T("%.*f"),precision,jvel.at(0)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT0)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jvel.at(1)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT1)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jvel.at(2)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT2)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jvel.at(3)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT3)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jvel.at(4)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT4)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jvel.at(5)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT5)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jvel.at(6)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT6)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jvel.at(7)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT7)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jvel.at(8)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT8)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jvel.at(9)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT9)->SetWindowTextW(s);
		s.Format(_T("%.*f"),precision,jvel.at(10)*RAD_TO_DEG_F);GetDlgItem(IDC_VEL_RIGHT_JOINT10)->SetWindowTextW(s);
	}else if(!right_enabled && left_enabled){
		// TO DO
	}else if(right_enabled && left_enabled){
		//TO DO
	}
}

void CARoS_ros_interfaceDlg::updateJointValues()
{

	while(start_joint_update){
		Joint_States jstate;
		if(yarp_upperlimb->getJointState(jstate)){
			if(right_enabled && !left_enabled){
				std::vector<float>jpos = jstate.position;
				std::vector<float>jvel = jstate.velocity;
				CString s; int precision = 2;
		
				s.Format(_T("%.*f"),precision,jpos.at(0)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT0)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jpos.at(1)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT1)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jpos.at(2)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT2)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jpos.at(3)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT3)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jpos.at(4)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT4)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jpos.at(5)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT5)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jpos.at(6)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT6)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jpos.at(7)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT7)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jpos.at(8)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT8)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jpos.at(9)*RAD_TO_DEG_F); GetDlgItem(IDC_POS_RIGHT_JOINT9)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jpos.at(10)*RAD_TO_DEG_F);GetDlgItem(IDC_POS_RIGHT_JOINT10)->SetWindowTextW(s);

				s.Format(_T("%.*f"),precision,jvel.at(0)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT0)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jvel.at(1)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT1)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jvel.at(2)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT2)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jvel.at(3)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT3)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jvel.at(4)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT4)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jvel.at(5)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT5)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jvel.at(6)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT6)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jvel.at(7)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT7)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jvel.at(8)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT8)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jvel.at(9)*RAD_TO_DEG_F); GetDlgItem(IDC_VEL_RIGHT_JOINT9)->SetWindowTextW(s);
				s.Format(_T("%.*f"),precision,jvel.at(10)*RAD_TO_DEG_F);GetDlgItem(IDC_VEL_RIGHT_JOINT10)->SetWindowTextW(s);

			}else if(!right_enabled && left_enabled){
				// TO DO
			}else if(right_enabled && left_enabled){
				//TO DO
			}
		}
	}
	//addLogLine(_T("thread finished"));
}

void CARoS_ros_interfaceDlg::updateVisionValues()
{
	std::vector<float> obj_pos;
	std::vector<float> obj_or;
	Quaternionf obj_q_or, tar_q_or;
	Matrix3f obj_m_or,tar_m_or;
	CString s; int precision = 2;
	while(start_vision_update){
		if(this->yarp_vision->getVisionInfo()){	
			CString str_freq_pos, str_dt_pos, str_freq_or, str_dt_or;
			GetDlgItemText(IDC_EDIT_CUTOFF_FREQ_POS,str_freq_pos);
			GetDlgItemText(IDC_EDIT_DELTA_TIME_POS,str_dt_pos);
			GetDlgItemText(IDC_EDIT_CUTOFF_FREQ_OR,str_freq_or);
			GetDlgItemText(IDC_EDIT_DELTA_TIME_OR,str_dt_or);
			float value_freq_pos = _ttof(str_freq_pos);
			float value_dt_pos = _ttof(str_dt_pos);
			float value_freq_or = _ttof(str_freq_or);
			float value_dt_or = _ttof(str_dt_or);
			// green column
			this->yarp_vision->getGreenColumn()->setLowPassFilters(value_freq_pos,value_dt_pos,value_freq_or,value_dt_or);
			this->yarp_vision->getGreenColumn()->getPos(obj_pos);			
			s.Format(_T("%.*f"),precision,obj_pos.at(0)); GetDlgItem(IDC_X_POS_GC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_pos.at(1)); GetDlgItem(IDC_Y_POS_GC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_pos.at(2)); GetDlgItem(IDC_Z_POS_GC)->SetWindowTextW(s);			
		    obj_q_or = this->yarp_vision->getGreenColumn()->getQOr();			
			s.Format(_T("%.*f"),precision,obj_q_or.x()); GetDlgItem(IDC_QX_GC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_q_or.y()); GetDlgItem(IDC_QY_GC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_q_or.z()); GetDlgItem(IDC_QZ_GC)->SetWindowTextW(s);						
			s.Format(_T("%.*f"),precision,obj_q_or.w()); GetDlgItem(IDC_QW_GC)->SetWindowTextW(s);						
			// red column
			this->yarp_vision->getRedColumn()->setLowPassFilters(value_freq_pos,value_dt_pos,value_freq_or,value_dt_or);
			this->yarp_vision->getRedColumn()->getPos(obj_pos);			
			s.Format(_T("%.*f"),precision,obj_pos.at(0)); GetDlgItem(IDC_X_POS_TAR)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_pos.at(1)); GetDlgItem(IDC_y_POS_TAR)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_pos.at(2)); GetDlgItem(IDC_Z_POS_TAR)->SetWindowTextW(s);			
			this->yarp_vision->getRedColumn()->getOr(obj_or);			
			s.Format(_T("%.*f"),precision,obj_or.at(0)); GetDlgItem(IDC_Roll_TAR)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_or.at(1)); GetDlgItem(IDC_Pitch_TAR)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_or.at(2)); GetDlgItem(IDC_Yaw_TAR)->SetWindowTextW(s);						
			obj_q_or = this->yarp_vision->getRedColumn()->getQOr();			
			s.Format(_T("%.*f"),precision,obj_q_or.x()); GetDlgItem(IDC_QX_RC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_q_or.y()); GetDlgItem(IDC_QY_RC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_q_or.z()); GetDlgItem(IDC_QZ_RC)->SetWindowTextW(s);						
			s.Format(_T("%.*f"),precision,obj_q_or.w()); GetDlgItem(IDC_QW_RC)->SetWindowTextW(s);	
			tar_q_or = this->yarp_vision->getRedColumn()->getTarQOr();			
			s.Format(_T("%.*f"),precision,tar_q_or.x()); GetDlgItem(IDC_QX_RC_TAR)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,tar_q_or.y()); GetDlgItem(IDC_QY_RC_TAR)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,tar_q_or.z()); GetDlgItem(IDC_QZ_RC_TAR)->SetWindowTextW(s);						
			s.Format(_T("%.*f"),precision,tar_q_or.w()); GetDlgItem(IDC_QW_RC_TAR)->SetWindowTextW(s);	
			obj_m_or = this->yarp_vision->getRedColumn()->getRot();
			s.Format(_T("%.*f"),precision,obj_m_or(0,0)); GetDlgItem(IDC_OBJ_ROT_00)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_m_or(0,1)); GetDlgItem(IDC_OBJ_ROT_01)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_m_or(0,2)); GetDlgItem(IDC_OBJ_ROT_02)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_m_or(1,0)); GetDlgItem(IDC_OBJ_ROT_10)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_m_or(1,1)); GetDlgItem(IDC_OBJ_ROT_11)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_m_or(1,2)); GetDlgItem(IDC_OBJ_ROT_12)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_m_or(2,0)); GetDlgItem(IDC_OBJ_ROT_20)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_m_or(2,1)); GetDlgItem(IDC_OBJ_ROT_21)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_m_or(2,2)); GetDlgItem(IDC_OBJ_ROT_22)->SetWindowTextW(s);
			tar_m_or = this->yarp_vision->getRedColumn()->getTarRot();
			s.Format(_T("%.*f"),precision,tar_m_or(0,0)); GetDlgItem(IDC_TAR_ROT_00)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,tar_m_or(0,1)); GetDlgItem(IDC_TAR_ROT_01)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,tar_m_or(0,2)); GetDlgItem(IDC_TAR_ROT_02)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,tar_m_or(1,0)); GetDlgItem(IDC_TAR_ROT_10)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,tar_m_or(1,1)); GetDlgItem(IDC_TAR_ROT_11)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,tar_m_or(1,2)); GetDlgItem(IDC_TAR_ROT_12)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,tar_m_or(2,0)); GetDlgItem(IDC_TAR_ROT_20)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,tar_m_or(2,1)); GetDlgItem(IDC_TAR_ROT_21)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,tar_m_or(2,2)); GetDlgItem(IDC_TAR_ROT_22)->SetWindowTextW(s);
			// magenta column
			this->yarp_vision->getMagentaColumn()->setLowPassFilters(value_freq_pos,value_dt_pos,value_freq_or,value_dt_or);
			this->yarp_vision->getMagentaColumn()->getPos(obj_pos);			
			s.Format(_T("%.*f"),precision,obj_pos.at(0)); GetDlgItem(IDC_X_POS_MC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_pos.at(1)); GetDlgItem(IDC_Y_POS_MC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_pos.at(2)); GetDlgItem(IDC_Z_POS_MC)->SetWindowTextW(s);			
			obj_q_or = this->yarp_vision->getMagentaColumn()->getQOr();			
			s.Format(_T("%.*f"),precision,obj_q_or.x()); GetDlgItem(IDC_QX_MC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_q_or.y()); GetDlgItem(IDC_QY_MC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_q_or.z()); GetDlgItem(IDC_QZ_MC)->SetWindowTextW(s);						
			s.Format(_T("%.*f"),precision,obj_q_or.w()); GetDlgItem(IDC_QW_MC)->SetWindowTextW(s);						
			// blue column
			this->yarp_vision->getBlueColumn()->setLowPassFilters(value_freq_pos,value_dt_pos,value_freq_or,value_dt_or);
			this->yarp_vision->getBlueColumn()->getPos(obj_pos);			
			s.Format(_T("%.*f"),precision,obj_pos.at(0)); GetDlgItem(IDC_X_POS_BC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_pos.at(1)); GetDlgItem(IDC_Y_POS_BC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_pos.at(2)); GetDlgItem(IDC_Z_POS_BC)->SetWindowTextW(s);			
			obj_q_or = this->yarp_vision->getBlueColumn()->getQOr();			
			s.Format(_T("%.*f"),precision,obj_q_or.x()); GetDlgItem(IDC_QX_BC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_q_or.y()); GetDlgItem(IDC_QY_BC)->SetWindowTextW(s);
			s.Format(_T("%.*f"),precision,obj_q_or.z()); GetDlgItem(IDC_QZ_BC)->SetWindowTextW(s);						
			s.Format(_T("%.*f"),precision,obj_q_or.w()); GetDlgItem(IDC_QW_BC)->SetWindowTextW(s);						
		}
	}
}

float CARoS_ros_interfaceDlg::getJointsLpfFreq()
{
	CString str_freq;
	GetDlgItemText(IDC_EDIT_JOINTS_CUTOFF_FREQ,str_freq);
	return _ttof(str_freq);				
}

float CARoS_ros_interfaceDlg::getJointsLpfDt()
{
	CString str_dt;
	GetDlgItemText(IDC_EDIT_JOINTS_DELTA_TIME,str_dt);
	return _ttof(str_dt);				
}

void CARoS_ros_interfaceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	if(nID == SC_CLOSE)
	{
		closing = true;
		if(yarp_upperlimb->connected || yarp_vision->connected){
			AfxMessageBox(_T("Close the connections with the YARP modules, then exit."));
		}else{
			CDialogEx::OnSysCommand(nID, lParam);
		}
	}else{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CARoS_ros_interfaceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CARoS_ros_interfaceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/**
void CARoS_ros_interfaceDlg::OnBnClickedButtonListen()
{
	ros_node->listen();
}

void CARoS_ros_interfaceDlg::OnBnClickedButtonTalk()
{
	ros_node->advertise("chatter1");
}
*/

void CARoS_ros_interfaceDlg::setROSNode(rosPtr r)
{
	ros_node = r;
}

rosPtr CARoS_ros_interfaceDlg::getROSNode()
{
	return ros_node;
}

void CARoS_ros_interfaceDlg::setYARPUpperlimb(CYarpCommInterface* y)
{
	yarp_upperlimb = y;
}

CYarpCommInterface* CARoS_ros_interfaceDlg::getYARPUpperlimb()
{
	return yarp_upperlimb;
}

void CARoS_ros_interfaceDlg::setYARPVision(CYarpCommInterface* y)
{
	yarp_vision = y;
}

CYarpCommInterface* CARoS_ros_interfaceDlg::getYARPVision()
{
	return yarp_vision;
}

void CARoS_ros_interfaceDlg::addLogLine(CString str)
{
	m_log_list.InsertString(-1,str);
	int min,max;
	m_log_list.GetScrollRange(SB_VERT,&min,&max);
	m_log_list.SetScrollPos(SB_VERT,max,TRUE);
	m_log_list.SendMessage(WM_VSCROLL,SB_BOTTOM,0);
	m_log_list.UpdateWindow();
}

void CARoS_ros_interfaceDlg::start_joint_updating()
{
	start_joint_update = true;
	update_joints_values_thd = boost::thread(boost::bind(&CARoS_ros_interfaceDlg::updateJointValues, this));

}
void CARoS_ros_interfaceDlg::stop_joint_updating()
{
	start_joint_update = false;		
}

void CARoS_ros_interfaceDlg::start_vision_updating()
{
	start_vision_update = true;
	update_vision_values_thd = boost::thread(boost::bind(&CARoS_ros_interfaceDlg::updateVisionValues, this));

}
void CARoS_ros_interfaceDlg::stop_vision_updating()
{
	start_vision_update = false;		
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightHome0()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->homeArmJoint(0)){
			addLogLine(_T("Right Joint 0 at Home"));
		}else{
			addLogLine(_T("Home right Joint 0 failed!"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightHome1()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->homeArmJoint(1)){
			addLogLine(_T("Right Joint 1 at Home"));
		}else{
			addLogLine(_T("Home right Joint 1 failed!"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightHome2()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->homeArmJoint(2)){
			addLogLine(_T("Right Joint 2 at Home"));
		}else{
			addLogLine(_T("Home right Joint 2 failed!"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightHome3()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->homeArmJoint(3)){
			addLogLine(_T("Right Joint 3 at Home"));
		}else{
			addLogLine(_T("Home right Joint 3 failed!"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightHome4()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->homeArmJoint(4)){
			addLogLine(_T("Right Joint 4 at Home"));
		}else{
			addLogLine(_T("Home right Joint 4 failed!"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightHome5()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->homeArmJoint(5)){
			addLogLine(_T("Right Joint 5 at Home"));
		}else{
			addLogLine(_T("Home right Joint 5 failed!"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightHome6()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->homeArmJoint(6)){
			addLogLine(_T("Right Joint 6 at Home"));
		}else{
			addLogLine(_T("Home right Joint 6 failed!"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightHandInit()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->initRightHand()){
			addLogLine(_T("Right Hand Initialized"));
		}else{
			addLogLine(_T("Right Hand initialization failed!"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightHandOpen()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->openRightHand()){
			addLogLine(_T("Right Hand opened"));
		}else{
			addLogLine(_T("Right Hand opening failed!"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightHandOpenPark()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->openRightHandPark()){
			addLogLine(_T("Right Hand opened and parked"));
		}else{
			addLogLine(_T("Right Hand opening and parking failed!"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightHandClose()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->closeRightHand()){
			addLogLine(_T("Right Hand closed"));
		}else{
			addLogLine(_T("Right Hand closing failed!"));
		}
	}
}


void CARoS_ros_interfaceDlg::onBnClickedButtonRightUpperLimbHome()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->homeRightUpperLimb()){
			addLogLine(_T("Right Upper limb homed"));
		}else{
			addLogLine(_T("Right Upper limb homing failed!"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightUpperLimbPark()
{
	if(yarp_upperlimb->connected){
		if(yarp_upperlimb->parkRightUpperLimb()){
			addLogLine(_T("Right Upper limb parked"));
		}else{
			addLogLine(_T("Right Upper limb parking failed!"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedButtonRightUpperLimbStop()
{
	if(yarp_upperlimb->connected)
	{
		yarp_upperlimb->stopMovement();
		addLogLine(_T("Right Upper limb stopped"));		
	}
}

void CARoS_ros_interfaceDlg::onBnClickedCheckOffline()
{
	CButton* m_offline = (CButton*)GetDlgItem(IDC_CHECK_OFFLINE);
	if(m_offline->GetCheck()==BST_CHECKED)
	{
		GetDlgItem(IDC_RADIO_POS)->EnableWindow(true);
		GetDlgItem(IDC_RADIO_VEL)->EnableWindow(true);
		CheckDlgButton(IDC_RADIO_POS,1);
		CheckDlgButton(IDC_RADIO_VEL,0);
		EnableOffLineModeGroup(true);
		if(this->ros_node->isConnected())
			this->ros_node->unsubscribeSetJoints();
		addLogLine(_T("OFF-LINE mode"));	
	}else{
		GetDlgItem(IDC_RADIO_POS)->EnableWindow(false);
		GetDlgItem(IDC_RADIO_VEL)->EnableWindow(true);
		CheckDlgButton(IDC_RADIO_POS,0);
		CheckDlgButton(IDC_RADIO_VEL,1);
		EnableOffLineModeGroup(false);
		addLogLine(_T("ON-LINE mode: only velocity control is allowed"));
		if(this->ros_node->isConnected()){
			this->ros_node->subscribeSetJoints(this->topic_set_joints);
			addLogLine(_T("ON-LINE mode: ROS node subscribed"));
		}else{
			addLogLine(_T("ON-LINE mode: connect to the ROS network to get the joints values"));
		}
	}
}

void CARoS_ros_interfaceDlg::onBnClickedLoadTask()
{
	bool success = true;
	CFileDialog* pFileDlg = new CFileDialog(true,_T("task"),NULL,OFN_OVERWRITEPROMPT,_T("Data Files (*.task)|*.task| ||"),NULL);
	INT_PTR nResponse = pFileDlg->DoModal();
	if(nResponse==IDOK){
		std::wstring w_file_name = pFileDlg->GetPathName();
		CString file_name(w_file_name.c_str());
		success = loadTask(file_name);
	}else{success = false;}

	if(success)
	{
		task_loaded = true;
		GetDlgItem(IDC_BUTTON_EXEC_TASK)->EnableWindow(true);
		addLogLine(_T("Task loaded"));	
	}else{
		task_loaded = false;
		GetDlgItem(IDC_BUTTON_EXEC_TASK)->EnableWindow(false);
		addLogLine(_T("Task NOT loaded"));
	}
	delete pFileDlg;
	pFileDlg = nullptr;
}

bool CARoS_ros_interfaceDlg::loadTask(CString file_name)
{
	bool success = true; CStdioFile task_file;
	task.clear();
	try
	{
		if(task_file.Open(file_name,CFile::modeRead)){
			float time_step, // read time step value, sec
				time_step_prev, // previous time step, sec
				min_timestep_value; // minimum time step value allowed, sec

			int steps_stage = 0; // number of steps in one stage

			CString strLine;
			while(task_file.ReadString(strLine))
			{
				if((strLine.Find(_T("#"))!=-1) && (strLine.Find(_T("#END"))==-1))
				{ 
					// new movement in the task
					if(steps_stage!=0 && !task.empty()){task.at(task.size()-1).stage_sizes.push_back(steps_stage);}
					steps_stage = 0;
					task.push_back(movement());

					// planner
					if((strLine.Find(_T("Planner:"))!=-1) && (strLine.Find(_T("HUMP"))!=-1)){
						min_timestep_value = 0.01; 
					}else{ min_timestep_value = 0.2; }

					// type of movement
					if((strLine.Find(_T("Reach-to-grasp,"))!=-1)){
						task.at(task.size()-1).mov_type = movementType::PICK;
					}else if((strLine.Find(_T("Engage,"))!=-1) || (strLine.Find(_T("Disengage,"))!=-1) || (strLine.Find(_T("Transport,"))!=-1)){
						task.at(task.size()-1).mov_type = movementType::PLACE;
					}else if((strLine.Find(_T("Go park,"))!=-1) || (strLine.Find(_T("Reaching,"))!=-1)){
						task.at(task.size()-1).mov_type = movementType::MOVE;
					}
				}else if(strLine.Find(_T("Movement stage"))!=-1){
					// new stage in the movement
					if(steps_stage!=0){task.at(task.size()-1).stage_sizes.push_back(steps_stage);}
					steps_stage = 0;
					// type of stage
					if (strLine.Find(_T("plan"))!=-1){
						task.at(task.size()-1).stages.push_back(stageType::PLAN); 
					}else if(strLine.Find(_T("approach"))!=-1){
						task.at(task.size()-1).stages.push_back(stageType::APPROACH);
					}else if(strLine.Find(_T("retreat"))!=-1){
						task.at(task.size()-1).stages.push_back(stageType::RETREAT);
					}
				}else if(strLine.Find(_T("step"))!=-1){
					// new step of the movement
					std::vector<float> j_pos; std::vector<float> j_vel; std::vector<float> j_acc;
					
					int idx = 0; CString token;
					token = strLine.Tokenize(_T(","),idx);
					while(!token.IsEmpty())
					{
						//TRACE(token+_T(" "));
						token = strLine.Tokenize(_T(","),idx);
						if(token.Find(_T("time step"))!=-1)
						{
							int idx1=0;	CString time_step_token;
							time_step_token = token.Tokenize(_T("="),idx1);
							time_step_token = token.Tokenize(_T("="),idx1);
							//TRACE(time_step_token+_T(" "));
							time_step = _ttof(time_step_token); 
							if(time_step < min_timestep_value){time_step = min_timestep_value;}
							task.at(task.size()-1).time_steps.push_back(time_step);
						}
						if(token.Find(_T("Joint 1"))!=-1 && token.Find(_T("Joint 10"))==-1 && token.Find(_T("Joint 11"))==-1)
						{
							// Joint 1 
							int idx1=0;	CString joint_token;
							joint_token = token.Tokenize(_T("="),idx1);
							joint_token = token.Tokenize(_T("="),idx1);
							//TRACE(joint_token+_T(" "));
							int idx2=0;	CString joint_values_token;
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // pos
							j_pos.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // vel
							j_vel.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // acc
							j_acc.push_back(_ttof(joint_values_token));
						}
						if(token.Find(_T("Joint 2"))!=-1)
						{
							// Joint 2 
							int idx1=0;	CString joint_token;
							joint_token = token.Tokenize(_T("="),idx1);
							joint_token = token.Tokenize(_T("="),idx1);
							//TRACE(joint_token+_T(" "));
							int idx2=0;	CString joint_values_token;
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // pos
							j_pos.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // vel
							j_vel.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // acc
							j_acc.push_back(_ttof(joint_values_token));
						}
						if(token.Find(_T("Joint 3"))!=-1)
						{
							// Joint 3 
							int idx1=0;	CString joint_token;
							joint_token = token.Tokenize(_T("="),idx1);
							joint_token = token.Tokenize(_T("="),idx1);
							//TRACE(joint_token+_T(" "));
							int idx2=0;	CString joint_values_token;
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // pos
							j_pos.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // vel
							j_vel.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // acc
							j_acc.push_back(_ttof(joint_values_token));
						}
						if(token.Find(_T("Joint 4"))!=-1)
						{
							// Joint 4 
							int idx1=0;	CString joint_token;
							joint_token = token.Tokenize(_T("="),idx1);
							joint_token = token.Tokenize(_T("="),idx1);
							//TRACE(joint_token+_T(" "));
							int idx2=0;	CString joint_values_token;
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // pos
							j_pos.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // vel
							j_vel.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // acc
							j_acc.push_back(_ttof(joint_values_token));
						}
						if(token.Find(_T("Joint 5"))!=-1)
						{
							// Joint 5 
							int idx1=0;	CString joint_token;
							joint_token = token.Tokenize(_T("="),idx1);
							joint_token = token.Tokenize(_T("="),idx1);
							//TRACE(joint_token+_T(" "));
							int idx2=0;	CString joint_values_token;
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // pos
							j_pos.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // vel
							j_vel.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // acc
							j_acc.push_back(_ttof(joint_values_token));
						}
						if(token.Find(_T("Joint 6"))!=-1)
						{
							// Joint 6 
							int idx1=0;	CString joint_token;
							joint_token = token.Tokenize(_T("="),idx1);
							joint_token = token.Tokenize(_T("="),idx1);
							//TRACE(joint_token+_T(" "));
							int idx2=0;	CString joint_values_token;
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // pos
							j_pos.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // vel
							j_vel.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // acc
							j_acc.push_back(_ttof(joint_values_token));
						}
						if(token.Find(_T("Joint 7"))!=-1)
						{
							// Joint 7 
							int idx1=0;	CString joint_token;
							joint_token = token.Tokenize(_T("="),idx1);
							joint_token = token.Tokenize(_T("="),idx1);
							//TRACE(joint_token+_T(" "));
							int idx2=0;	CString joint_values_token;
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // pos
							j_pos.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // vel
							j_vel.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // acc
							j_acc.push_back(_ttof(joint_values_token));
						}
						if(token.Find(_T("Joint 8"))!=-1)
						{
							// Joint 8 
							int idx1=0;	CString joint_token;
							joint_token = token.Tokenize(_T("="),idx1);
							joint_token = token.Tokenize(_T("="),idx1);
							//TRACE(joint_token+_T(" "));
							int idx2=0;	CString joint_values_token;
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // pos
							j_pos.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // vel
							j_vel.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // acc
							j_acc.push_back(_ttof(joint_values_token));
						}
						if(token.Find(_T("Joint 9"))!=-1)
						{
							// Joint 9
							int idx1=0;	CString joint_token;
							joint_token = token.Tokenize(_T("="),idx1);
							joint_token = token.Tokenize(_T("="),idx1);
							//TRACE(joint_token+_T(" "));
							int idx2=0;	CString joint_values_token;
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // pos
							j_pos.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // vel
							j_vel.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // acc
							j_acc.push_back(_ttof(joint_values_token));
						}
						if(token.Find(_T("Joint 10"))!=-1)
						{
							// Joint 10 
							int idx1=0;	CString joint_token;
							joint_token = token.Tokenize(_T("="),idx1);
							joint_token = token.Tokenize(_T("="),idx1);
							//TRACE(joint_token+_T(" "));
							int idx2=0;	CString joint_values_token;
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // pos
							j_pos.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // vel
							j_vel.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // acc
							j_acc.push_back(_ttof(joint_values_token));
						}
						if(token.Find(_T("Joint 11"))!=-1)
						{
							// Joint 11
							int idx1=0;	CString joint_token;
							joint_token = token.Tokenize(_T("="),idx1);
							joint_token = token.Tokenize(_T("="),idx1);
							//TRACE(joint_token+_T(" "));
							int idx2=0;	CString joint_values_token;
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // pos
							j_pos.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // vel
							j_vel.push_back(_ttof(joint_values_token));
							joint_values_token = joint_token.Tokenize(_T("|"),idx2); // acc
							j_acc.push_back(_ttof(joint_values_token));
						}
					} // while tokens step line

					// add position offset and convert to radiants
					for(int i=0;i<j_pos.size();++i)
					{
						j_pos.at(i) *= DEG_TO_RAD_F; j_pos.at(i) += yarp_upperlimb->upperlimb_offset.at(i); 
						j_vel.at(i) *= DEG_TO_RAD_F;
						j_acc.at(i) *= DEG_TO_RAD_F;						
					}
					// add time step to positions
					if(steps_stage==0){
						j_pos.push_back(min_timestep_value);
						//j_vel.push_back(min_timestep_value);
						//j_acc.push_back(min_timestep_value);
					}else{
						j_pos.push_back(time_step_prev);
						//j_vel.push_back(time_step_prev);
						//j_acc.push_back(time_step_prev);
					}
					// add time step to velocities and accelerations
					if(time_step <= min_timestep_value)
					{
						j_vel.push_back(min_timestep_value);
						j_acc.push_back(min_timestep_value);
					}else{
						j_vel.push_back(time_step);
						j_acc.push_back(time_step);
					}
					time_step_prev = time_step;
					// add the step to the movement
					task.at(task.size()-1).j_pos_traj.push_back(j_pos);
					task.at(task.size()-1).j_vel_traj.push_back(j_vel);
					task.at(task.size()-1).j_acc_traj.push_back(j_acc);
					// increment the steps
					steps_stage++;

				}else if(strLine.Find(_T("#END"))!=-1){
					if(steps_stage!=0){task.at(task.size()-1).stage_sizes.push_back(steps_stage);}
				}else{
					//success=false;
					//AfxMessageBox(_T("File not readable, aborting ..."));
					//TRACE(strLine+_T("\n"));
					//break;
				}				
				//TRACE(_T("\n"));
			} // while lines in the files 
			task_file.Close();	

			
			// task with micro steps
			CString value_text;
			GetDlgItemText(IDC_EDIT_MICRO,value_text);
			int micro_step_factor = _ttoi(value_text);
			this->divideTask(micro_step_factor);
			this->task = this->task_micro;
			

			// upload the looking positions
			int scene = 2; // scene = 0: toy vehicle scenario, scene = 1: drinking service scenario, scene = 2 : natural obstacle avoidance
			std::vector<float> look(3);
			switch(scene)
			{
				case 0: // toy vehicle scenario
					{
						// pick
						look.at(0) = 86.93f; look.at(1) = -30.93f; look.at(2) = 130.60f;
						look_pos.push_back(look);
						// place 
						look.at(0) = 86.93f; look.at(1) = 0.0f; look.at(2) = 100.60f;
						look_pos.push_back(look);
						// go park
						look.at(0) = 200.0f; look.at(1) = 0.0f; look.at(2) = 130.0f;
						look_pos.push_back(look);
					}
					break;
				case 1: // drinking service scenario
					{
						
						// pick
						look.at(0) = 86.93f; look.at(1) = -35.93f; look.at(2) = 100.60f;
						look_pos.push_back(look);
						// place 1
						look.at(0) = 86.93f; look.at(1) = 0.0f; look.at(2) = 100.60f;
						look_pos.push_back(look);
						// place 2
						look.at(0) = 86.93f; look.at(1) = 0.0f; look.at(2) = 100.60f;
						look_pos.push_back(look);
						// place 3
						look.at(0) = 86.93f; look.at(1) = 0.0f; look.at(2) = 100.60f;
						look_pos.push_back(look);
						// place 4
						look.at(0) = 86.93f; look.at(1) = -35.93f; look.at(2) = 100.60f;
						look_pos.push_back(look);
						// go park
						look.at(0) = 200.0f; look.at(1) = 0.0f; look.at(2) = 130.0f;
						look_pos.push_back(look);
					}
					break;

				case 2: // natural obstacle avoidance
						// pick
						look.at(0) = 86.93f; look.at(1) = 0.0f; look.at(2) = 130.60f;
						look_pos.push_back(look);
						// place 
						look.at(0) = 100.0f; look.at(1) = 0.0f; look.at(2) = 130.0f;
						look_pos.push_back(look);

					break;
				default:
					break;
			}
		}else{
			TRACE(_T("Unable to open the file\n"));
			success=false;
		}
	}
	catch(CFileException* pe)
	{
		success=false;
		TRACE(_T("Loading task failed: %d\n"),pe->m_cause);
		pe->Delete();
	}

	return success;
}



void CARoS_ros_interfaceDlg::onBnClickedExecTask()
{
	bool success = true;
	UINT pos_check = ((CButton*)GetDlgItem(IDC_RADIO_POS))->GetCheck();
	UINT vel_check = ((CButton*)GetDlgItem(IDC_RADIO_VEL))->GetCheck();
	if(task_loaded)
	{
		bool pos = pos_check==BST_CHECKED && vel_check==BST_UNCHECKED;
		success = execTask(pos);
	}else{success = false;}

	if(success){addLogLine(_T("Task executed with success"));}else{addLogLine(_T("Task NOT executed"));}
}


 void CARoS_ros_interfaceDlg::divideTask(unsigned int n_micro_steps)
 {
	if(n_micro_steps>0){
		this->task_micro.clear();
		for(size_t i=0; i<task.size(); ++i)
		{
			//current movement
			movement mov = task.at(i);
			// movement to create
			movement mov_micro;

			// type of the movement
			mov_micro.mov_type = mov.mov_type;
			// type of stages
			mov_micro.stages = mov.stages;

			// position, velocity and acceleration
			std::vector<float> j_pos_init = mov.j_pos_traj.at(0);
			std::vector<float> j_vel_init = mov.j_vel_traj.at(0);
			std::vector<float> j_acc_init = mov.j_acc_traj.at(0);
			float timesteps_micro_init = (j_pos_init.back())/(n_micro_steps+1);
			j_pos_init.at(j_pos_init.size()-1) = timesteps_micro_init;
			j_vel_init.at(j_vel_init.size()-1) = timesteps_micro_init;
			j_acc_init.at(j_acc_init.size()-1) = timesteps_micro_init;
			mov_micro.j_pos_traj.push_back(j_pos_init);
			mov_micro.j_vel_traj.push_back(j_vel_init);
			mov_micro.j_acc_traj.push_back(j_acc_init);
			mov_micro.time_steps.push_back(timesteps_micro_init);
			for(size_t j=0; j<mov.j_pos_traj.size()-1; ++j)
			{
				std::vector<float> j_pos_j = mov.j_pos_traj.at(j);
				std::vector<float> j_pos_j1 = mov.j_pos_traj.at(j+1);
				std::vector<float> j_vel_j = mov.j_vel_traj.at(j);
				std::vector<float> j_vel_j1 = mov.j_vel_traj.at(j+1);
				std::vector<float> j_acc_j = mov.j_acc_traj.at(j);
				std::vector<float> j_acc_j1 = mov.j_acc_traj.at(j+1);

				float timesteps_micro_j = (j_pos_j.back())/(n_micro_steps+1);
				float timesteps_micro_j1 = (j_pos_j1.back())/(n_micro_steps+1);
				j_pos_j.at(j_pos_j.size()-1) = timesteps_micro_j; 
				j_pos_j1.at(j_pos_j1.size()-1) = timesteps_micro_j1; 
				j_vel_j.at(j_vel_j.size()-1) = timesteps_micro_j; 
				j_vel_j1.at(j_vel_j1.size()-1) = timesteps_micro_j1; 
				j_acc_j.at(j_acc_j.size()-1) = timesteps_micro_j; 
				j_acc_j1.at(j_acc_j1.size()-1) = timesteps_micro_j1; 
				for(int k=1;k<=n_micro_steps;++k)
				{
					std::vector<float> j_pos_k(j_pos_j.size());
					std::vector<float> j_vel_k(j_vel_j.size());
					std::vector<float> j_acc_k(j_acc_j.size());
					for(size_t h=0; h<j_pos_j.size()-1; ++h)
					{
						j_pos_k.at(h) = j_pos_j.at(h) + (j_pos_j1.at(h)-j_pos_j.at(h))*k/(n_micro_steps+1);
						j_vel_k.at(h) = j_vel_j.at(h) + (j_vel_j1.at(h)-j_vel_j.at(h))*k/(n_micro_steps+1);
						j_acc_k.at(h) = j_acc_j.at(h) + (j_acc_j1.at(h)-j_acc_j.at(h))*k/(n_micro_steps+1);
					}// for loop variables				
					j_pos_k.at(j_pos_k.size()-1) = timesteps_micro_j1;
					j_vel_k.at(j_vel_k.size()-1) = timesteps_micro_j1;
					j_acc_k.at(j_acc_k.size()-1) = timesteps_micro_j1;
					mov_micro.j_pos_traj.push_back(j_pos_k);
					mov_micro.j_vel_traj.push_back(j_vel_k);
					mov_micro.j_acc_traj.push_back(j_acc_k);
					mov_micro.time_steps.push_back(timesteps_micro_j1);
				} // for loop microsteps
				mov_micro.j_pos_traj.push_back(j_pos_j1);
				mov_micro.j_vel_traj.push_back(j_vel_j1);
				mov_micro.j_acc_traj.push_back(j_acc_j1);
				mov_micro.time_steps.push_back(timesteps_micro_j1);
			}// for loop steps

			// multiply the size of each stage
			mov_micro.stage_sizes.resize(mov.stage_sizes.size());
			for(size_t ss=0; ss<mov.stage_sizes.size(); ++ss)
			{
				mov_micro.stage_sizes.at(ss) = mov.stage_sizes.at(ss)*(n_micro_steps+1) - n_micro_steps;
			} // for loop size of each stage
			
			//delete extra steps
			for(size_t ss=0; ss<mov.stage_sizes.size()-1; ++ss)
			{
				int stage_size = mov_micro.stage_sizes.at(ss);
				int prev_stage_size=0;
				if(ss!=0)
				{
					prev_stage_size=mov_micro.stage_sizes.at(ss-1);
				}
				mov_micro.j_pos_traj.erase(mov_micro.j_pos_traj.begin()+prev_stage_size+stage_size,
											mov_micro.j_pos_traj.begin()+prev_stage_size+stage_size+n_micro_steps);
				mov_micro.j_vel_traj.erase(mov_micro.j_vel_traj.begin()+prev_stage_size+stage_size,
											mov_micro.j_vel_traj.begin()+prev_stage_size+stage_size+n_micro_steps);
				mov_micro.j_acc_traj.erase(mov_micro.j_acc_traj.begin()+prev_stage_size+stage_size,
											mov_micro.j_acc_traj.begin()+prev_stage_size+stage_size+n_micro_steps);			
			}

			// add the movement to the task
			task_micro.push_back(mov_micro);
		}// for loop task
	}else{
		this->task_micro = this->task;
	}
}

bool CARoS_ros_interfaceDlg::execTask(bool pos)
{
	bool success = true;
	if(!task.empty() 
		//&& yarp_upperlimb->connected)
		)
	{
		bool b_arm = false; bool b_hand = false; int n_step = 0;
		std::vector<float> j_pos_prev_ret; // trajectory of the retreat stage in the previous movement
		int ret_stage_size_prev; //size of the retreat stage of the previous movement
		std::vector<float> j_pos_exec; // position trajectory to execute
		std::vector<std::vector<float>> j_vel_traj_exec; // velocity trajectory to execute
		std::vector<std::vector<float>> j_vel_prev_ret; // velocity trajectory in the previous movement
		//std::vector<std::vector<float>> j_pos_traj_exec; // position trajectory to execute
		//std::vector<std::vector<float>> j_pos_traj_prev_ret; // position trajectory in the previous movement
		int steps_exec; // steps of the exec trajectory
		bool exec; // true if execute the trajectory, false otherwise
		int plan_stage_size, app_stage_size, ret_stage_size; // sizes of the stages
		int k_place = 1; // counter of place movements

		for(int m=0; m < task.size(); ++m)
		{	// m movement

			int posture_size = task.at(m).j_pos_traj.at(0).size(); // 11 joints + timestep = 12
			//int velocity_size = task.at(m).j_vel_traj.at(0).size(); // 11 joints
			int n_stages = task.at(m).stages.size(); // number of stages in the movement
			bool join_ret_plan_app = false; // join the stages retreat, plan and approach
			bool join_plan_app = false; // join the stages plan and approach
			bool join_ret_plan = false; // join the stages retreat and plan 
			bool join_plan = false; // only plan stage
			j_pos_exec.clear();
			//j_pos_traj_exec.clear(); 
			j_vel_traj_exec.clear();

			if(n_stages > 1)
			{ // there is more than one stage: either plan and approach or plan and retreat
				if(task.at(m).stages.at(0)==stageType::PLAN && task.at(m).stages.at(1)==stageType::APPROACH)
				{
					plan_stage_size = task.at(m).stage_sizes.at(0);	
					app_stage_size = task.at(m).stage_sizes.at(1);
					
					if(m==0 || j_pos_prev_ret.empty()){
						// first movement of the task  => there is not any retreat stage or no previous retreat 
						join_plan_app = true;
						j_pos_exec.resize((plan_stage_size+app_stage_size)*posture_size); 
						j_vel_traj_exec.resize(plan_stage_size+app_stage_size);
						//j_pos_traj_exec.resize(plan_stage_size+app_stage_size);
					}else if(!j_pos_prev_ret.empty()){
						join_ret_plan_app = true;
						j_pos_exec.resize((ret_stage_size_prev+plan_stage_size+app_stage_size)*posture_size); 
						j_vel_traj_exec.resize(ret_stage_size_prev+plan_stage_size+app_stage_size);
						//j_pos_traj_exec.resize(ret_stage_size_prev+plan_stage_size+app_stage_size);
					}
					if (n_stages > 2){
						ret_stage_size = task.at(m).stage_sizes.at(2);
					}else{
						ret_stage_size = 0;
					}
				}else if(task.at(m).stages.at(0)==stageType::PLAN && task.at(m).stages.at(1)==stageType::RETREAT)
				{
					plan_stage_size = task.at(m).stage_sizes.at(0);	
					app_stage_size = 0;				
					if(m!=0 && !j_pos_prev_ret.empty()){
						// this is not the first movement and there was a previous retreat stage
						join_ret_plan = true;
						j_pos_exec.resize((ret_stage_size_prev+plan_stage_size)*posture_size); 	
						j_vel_traj_exec.resize(ret_stage_size_prev+plan_stage_size);
						//j_pos_traj_exec.resize(ret_stage_size_prev+plan_stage_size);
					}
					ret_stage_size = task.at(m).stage_sizes.at(1);
				}
			}else{
				// there is only the plan stage
				plan_stage_size = task.at(m).stage_sizes.at(0);
				app_stage_size = 0;	ret_stage_size = 0;
				if(m!=0 && !j_pos_prev_ret.empty()){
					// there was a previous retreat stage
					join_ret_plan = true;
					j_pos_exec.resize((ret_stage_size_prev+plan_stage_size)*posture_size); 
					j_vel_traj_exec.resize(ret_stage_size_prev+plan_stage_size);
					//j_pos_traj_exec.resize(ret_stage_size_prev+plan_stage_size);
				}else{
					// there is not any previous retreat movement
					join_plan = true;
					j_pos_exec.resize(plan_stage_size*posture_size); 
					j_vel_traj_exec.resize(plan_stage_size);
					//j_pos_traj_exec.resize(plan_stage_size);
				}
			}
			switch(task.at(m).mov_type)
			{
			case movementType::PICK:
				b_arm = true; b_hand = true; 
				if(!look_pos.empty() && yarp_vision->connected)
				{
					std::vector<float> ll = look_pos.at(0);
					yarp_vision->lookAtPosition(ll.at(0),ll.at(1),ll.at(2));
				}
				break;
			case movementType::PLACE:
				b_arm = true; b_hand = false;
				if(!look_pos.empty() && yarp_vision->connected)
				{
					std::vector<float> ll = look_pos.at(k_place);
					yarp_vision->lookAtPosition(ll.at(0),ll.at(1),ll.at(2));
				}
				k_place++;
				break;
			case movementType::MOVE:
				b_arm = true; b_hand = true;
				if(!look_pos.empty() && yarp_vision->connected)
				{
					std::vector<float> ll = look_pos.at(look_pos.size()-1);
					yarp_vision->lookAtPosition(ll.at(0),ll.at(1),ll.at(2));
				}
				break;

			}
			// for loop stages
			int n_steps = 0; // steps of the movement
			for(int s=0; s < n_stages; ++s) 
			{
				switch(task.at(m).stages.at(s))
				{
				case stageType::PLAN:
					if(join_ret_plan_app){
						// insert the previous retreat stage
						for(int i = 0; i < j_pos_prev_ret.size(); ++i){
							j_pos_exec.at(i) = j_pos_prev_ret.at(i);
						}// for loop steps of the stage
						for(int i = 0; i < j_vel_prev_ret.size(); ++i){
							j_vel_traj_exec.at(i) = j_vel_prev_ret.at(i);
						}
						/*
						for(int i = 0; i < j_pos_traj_prev_ret.size(); ++i){
							j_pos_traj_exec.at(i) = j_pos_traj_prev_ret.at(i);
						}*/
						// insert the plan and the approach stage
						std::vector<float>::iterator it = j_pos_exec.begin()+j_pos_prev_ret.size();		
						std::vector<std::vector<float>>::iterator it_vel = j_vel_traj_exec.begin() + ret_stage_size_prev;
						//std::vector<std::vector<float>>::iterator it_pos = j_pos_traj_exec.begin() + ret_stage_size_prev;
						for(int i = n_steps; i < (n_steps+plan_stage_size+app_stage_size); ++i){
							std::vector<float> &waypt = task.at(m).j_pos_traj.at(i);
							it = std::copy(waypt.begin(),waypt.end(),it);
							*it_vel = task.at(m).j_vel_traj.at(i);
							//*it_pos = task.at(m).j_pos_traj.at(i);
							it_vel++; //it_pos++;
						}// for loop steps of the stage
						j_pos_prev_ret.clear(); j_vel_prev_ret.clear(); //j_pos_traj_prev_ret.clear();
						n_steps += (plan_stage_size+app_stage_size);
						steps_exec = ret_stage_size_prev + plan_stage_size + app_stage_size;
						exec=true;
					}else if(join_ret_plan){
						// insert the previous retreat stage
						for(int i = 0; i < j_pos_prev_ret.size(); ++i){
							j_pos_exec.at(i) = j_pos_prev_ret.at(i);
						}// for loop steps of the stage
						for(int i = 0; i < j_vel_prev_ret.size(); ++i){
							j_vel_traj_exec.at(i) = j_vel_prev_ret.at(i);
						}
						/*
						for(int i = 0; i < j_pos_traj_prev_ret.size(); ++i){
							j_pos_traj_exec.at(i) = j_pos_traj_prev_ret.at(i);
						}*/
						// insert the plan stage
						std::vector<float>::iterator it = j_pos_exec.begin()+j_pos_prev_ret.size();		
						std::vector<std::vector<float>>::iterator it_vel = j_vel_traj_exec.begin() + ret_stage_size_prev;
						//std::vector<std::vector<float>>::iterator it_pos = j_pos_traj_exec.begin() + ret_stage_size_prev;
						for(int i = n_steps; i < (n_steps+plan_stage_size); ++i){
							std::vector<float> &waypt = task.at(m).j_pos_traj.at(i);
							it = std::copy(waypt.begin(),waypt.end(),it);
							*it_vel = task.at(m).j_vel_traj.at(i);
							//*it_pos = task.at(m).j_pos_traj.at(i);
							it_vel++; //it_pos++;
						}// for loop steps of the stage
						j_pos_prev_ret.clear(); j_vel_prev_ret.clear(); //j_pos_traj_prev_ret.clear();
						steps_exec = ret_stage_size_prev + plan_stage_size;
						exec=true;
					}else if(join_plan_app){
						std::vector<float>::iterator it = j_pos_exec.begin();	
						std::vector<std::vector<float>>::iterator it_vel = j_vel_traj_exec.begin();
						//std::vector<std::vector<float>>::iterator it_pos = j_pos_traj_exec.begin();
						for(int i = n_steps; i < (n_steps+plan_stage_size+app_stage_size); ++i){
							std::vector<float> &waypt = task.at(m).j_pos_traj.at(i);
							it = std::copy(waypt.begin(),waypt.end(),it);
							*it_vel = task.at(m).j_vel_traj.at(i);
							//*it_pos = task.at(m).j_pos_traj.at(i);
							it_vel++; //it_pos++;
						}// for loop steps of the stage
						n_steps += (plan_stage_size+app_stage_size);
						steps_exec = plan_stage_size + app_stage_size;
						exec=true;
					}else if(join_plan){
						std::vector<float>::iterator it = j_pos_exec.begin();	
						std::vector<std::vector<float>>::iterator it_vel = j_vel_traj_exec.begin();
						//std::vector<std::vector<float>>::iterator it_pos = j_pos_traj_exec.begin();
						for(int i = n_steps; i < (n_steps+plan_stage_size); ++i){
							std::vector<float> &waypt = task.at(m).j_pos_traj.at(i);
							it = std::copy(waypt.begin(),waypt.end(),it);
							*it_vel = task.at(m).j_vel_traj.at(i);
							//*it_pos = task.at(m).j_pos_traj.at(i);
							it_vel++; //it_pos++;
						}// for loop steps of the stage
						n_steps += plan_stage_size;
						steps_exec = plan_stage_size;
						exec=true;
					}
					break;
				case stageType::APPROACH:
					exec=false;
					break;
				case stageType::RETREAT:
					{
						if(task.at(m).mov_type==movementType::PICK){yarp_upperlimb->closeRightHand(); b_hand=false;}	
						if(task.at(m).mov_type==movementType::PLACE){
							if (pos){
								yarp_upperlimb->openRightHand(); 
							}
							else{
								//std::vector<float> pos_0 = task.at(m).j_pos_traj.at(n_steps);
								yarp_upperlimb->openRightHand(task.at(m).j_pos_traj.at(n_steps).at(10)*RAD_TO_DEG_F); 
							}
							b_hand=true;}
						if(m==task.size()-1){
							// this is the retreat stage of the last movement => execute it
							j_pos_exec.resize(ret_stage_size*posture_size); 
							j_vel_traj_exec.resize(ret_stage_size);
							//j_pos_traj_exec.resize(ret_stage_size);
							std::vector<float>::iterator it = j_pos_exec.begin();	
							std::vector<std::vector<float>>::iterator it_vel = j_vel_traj_exec.begin();
							//std::vector<std::vector<float>>::iterator it_pos = j_pos_traj_exec.begin();
							for(int i = n_steps; i < task.at(m).j_pos_traj.size(); ++i){
								std::vector<float> &waypt = task.at(m).j_pos_traj.at(i);
								it = std::copy(waypt.begin(),waypt.end(),it);
								*it_vel = task.at(m).j_vel_traj.at(i);
								//*it_pos = task.at(m).j_pos_traj.at(i);
								it_vel++; //it_pos++;
							}// for loop steps of the stage
							steps_exec = ret_stage_size;
							exec=true;
						}else{
							j_pos_prev_ret.clear(); j_vel_prev_ret.clear(); //j_pos_traj_prev_ret.clear();
							j_pos_prev_ret.resize(ret_stage_size*posture_size); 
							j_vel_prev_ret.resize(ret_stage_size);
							std::vector<float>::iterator it = j_pos_prev_ret.begin();
							std::vector<std::vector<float>>::iterator it_vel = j_vel_prev_ret.begin();
							//std::vector<std::vector<float>>::iterator it_pos = j_pos_traj_prev_ret.begin();
							for(int i = n_steps; i < task.at(m).j_pos_traj.size(); ++i){
								std::vector<float> &waypt = task.at(m).j_pos_traj.at(i);
								it = std::copy(waypt.begin(),waypt.end(),it);
								*it_vel = task.at(m).j_vel_traj.at(i);
								//*it_pos = task.at(m).j_pos_traj.at(i);
								it_vel++; //it_pos++;
							}// for loop steps of the stage
							ret_stage_size_prev = ret_stage_size;
							exec=false;
						}
						break;
					}
				}// switch stages

				if(exec)
				{
					// execute 
					try
					{
						if(pos)
						{ // position control 
							// Send the trajectory to the UpperLimb server
							if(!yarp_upperlimb->addTrajectory(j_pos_exec,steps_exec,b_arm,b_hand))
							{
								success=false;
								break;
							}
							// wait asynchronously for the end of the execution
							if(!yarp_upperlimb->sendWaitTrajEndAsync())
							{
								success = false;
								break;
							}
							yarp_upperlimb->waitAsync();
						}else{
							// velocity control
							//yarp_upperlimb->addVelTrajectoryExec(j_vel_traj_exec);
							yarp_upperlimb->addVelTrajectory(j_vel_traj_exec);
							// wait asynchronously for the end of the execution
							if(!yarp_upperlimb->sendWaitVelTrajEndAsync())
							{
								success = false;
								break;
							}
							yarp_upperlimb->waitVelAsync();
						}
					}
					catch (std::runtime_error &ex)
					{
						AfxMessageBox(CA2CT(ex.what()),MB_OK);
					}
				}
			}// for loop stages of the movement
		}// for loop movements of the task

	}else{success=false;}
	return success;
}

