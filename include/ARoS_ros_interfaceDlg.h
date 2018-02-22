
// ARoS_ros_interfaceDlg.h : header file
//

#pragma once

#include "Node.h"

// CARoS_ros_interfaceDlg dialog
class CARoS_ros_interfaceDlg : public CDialogEx
{
// Construction
public:
	CARoS_ros_interfaceDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_AROS_ROS_INTERFACE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

};
