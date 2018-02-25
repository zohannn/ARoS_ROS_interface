// Node.cpp : file di implementazione
//

#include "../include/stdafx.h"
#include "../include/ARoS_ROS_interface.h"
#include "../include/Node.h"


// CNode

IMPLEMENT_DYNCREATE(CNode, CWinThread)

CNode::CNode()
{
	// logging
    init_log();
    logging::add_common_attributes();
}

CNode::~CNode()
{
	on_end();
    //wait();
}

BOOL CNode::InitInstance()
{
	//  eseguire l'inizializzazione dei singoli thread
	//m_pMainWnd = new CROS_Comm_dlg;
	//m_pMainWnd->SetForegroundWindow();
	//m_pMainWnd->ShowWindow(SW_SHOW);
	
	return TRUE;
}

bool CNode::on_init()
{
   node_name = _T("ARoS");
   ros::init(__argc,__argv,node_name.GetString());
	if ( ! ros::master::check() ) {
		return false;
	}
    ros::start();
    //start();
	ResumeThread();
	//WaitForSingleObject(m_hThread,INFINITE); // wait and do not go to run()
	return true;
}

bool CNode::on_init(CString master,CString ip)
{
	node_name = _T("ARoS");
	std::map<std::string,std::string> remappings;
	remappings["__master"] = master.GetString();
	remappings["__hostname"] = ip.GetString();
	ros::init(remappings,node_name.GetString());
	if ( ! ros::master::check() ) {
		return false;
	}
    ros::start();
    //start();
	ResumeThread();
	//WaitForSingleObject(m_hThread,INFINITE); // wait and do not go to run()
	return true;
}

bool CNode::on_end()
{
	if(ros::isStarted()){
		ros::shutdown();
		return ros::isShuttingDown();
	}else{
		return true;
	}
}

int CNode::Run()
{
	
	//TRACE(_T("Run called\n"));		
	//ros::Rate loop_rate(0.5);
    //ros::NodeHandle node;

    while ( ros::ok() ) {} // infinite loop while ros is running

    ros::spinOnce(); // handles ROS messages
    //std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
    //Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
	return 0; 
}


CString CNode::getNodeName()
{
	return node_name;
}

void CNode::setNodeName(CString name)
{
	node_name=name;
}
int CNode::ExitInstance()
{
	on_end();
	return CWinThread::ExitInstance();
}

const char * CNode::WinGetEnv(const char * name)
{
    const DWORD buffSize = 65535;
    static char buffer[buffSize];
    if (GetEnvironmentVariableA(name, buffer, buffSize))
    {
        return buffer;
    }
    else
    {
        return 0;
    }
}

void CNode::init_log()
{
	logging::add_file_log
    (
        keywords::file_name = "QNode_%N.log",                                        /*< file name pattern >*/
        keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
        keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0,0,0), /*< ...or at midnight >*/
        keywords::format = "[%TimeStamp%]: %Message%",                                 /*< log record format >*/
        keywords::target = "Boost_logs"
    );

    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );
}

BEGIN_MESSAGE_MAP(CNode, CWinThread)
END_MESSAGE_MAP()


// gestori di messaggi CNode
