// ROS_Comm_dlg.cpp : file di implementazione
//

#include "../include/stdafx.h"
#include "../include/ARoS_ros_interface.h"
#include "../include/ROS_Comm_dlg.h"
#include "afxdialogex.h"


// finestra di dialogo CROS_Comm_dlg

IMPLEMENT_DYNAMIC(CROS_Comm_dlg, CDialogEx)

CROS_Comm_dlg::CROS_Comm_dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CROS_Comm_dlg::IDD, pParent)
	, m_ROS_MASTER_txt(_T("Enter the ROS_MASTER_URI"))
	, m_check_env_vars(TRUE)
	, m_ROS_IP_txt(_T("IP address of this PC"))
	, m_ros_connected(false)
{
	//Create(IDD_ROS_COMM_DIALOG,NULL);

}

CROS_Comm_dlg::CROS_Comm_dlg(CWnd* pParent,CNode* node)
	: CDialogEx(CROS_Comm_dlg::IDD, pParent)
	, m_ROS_MASTER_txt(_T("Enter the ROS_MASTER_URI"))
	, m_check_env_vars(TRUE)
	, m_ROS_IP_txt(_T("IP address of this PC"))
	, m_ros_connected(false)
{
	//Create(IDD_ROS_COMM_DIALOG,NULL);

}

CROS_Comm_dlg::~CROS_Comm_dlg()
{
}

BOOL CROS_Comm_dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_buttonConnect.SetWindowTextA(strButtonConnect);
	m_ros_conn.SetWindowTextA(strStatus);

	if(m_check_env_vars){
		m_edit_master_uri.EnableWindow(FALSE);
		m_edit_ros_ip.EnableWindow(FALSE);
	}else{
		m_edit_master_uri.EnableWindow(TRUE);
		m_edit_ros_ip.EnableWindow(TRUE);
	}

	return TRUE;
}

void CROS_Comm_dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ROS_MASTER, m_ROS_MASTER_txt);
	DDX_Check(pDX, IDC_CHECK_ENV_VARS, m_check_env_vars);
	DDX_Text(pDX, IDC_EDIT_ROS_IP, m_ROS_IP_txt);
	DDX_Control(pDX, IDC_EDIT_ROS_MASTER, m_edit_master_uri);
	DDX_Control(pDX, IDC_EDIT_ROS_IP, m_edit_ros_ip);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_buttonConnect);
	DDX_Control(pDX, IDC_STATIC_ROS_STATUS_VALUE, m_ros_conn);
}


BEGIN_MESSAGE_MAP(CROS_Comm_dlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_ENV_VARS, &CROS_Comm_dlg::OnBnClickedCheckEnvVars)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CROS_Comm_dlg::OnBnClickedButtonConnect)
END_MESSAGE_MAP()







// This function sets the ROS_MASTER_URI
void CROS_Comm_dlg::setMasterURI(CString& master_txt)
{
	m_ROS_MASTER_txt = master_txt;
}


// set the ROS IP
void CROS_Comm_dlg::setROSIP(CString& ip_addr)
{
	m_ROS_IP_txt = ip_addr;
}


// set for using the environment variables
void CROS_Comm_dlg::setEnvVars(bool r)
{
	m_check_env_vars = r;
}


// get the ROS Master URI
CString CROS_Comm_dlg::getMasterURI(void)
{
	return m_ROS_MASTER_txt.Trim();
}


// get the ROS IP address
CString CROS_Comm_dlg::getROSIP(void)
{
	return m_ROS_IP_txt.Trim();
}


// get the boolean using the environment variables
bool CROS_Comm_dlg::getEnvVars(void)
{
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

void CROS_Comm_dlg::setROSNode(CNode* r)
{
	node = r;
}

CNode* CROS_Comm_dlg::getROSNode()
{
	return node;
}

bool CROS_Comm_dlg::getROSConn()
{
	return m_ros_connected;

}


void CROS_Comm_dlg::showNoMasterMessage()
{
	MessageBox(_T("Couldn't find the ros master."),_T("Error"),MB_ICONERROR|MB_OK);
}

void CROS_Comm_dlg::OnBnClickedButtonConnect()
{
	UpdateData(TRUE);

	CString caption;
	m_buttonConnect.GetWindowTextA(caption);

	if (caption.Compare(_T("Connect"))==0){
		// I want to connect
        if (m_check_env_vars) {
            // use environment is checked
			m_edit_master_uri.EnableWindow(FALSE);
			m_edit_ros_ip.EnableWindow(FALSE);
            if ( !node->on_init() ) {
                showNoMasterMessage();                
				m_buttonConnect.SetWindowTextA(_T("Connect"));
                //m_check_env_vars=true;

                node->on_end();
                status = _T("Node disconnected from ROS");
				m_ros_conn.SetWindowTextA(_T("disconnected"));
                m_ros_connected=false;
            } else {
                //m_check_env_vars=false;
				m_buttonConnect.SetWindowTextA(_T("Disconnect"));
                status = "Node connected to ROS";
				m_ros_conn.SetWindowTextA(_T("connected"));
                m_ros_connected=true;

                //qnode->log(QNode::Info,status);
                //Q_EMIT rosConnected(bstatus);
            }
        } else {
			m_edit_master_uri.EnableWindow(TRUE);
			m_edit_ros_ip.EnableWindow(TRUE);
            if (!node->on_init(m_ROS_MASTER_txt,m_ROS_IP_txt)){
                showNoMasterMessage();                
				m_buttonConnect.SetWindowTextA(_T("Connect"));
                //m_check_env_vars=true;

                node->on_end();
                status = _T("Node disconnected from ROS");
				m_ros_conn.SetWindowTextA(_T("disconnected"));
                m_ros_connected=false;

            } else {
                //m_check_env_vars=false;
				m_buttonConnect.SetWindowTextA(_T("Disconnect"));
                status = "Node connected to ROS";
				m_ros_conn.SetWindowTextA(_T("connected"));
                m_ros_connected=true;

                //qnode->log(QNode::Info,status);
                //Q_EMIT rosConnected(bstatus);
            }
		}
	}else{
// I want to disconnect
			m_buttonConnect.SetWindowTextA(_T("Connect"));
            //m_check_env_vars=true;

            node->on_end();
            status = _T("Node disconnected from ROS");
			m_ros_conn.SetWindowTextA(_T("disconnected"));
            m_ros_connected=false;

        //node->log(QNode::Info,status);
        //Q_EMIT rosConnected(bstatus);

    }

}
