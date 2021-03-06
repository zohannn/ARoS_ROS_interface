
// ARoS_ros_interface.cpp : Defines the class behaviors for the application.
//

#include "../include/stdafx.h"
#include "../include/ARoS_ros_interface.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

#include <vld.h>//Visual Leak Detector

// CARoS_ros_interfaceApp

BEGIN_MESSAGE_MAP(CARoS_ros_interfaceApp, CWinApp)
	//ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
	ON_COMMAND(ID_ABOUT_AROSINTERFACE, &CARoS_ros_interfaceApp::OnAboutArosinterface)
	ON_COMMAND(ID_ROSCONNECT, &CARoS_ros_interfaceApp::OnRosConnect)
	ON_COMMAND(ID_YARP_UPPERLIMB, &CARoS_ros_interfaceApp::OnYarpUpperlimb)
	ON_COMMAND(ID_YARP_VISION, &CARoS_ros_interfaceApp::OnYarpVision)
END_MESSAGE_MAP()


// CARoS_ros_interfaceApp construction

CARoS_ros_interfaceApp::CARoS_ros_interfaceApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance


	upperlimb_mode = 0; //RIGHT UPPER LIMB
	switch(upperlimb_mode){
		case 0: default: str_upperlimb_server = "/aros/upperlimb_right"; break;
		case 1: str_upperlimb_server = "/aros/upperlimb_left"; break;
		case 2: str_upperlimb_server = "/aros/upperlimbs"; break;
	}

	str_vision_server = "/aros/vision";
	look_home.resize(3);
	look_home.at(0) = 200.0f; //x
	look_home.at(1) = 0.0f; //y
	look_home.at(2) = 130.60f; //z

	// ROS NODE
	ros_node = boost::make_shared<CNode>();
	ros_node->CreateThread(CREATE_SUSPENDED);
	ros_node->m_bAutoDelete=false;
	ros_comm_dlg.setROSNode(ros_node);
	main_dlg.setROSNode(ros_node);


	yarp_upperlimb = new CYarpCommInterface("/aros_ros_interface/upperlimb",str_upperlimb_server.c_str());
	main_dlg.setYARPUpperlimb(yarp_upperlimb);

	yarp_vision = new CYarpCommInterface("/aros_ros_interface/vision",str_vision_server.c_str());
	main_dlg.setYARPVision(yarp_vision);

	b_ros_connected = false;
	b_env_vars = true;
	ros_master = _T("Enter the ROS_MASTER_URI");
	ros_ip = _T("IP address of this PC");


	b_yarp_upperlimb_states = false;
	b_yarp_vision_states = false;

	// signals
	ros_node->sig_log.connect(boost::bind(&CARoS_ros_interfaceDlg::addLogLine,&main_dlg,_1));
	ros_node->sig_joints.connect(boost::bind(&CARoS_ros_interfaceApp::setJoints,this,_1));
	ros_node->sig_open_close.connect(boost::bind(&CARoS_ros_interfaceApp::open_close_BH,this,_1));
	yarp_upperlimb->sig_joints_update.connect(boost::bind(&CARoS_ros_interfaceApp::updateJoints,this));
	yarp_upperlimb->sig_vision_update.connect(boost::bind(&CARoS_ros_interfaceApp::updateVision,this));
	

	// buffers
	this->samples_upperlimb_pos = 0;
	this->samples_upperlimb_vel = 0;
	this->N_filter_length = 5;
	this->buffer_size = 11;
	this->upperlimb_pos_buff = boost::make_shared<CircularBuffers<float>>(this->buffer_size, this->N_filter_length);
	this->upperlimb_vel_buff = boost::make_shared<CircularBuffers<float>>(this->buffer_size, this->N_filter_length);

}

CARoS_ros_interfaceApp::~CARoS_ros_interfaceApp()
{

}


// The one and only CARoS_ros_interfaceApp object

CARoS_ros_interfaceApp theApp;


// CARoS_ros_interfaceApp initialization

BOOL CARoS_ros_interfaceApp::InitInstance()
{
	yarp::os::Network yarp_net; //Initialize the YARP network

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	
	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("UM:DEI - Mobile and Anthropomorphic Robotics Laboratory"));


	m_pMainWnd = &main_dlg;
	INT_PTR nResponse = main_dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	if(yarp_upperlimb->connected){
		yarp_upperlimb->close();
		yarp_upperlimb->connected=false;
		yarp_upperlimb->stop_joint_states_receiver();
		b_yarp_upperlimb_states = false;
	}
	delete yarp_upperlimb;
	yarp_upperlimb = nullptr;

	if(yarp_vision->connected){
		yarp_vision->close();
		yarp_vision->connected=false;
		b_yarp_vision_states = false;
	}
	delete yarp_vision;
	yarp_vision = nullptr;

	yarp_net.fini(); // Finilize the YARP network

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


BOOL CARoS_ros_interfaceApp::ExitInstance()
{

	CWinApp::ExitInstance();

	return FALSE;

}



void CARoS_ros_interfaceApp::OnAboutArosinterface()
{	
	about_dlg.DoModal();
}


void CARoS_ros_interfaceApp::OnRosConnect()
{
	if(b_ros_connected){
		ros_comm_dlg.strStatus=_T("connected");
		ros_comm_dlg.strButtonConnect=_T("Disconnect");
	}else{
		ros_comm_dlg.strStatus=_T("disconnected");
		ros_comm_dlg.strButtonConnect=_T("Connect");
	}
	ros_comm_dlg.setEnvVars(b_env_vars);
	ros_comm_dlg.setMasterURI(ros_master);
	ros_comm_dlg.setROSIP(ros_ip);
	ros_comm_dlg.DoModal();
	b_ros_connected = ros_comm_dlg.getROSConn();
	b_env_vars = ros_comm_dlg.getEnvVars();
	ros_master = ros_comm_dlg.getMasterURI();
	ros_ip =ros_comm_dlg.getROSIP();

	CString status;
	if(b_ros_connected)
	{
		status = _T("Node is connected to the ROS Master");
		this->ros_node->advertiseSrvOpenCloseBH();
	}else
	{
		status = _T("Node is NOT connected to the ROS Master");
	}
	main_dlg.addLogLine(status);

}


void CARoS_ros_interfaceApp::OnYarpUpperlimb()
{
	
	CString status;
	if(!yarp_upperlimb->connected)
	{ // not connected
		if(yarp_upperlimb->open())
		{
			yarp_upperlimb->connected=true;
			status = _T("Node is connected to the Upperlimb YARP server");
			//main_dlg.EnableJointsLowPassFilterGroup(true);
			m_pMainWnd->GetMenu()->CheckMenuItem(ID_YARP_UPPERLIMB,MF_CHECKED|MF_BYCOMMAND);
			switch(upperlimb_mode){
				case 0:	default: main_dlg.EnableRightUpperLimbGroup(true); main_dlg.EnableLeftUpperLimbGroup(false); break;
				case 1: main_dlg.EnableRightUpperLimbGroup(false); main_dlg.EnableLeftUpperLimbGroup(true); break;
				case 2: main_dlg.EnableRightUpperLimbGroup(true); main_dlg.EnableLeftUpperLimbGroup(true); break;
			}
			main_dlg.EnableExecModeGroup(true);
		}else{
			yarp_upperlimb->connected=false;
			status = _T("Node could NOT connect to the Upperlimb YARP server");
			m_pMainWnd->GetMenu()->CheckMenuItem(ID_YARP_UPPERLIMB,MF_UNCHECKED|MF_BYCOMMAND);
			main_dlg.EnableRightUpperLimbGroup(false); 
			main_dlg.EnableLeftUpperLimbGroup(false);
			main_dlg.EnableExecModeGroup(false);
			//main_dlg.EnableJointsLowPassFilterGroup(false);
		}
	}else{
		// connected
		yarp_upperlimb->close();
		yarp_upperlimb->connected=false;
		status = _T("Connection closed with the Upperlimb YARP server");
		m_pMainWnd->GetMenu()->CheckMenuItem(ID_YARP_UPPERLIMB,MF_UNCHECKED|MF_BYCOMMAND);
		main_dlg.EnableRightUpperLimbGroup(false); 
		main_dlg.EnableLeftUpperLimbGroup(false);
		main_dlg.EnableExecModeGroup(false);
		//main_dlg.EnableJointsLowPassFilterGroup(false);
	}
	main_dlg.addLogLine(status);
	if(yarp_upperlimb->connected)
	{
		if(yarp_upperlimb->start_joint_states_recever(str_upperlimb_server+"/joint_states"))
		{
			main_dlg.addLogLine(_T("Joint States receiver started"));
			b_yarp_upperlimb_states = true;	
			//main_dlg.start_joint_updating();
			update_joints_values_thd = boost::thread(boost::bind(&CARoS_ros_interfaceApp::updateUpperLimbValues, this));

		}
	}else{
		yarp_upperlimb->stop_joint_states_receiver();
		b_yarp_upperlimb_states = false;	
		//main_dlg.stop_joint_updating();
		main_dlg.addLogLine(_T("Joint States receiver stopped"));
	}
}

void CARoS_ros_interfaceApp::OnYarpVision()
{	
	CString status;
	if(!yarp_vision->connected)
	{ // not connected
		if(yarp_vision->open())
		{
			yarp_vision->connected=true;
			status = _T("Node is connected to the Vision YARP server");
			m_pMainWnd->GetMenu()->CheckMenuItem(ID_YARP_VISION,MF_CHECKED|MF_BYCOMMAND);

			//yarp_vision->lookAtPosition(look_home.at(0),look_home.at(1),look_home.at(2));
		}else{
			yarp_vision->connected=false;
			status = _T("Node could NOT connect to the Vision YARP server");
			m_pMainWnd->GetMenu()->CheckMenuItem(ID_YARP_VISION,MF_UNCHECKED|MF_BYCOMMAND);
		}
	}else{
		// connected
		yarp_vision->close();
		yarp_vision->connected=false;
		status = _T("Connection closed with the Vision YARP server");
		m_pMainWnd->GetMenu()->CheckMenuItem(ID_YARP_VISION,MF_UNCHECKED|MF_BYCOMMAND);
	}
	main_dlg.addLogLine(status);
	if(yarp_vision->connected)
	{// connected
			main_dlg.addLogLine(_T("Vision updating started"));
			b_yarp_vision_states = true;	
			main_dlg.start_vision_updating();
			main_dlg.EnableVisionGroup(true);
			update_vision_values_thd = boost::thread(boost::bind(&CARoS_ros_interfaceApp::updateVisionValues, this));

	}else{
		// disconnected
		b_yarp_vision_states = false;	
		main_dlg.stop_vision_updating();
		main_dlg.EnableVisionGroup(false);
		main_dlg.addLogLine(_T("Vision updating stopped"));
	}
}

int CARoS_ros_interfaceApp::binomialCoeff(int n, int k)
{
    // https://www.geeksforgeeks.org/binomial-coefficient-dp-9/

    if (k>=0 && k<=n)
    {
		int **C;
		// allocate memory
		C = new int*[n+1]();
		for (int ii = 0; ii < (n+1); ++ii)
			C[ii] = new int[k+1]();
		//int C[n + 1][k + 1];
        int i, j;

        // Caculate value of Binomial Coefficient
        // in bottom up manner
        for (i = 0; i <= n; i++)
        {
            for (j = 0; j <= min(i, k); j++)
            {
                // Base Cases
                if (j == 0 || j == i)
                    C[i][j] = 1;

                // Calculate value using previosly
                // stored values
                else
                    C[i][j] = C[i - 1][j - 1] +
                              C[i - 1][j];
            }
        }

		int cc = C[n][k];
		// don't forget to free your memory!
		for (int ii = 0; ii < (n+1); ++ii)
			delete[] C[ii];
		delete[] C;

		// return 
        return cc;
    }else{
        return 0;
    }
}

float CARoS_ros_interfaceApp::getNoiseRobustDerivate(int N, float h, std::deque<float>& buff)
{
    // http://www.holoborodko.com/pavel/numerical-methods/numerical-derivative/smooth-low-noise-differentiators/#noiserobust_2
    static const std::runtime_error ex_cap(std::string("The capacity of the buffer differs the given filter length"));
    static const std::runtime_error ex_length(std::string("The filter length must be greater or equal to 5"));
    if(N>=5)
    {
        if(N==buff.size())
        {
            int M = (N-1)/2;
            int m = (N-3)/2;
            float sum=0.0;
            for(int k=1;k<=M;++k)
            {
                float c1 = this->binomialCoeff(2*m,(m-k+1));
                float c2 = this->binomialCoeff(2*m,(m-k-1));
                float ck = (1/pow(static_cast<float>(2),static_cast<float>((2*m)+1)))*(c1-c2);
                sum+=ck*(buff[M+k]-buff[M-k]);
            }
            float der = sum/h;
            return der;
        }else{
            throw ex_cap;
        }
    }else{
        throw ex_length;
    }
}

void CARoS_ros_interfaceApp::updateJoints()
{
	float time_step = 5; // ms
	Joint_States jstate;	
	//jstate.setLowPassFilters(main_dlg.getJointsLpfFreq(),main_dlg.getJointsLpfDt());
		if(yarp_upperlimb->getJointState(jstate))
		{
			main_dlg.updateJointValuesAsync(jstate);
			if(b_ros_connected){
				//std::vector<float> ppos; jstate.getFilteredPositions(ppos);
				// derive velocity 				
				this->upperlimb_pos_buff->push(jstate.position);
				if(this->samples_upperlimb_pos==this->N_filter_length-1 && this->upperlimb_pos_buff->full()){
					for(size_t i=0; i< jstate.position.size();++i)
					{
						jstate.velocity_der.at(i) = this->getNoiseRobustDerivate(this->N_filter_length,time_step,this->upperlimb_pos_buff->at(i));
					}
				}else{this->samples_upperlimb_pos++;}

				// derive acceleration
				if(this->samples_upperlimb_pos==this->N_filter_length-1){
					this->upperlimb_vel_buff->push(jstate.velocity_der);
					if(this->samples_upperlimb_vel==this->N_filter_length-1 && this->upperlimb_vel_buff->full()){
						for(size_t i=0; i< jstate.velocity_der.size();++i)
						{
							jstate.acceleration_der.at(i) = this->getNoiseRobustDerivate(this->N_filter_length,time_step,this->upperlimb_vel_buff->at(i));
						}
					}else{this->samples_upperlimb_vel++;}
				}

				// send to the ROS network
				this->ros_node->advertiseJoints(jstate,"joints_state");
			}
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(time_step)); // loop rate 
}

void CARoS_ros_interfaceApp::updateVision()
{
	float time_step = 5; // ms
	if(yarp_vision->getVisionInfo() && b_ros_connected)
	{
		// send to the ROS network
		//objects
		this->ros_node->advertiseRedColumn(yarp_vision->getRedColumn(), "object_pose/red_column");
		this->ros_node->advertiseGreenColumn(yarp_vision->getGreenColumn(), "object_pose/green_column");
		this->ros_node->advertiseBlueColumn(yarp_vision->getBlueColumn(), "object_pose/blue_column");
		this->ros_node->advertiseMagentaColumn(yarp_vision->getMagentaColumn(), "object_pose/magenta_column");

		//target
		std::vector<float>tar_ppos; yarp_vision->getRedColumn()->getTarPos(tar_ppos);
		Quaternionf tar_q_oor = yarp_vision->getRedColumn()->getTarQOr();
		this->ros_node->advertiseTarget(tar_ppos,tar_q_oor, "target_pose");
	}
	boost::this_thread::sleep(boost::posix_time::milliseconds(time_step)); // loop rate 
}

void CARoS_ros_interfaceApp::updateUpperLimbValues()
{	
	while(b_yarp_upperlimb_states)
	{
		this->updateJoints();
	} // while
}

void CARoS_ros_interfaceApp::updateVisionValues()
{
	while(b_yarp_vision_states)
	{
		this->updateVision();
	}// while
}

void CARoS_ros_interfaceApp::setJoints(std::vector<float>& arr_vel)
{
	/*
	CString str("Velocities: ");
	for(size_t i = 0; i < arr_vel.size(); ++i)
	{
		CString tmp_str; tmp_str.Format(_T("%g "), arr_vel.at(i));
		str = str +tmp_str;
		
	}
	main_dlg.addLogLine(str);
	*/
	if(b_yarp_upperlimb_states && b_ros_connected && !arr_vel.empty()){
		this->yarp_upperlimb->setRightVelocities(arr_vel);
		//std::vector<float> arr_hand_pos(arr_vel.end() - 4, arr_vel.end());
		//this->yarp_upperlimb->moveRightHandFingers(arr_hand_pos);
	}
}

bool CARoS_ros_interfaceApp::open_close_BH(bool close)
{
	if(close){
		return this->yarp_upperlimb->closeRightHand();
	}else{
		return this->yarp_upperlimb->openRightHandHome();
	}
}






