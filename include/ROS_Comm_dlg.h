#pragma once
#include "afxwin.h"
#include "Node.h"

// finestra di dialogo CROS_Comm_dlg

class CROS_Comm_dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CROS_Comm_dlg)

public:
	CROS_Comm_dlg(CWnd* pParent = NULL);   // costruttore standard
	CROS_Comm_dlg(CWnd* pParent,CNode* node);
	virtual ~CROS_Comm_dlg();

// Dati della finestra di dialogo
	enum { IDD = IDD_ROS_COMM_DIALOG };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV

	DECLARE_MESSAGE_MAP()
	// ROS MASTER URI of the machine where the roscore is running
	CString m_ROS_MASTER_txt;

protected:
	// true if the environment variables are used for ROS communication, false otherwise
	BOOL m_check_env_vars;
protected:
	// IP address of this PC
	CString m_ROS_IP_txt;
	// control the IDC_ROS_MASTER_URI
	CEdit m_edit_master_uri;
	// control variable for IDC_EDIT_ROS_IP
	CEdit m_edit_ros_ip;
public:
	// This function sets the ROS_MASTER_URI
	void setMasterURI(CString& master_txt);
	// set the ROS IP
	void setROSIP(CString& ip_addr);
	// set for using the environment variables
	void setEnvVars(bool r);
	// set the ROS node
	void setROSNode(CNode* r);
	// get the ROS node
	CNode* getROSNode();
	// get the ROS Master URI
	CString getMasterURI(void);
	// get the ROS IP address
	CString getROSIP(void);
	// get the boolean using the environment variables
	bool getEnvVars(void);
	// show error message box
	void showNoMasterMessage();
	// get the ROS status
	bool getROSConn();




public:
	afx_msg void OnBnClickedCheckEnvVars();
	// true if ROS is connected, false otherwise
	bool m_ros_connected;
	// ROS Communication status
	CString status;
	// caption of the connect button
	CString strButtonConnect;
	// label of ROS status
	CString strStatus;
	// ros node
	CNode* node;
	afx_msg void OnBnClickedButtonConnect();
	// control of the button Connect/Disconnect to/from the ROS network
	CButton m_buttonConnect;
	// control variable to check the ROS connection
	CStatic m_ros_conn;
};
