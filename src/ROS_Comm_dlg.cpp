// ROS_Comm_dlg.cpp : file di implementazione
//
#include <iostream>
#include "../include/stdafx.h"
#include "../include/ARoS_ros_interface.h"
#include "../include/ROS_Comm_dlg.h"
#include "afxdialogex.h"


// finestra di dialogo CROS_Comm_dlg

IMPLEMENT_DYNAMIC(CROS_Comm_dlg, CDialogEx)

CROS_Comm_dlg::CROS_Comm_dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CROS_Comm_dlg::IDD, pParent)
	, m_ROS_MASTER_txt(_T("Enter the ROS_MASTER_URI"))
	, m_check_env_vars(FALSE)
	, m_ROS_IP_txt(_T("IP address of this PC"))
	, m_ros_connected(false)
{
	//Create(IDD_ROS_COMM_DIALOG,NULL);
}

CROS_Comm_dlg::CROS_Comm_dlg(CWnd* pParent,CNode* node)
	: CDialogEx(CROS_Comm_dlg::IDD, pParent)
	, m_ROS_MASTER_txt(_T("Enter the ROS_MASTER_URI"))
	, m_check_env_vars(FALSE)
	, m_ROS_IP_txt(_T("IP address of this PC"))
	, m_ros_connected(false)
{
	//Create(IDD_ROS_COMM_DIALOG,NULL);
}

CROS_Comm_dlg::~CROS_Comm_dlg()
{
}

void CROS_Comm_dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ROS_MASTER, m_ROS_MASTER_txt);
	DDX_Check(pDX, IDC_CHECK_ENV_VARS, m_check_env_vars);
	DDX_Text(pDX, IDC_EDIT_ROS_IP, m_ROS_IP_txt);
	DDX_Control(pDX, IDC_EDIT_ROS_MASTER, m_edit_master_uri);
	DDX_Control(pDX, IDC_EDIT_ROS_IP, m_edit_ros_ip);
}


BEGIN_MESSAGE_MAP(CROS_Comm_dlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_ENV_VARS, &CROS_Comm_dlg::OnBnClickedCheckEnvVars)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CROS_Comm_dlg::OnBnClickedButtonConnect)
END_MESSAGE_MAP()







// This function sets the ROS_MASTER_URI
void CROS_Comm_dlg::setMasterURI(CString& master_txt)
{
	UpdateData(FALSE);
	m_ROS_MASTER_txt = master_txt;
}


// set the ROS IP
void CROS_Comm_dlg::setROSIP(CString& ip_addr)
{
	UpdateData(FALSE);
	m_ROS_IP_txt = ip_addr;
}


// set for using the environment variables
void CROS_Comm_dlg::setEnvVars(bool r)
{
	UpdateData(FALSE);
	m_check_env_vars = r;
}


// get the ROS Master URI
CString CROS_Comm_dlg::getMasterURI(void)
{
	UpdateData(TRUE);
	return m_ROS_MASTER_txt.Trim();
}


// get the ROS IP address
CString CROS_Comm_dlg::getROSIP(void)
{
	UpdateData(TRUE);
	return m_ROS_IP_txt.Trim();
}


// get the boolean using the environment variables
bool CROS_Comm_dlg::getEnvVars(void)
{
	UpdateData(TRUE);
	return m_check_env_vars;
}


void CROS_Comm_dlg::OnBnClickedCheckEnvVars()
{
	UpdateData(TRUE);
	if(m_check_env_vars){
		m_edit_master_uri.EnableWindow(FALSE);
		m_edit_ros_ip.EnableWindow(FALSE);
	}else{
		m_edit_master_uri.EnableWindow(TRUE);
		m_edit_ros_ip.EnableWindow(TRUE);
	}
}


void CROS_Comm_dlg::OnBnClickedButtonConnect()
{
	// TODO: aggiungere qui il codice per la gestione della notifica del controllo.

	std::cout<< "pushed" << std::endl;
}
