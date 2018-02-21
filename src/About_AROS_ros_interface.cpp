// About_AROS_ros_interface.cpp : file di implementazione
//

#include "../include/stdafx.h"
#include "../include/ARoS_ros_interface.h"
#include "../include/About_AROS_ros_interface.h"
#include "afxdialogex.h"


// finestra di dialogo CAbout_AROS_ros_interface

IMPLEMENT_DYNAMIC(CAbout_AROS_ros_dlg, CDialogEx)

CAbout_AROS_ros_dlg::CAbout_AROS_ros_dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAbout_AROS_ros_dlg::IDD, pParent)
{

}

CAbout_AROS_ros_dlg::~CAbout_AROS_ros_dlg()
{
}

void CAbout_AROS_ros_dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAbout_AROS_ros_dlg, CDialogEx)
END_MESSAGE_MAP()


// gestori di messaggi CAbout_AROS_ros_interface
