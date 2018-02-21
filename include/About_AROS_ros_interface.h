#pragma once


// finestra di dialogo CAbout_AROS_ros_interface

class CAbout_AROS_ros_dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAbout_AROS_ros_dlg)

public:
	CAbout_AROS_ros_dlg(CWnd* pParent = NULL);   // costruttore standard
	virtual ~CAbout_AROS_ros_dlg();

// Dati della finestra di dialogo
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Supporto DDX/DDV

	DECLARE_MESSAGE_MAP()
};
