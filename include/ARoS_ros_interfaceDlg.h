
// ARoS_ros_interfaceDlg.h : header file
//

#pragma once

#include "Node.h"
#include "afxwin.h"

// CARoS_ros_interfaceDlg dialog
class CARoS_ros_interfaceDlg : public CDialogEx
{
// Construction
public:
	CARoS_ros_interfaceDlg(CWnd* pParent = NULL);	// standard constructor
	CARoS_ros_interfaceDlg(CWnd* pParent,CNode* node);

// Dialog Data
	enum { IDD = IDD_AROS_ROS_INTERFACE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CNode * node;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	// logging list variable
	CListBox m_log_list;
	// add a line in loggings
	void addLogLine(CString line);
	// listen /chatter
	afx_msg void OnBnClickedButtonListen();
	// TALK /chatter1
	afx_msg void OnBnClickedButtonTalk();
	// set the ROS node
	void setROSNode(CNode* r);
	// get the ROS node
	CNode* getROSNode();
};
