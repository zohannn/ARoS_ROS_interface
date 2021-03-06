// Node.cpp : file di implementazione
//

#include "../include/stdafx.h"
#include "../include/ARoS_ROS_interface.h"
#include "../include/Node.h"

#include <stdlib.h>

// CNode

IMPLEMENT_DYNCREATE(CNode, CWinThread)

CNode::CNode()
{
	connected=false;
	// logging
    //init_log();
    //logging::add_common_attributes();
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
   std::map<std::string,std::string> remappings;
   //ros::init(__argc,__argv,"ARoS");
   const char* master = this->WinGetEnv("ROS_MASTER_URI");
   remappings["__master"] = master;
   const char* ip = this->WinGetEnv("ROS_IP");    
   remappings["__hostname"] = ip;
   ros::init(remappings,"ARoS");
	if ( ! ros::master::check() ) {
		connected=false;
		return false;
	}
    ros::start();
    //start();
	ResumeThread();
	//WaitForSingleObject(m_hThread,INFINITE); // wait and do not go to run()
	connected=true;
	return true;
}

bool CNode::on_init(CString master,CString ip)
{
	node_name = _T("ARoS");
	std::map<std::string,std::string> remappings;
	remappings["__master"] = CT2A(master);
	remappings["__hostname"] = CT2A(ip);
	ros::init(remappings,"ARoS");
	if ( ! ros::master::check() ) {
		connected=false;
		return false;
	}
    ros::start();
    //start();
	ResumeThread();
	//WaitForSingleObject(m_hThread,INFINITE); // wait and do not go to run()
	connected=true;
	return true;
}

bool CNode::on_end()
{
	if(ros::isStarted()){
		ros::shutdown();
		connected=false;
		return ros::isShuttingDown();
	}else{
		connected=false;
		return true;
	}
}

bool CNode::isConnected()
{
	return this->connected;
}

int CNode::Run()
{
	
	//TRACE(_T("Run called\n"));		
	//ros::Rate loop_rate(0.5);
    //ros::NodeHandle node;
    while ( ros::ok() ) {


		ros::spinOnce(); // handles ROS messages
	
	} // infinite loop while ros is running

    ros::spinOnce(); // handles ROS messages
	on_end();
    //std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
    //Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
	return 0; 
}
/**
UINT publishChat (LPVOID pParam)
{
  int count = 0;
  ros::Rate loop_rate(10);

  CNode* node = (CNode*)pParam;
  ros::Publisher pub = node->getPublisher();
  while (ros::ok())
  {

    std_msgs::String msg;

    std::stringstream ss;
    ss << "hello world " << count;
    msg.data = ss.str();

    //ROS_INFO("%s", msg.data.c_str());


    pub.publish(msg);

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }

  return 0;

}
*/

UINT publishRedColumn (LPVOID pParam)
{
 
  CNode* node = (CNode*)pParam;
  ros::Publisher pub = node->getRedColPublisher();
  boost::shared_ptr<Object> obj_ptr = node->getRedColPtr();

  std::vector<float> obj_ppos; obj_ptr->getPos(obj_ppos);
  Quaternionf obj_q = obj_ptr->getQOr();

  geometry_msgs::PoseStamped msg;

  msg.header.frame_id = "Red Column Pose";
  msg.pose.position.x = obj_ppos.at(0); 
  msg.pose.position.y = obj_ppos.at(1); 
  msg.pose.position.z = obj_ppos.at(2);   
  msg.pose.orientation.x = obj_q.x(); 
  msg.pose.orientation.y = obj_q.y(); 
  msg.pose.orientation.z = obj_q.z();  
  msg.pose.orientation.w = obj_q.w(); 

  pub.publish(msg);

  ros::spinOnce();

  return 0;

}

UINT publishGreenColumn (LPVOID pParam)
{

  CNode* node = (CNode*)pParam;
  ros::Publisher pub = node->getGreenColPublisher();
  boost::shared_ptr<Object> obj_ptr = node->getGreenColPtr();

  std::vector<float> obj_ppos; obj_ptr->getPos(obj_ppos);
  Quaternionf obj_q = obj_ptr->getQOr();

  geometry_msgs::PoseStamped msg;

  msg.header.frame_id = "Green Column Pose";
  msg.pose.position.x = obj_ppos.at(0); 
  msg.pose.position.y = obj_ppos.at(1); 
  msg.pose.position.z = obj_ppos.at(2);   
  msg.pose.orientation.x = obj_q.x(); 
  msg.pose.orientation.y = obj_q.y(); 
  msg.pose.orientation.z = obj_q.z();  
  msg.pose.orientation.w = obj_q.w(); 

  pub.publish(msg);

  ros::spinOnce();

  return 0;

}

UINT publishBlueColumn (LPVOID pParam)
{


  CNode* node = (CNode*)pParam;
  ros::Publisher pub = node->getBlueColPublisher();
  boost::shared_ptr<Object> obj_ptr = node->getBlueColPtr();

  std::vector<float> obj_ppos; obj_ptr->getPos(obj_ppos);
  Quaternionf obj_q = obj_ptr->getQOr();

  geometry_msgs::PoseStamped msg;

  msg.header.frame_id = "Blue Column Pose";
  msg.pose.position.x = obj_ppos.at(0); 
  msg.pose.position.y = obj_ppos.at(1); 
  msg.pose.position.z = obj_ppos.at(2);   
  msg.pose.orientation.x = obj_q.x(); 
  msg.pose.orientation.y = obj_q.y(); 
  msg.pose.orientation.z = obj_q.z();  
  msg.pose.orientation.w = obj_q.w(); 

  pub.publish(msg);

  ros::spinOnce();

  return 0;

}

UINT publishMagentaColumn (LPVOID pParam)
{

  CNode* node = (CNode*)pParam;
  ros::Publisher pub = node->getMagentaColPublisher();
  boost::shared_ptr<Object> obj_ptr = node->getMagentaColPtr();

  std::vector<float> obj_ppos; obj_ptr->getPos(obj_ppos);
  Quaternionf obj_q = obj_ptr->getQOr();

  geometry_msgs::PoseStamped msg;

  msg.header.frame_id = "Magenta Column Pose";
  msg.pose.position.x = obj_ppos.at(0); 
  msg.pose.position.y = obj_ppos.at(1); 
  msg.pose.position.z = obj_ppos.at(2);   
  msg.pose.orientation.x = obj_q.x(); 
  msg.pose.orientation.y = obj_q.y(); 
  msg.pose.orientation.z = obj_q.z();  
  msg.pose.orientation.w = obj_q.w(); 

  pub.publish(msg);

  ros::spinOnce();

  return 0;

}

UINT publishTarget (LPVOID pParam)
{
  
  CNode* node = (CNode*)pParam;

  std::vector<float> tar_ppos; node->getTargetPos(tar_ppos);
  Quaternionf tar_q_or = node->getTargetQOr();
  ros::Publisher pub = node->getTargetPublisher();

  geometry_msgs::PoseStamped msg;

  msg.header.frame_id = "Target Pose";
  msg.pose.position.x = tar_ppos.at(0); 
  msg.pose.position.y = tar_ppos.at(1); 
  msg.pose.position.z = tar_ppos.at(2);   
  msg.pose.orientation.x = tar_q_or.x(); 
  msg.pose.orientation.y = tar_q_or.y(); 
  msg.pose.orientation.z = tar_q_or.z();  
  msg.pose.orientation.w = tar_q_or.w(); 

  pub.publish(msg);

  ros::spinOnce();

  return 0;

}

UINT publishJoints (LPVOID pParam)
{
  
  CNode* node = (CNode*)pParam;
  Joint_States js; node->getJointStates(js);
  ros::Publisher pub = node->getJointStatePublisher();

  int n_joints = js.position.size();
  sensor_msgs::JointState msg;
  msg.header.frame_id = "States of the joints";
  msg.name.resize(n_joints);
  msg.position.resize(n_joints);
  msg.velocity.resize(n_joints);
  msg.effort.resize(n_joints);

  //std::vector<float> ppos; js.getFilteredPositions(ppos);

  std::stringstream ss; std::string ss_str;
  for(int i=0;i<n_joints;++i){
    ss.str(""); ss << i; ss_str = ss.str();
	msg.name[i] = "joint "+ss_str;
	msg.position[i] = js.position.at(i);
	//msg.position[i] = ppos.at(i);
	msg.velocity[i] = js.velocity_der.at(i);
	msg.effort[i] = js.acceleration_der.at(i);
  }
  
  pub.publish(msg);
  

  ros::spinOnce();

  return 0;

}


CString CNode::getNodeName()
{
	return node_name;
}

/**
ros::Publisher CNode::getPublisher()
{
	return pubChat;
}
*/

ros::Publisher CNode::getRedColPublisher()
{
	return this->pubRedColumn;
}

ros::Publisher CNode::getGreenColPublisher()
{
	return this->pubGreenColumn;
}

ros::Publisher CNode::getBlueColPublisher()
{
	return this->pubBlueColumn;
}

ros::Publisher CNode::getMagentaColPublisher()
{
	return this->pubMagentaColumn;
}

ros::Publisher CNode::getTargetPublisher()
{
	return this->pubTarget;
}

boost::shared_ptr<Object> CNode::getGreenColPtr()
{
	return this->greenColumnPtr;
}

boost::shared_ptr<Object> CNode::getBlueColPtr()
{
	return this->blueColumnPtr;
}

boost::shared_ptr<Object> CNode::getMagentaColPtr()
{
	return this->magentaColumnPtr;
}

boost::shared_ptr<Object> CNode::getRedColPtr()
{
	return this->redColumnPtr;
}

void CNode::getTargetPos(std::vector<float>& ppos)
{
	ppos = this->tar_pos;
}

Quaternionf CNode::getTargetQOr()
{
	return this->tar_q_or;
}

void CNode::getJointStates(Joint_States& js)
{
	js = this->jstate;
}

ros::Publisher CNode::getJointStatePublisher()
{
	return this->pubJoints;
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
        keywords::file_name = "ros_Node_%N.log",                                        /*< file name pattern >*/
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
/**
void CNode::chatterCallback(const std_msgs::String::ConstPtr& msg)
{
	//ROS_INFO("I heard: [%s]", msg->data.c_str());
	CString str_init("I heard: ");
	CString str_data(msg->data.c_str());
	CString str = str_init + str_data;
	sig_log(str);
}
*/
/**
void CNode::listen()
{	
	ros::NodeHandle handle_node; // handle of the ROS node
	subChat = handle_node.subscribe("chatter",1000,&CNode::chatterCallback,this);
}
*/


/*
void CNode::advertise(std::string topic)
{

	ros::NodeHandle handle_node; // handle of the ROS node
	pubChat = handle_node.advertise<std_msgs::String>(topic, 1000);

	AfxBeginThread(publishChat,this);

}
*/

void CNode::subscribeSetJoints(std::string topic)
{
	ros::NodeHandle handle_node; // handle of the ROS node
	this->subJoints = handle_node.subscribe(topic,1,&CNode::SetJointsCallback,this);
}

void CNode::unsubscribeSetJoints()
{
	this->subJoints.shutdown();
}


void CNode::SetJointsCallback(const std_msgs::Float32MultiArray::ConstPtr& arr)
{
	/*
	float arr_vel[11];
	int i=0;
	for(std::vector<float>::const_iterator it = arr->data.begin(); it != arr->data.end(); ++it)
	{
		arr_vel[i] = *it;
		i++;
	}
	*/
	std::vector<float> arr_vel(arr->data.begin(),arr->data.end());
	sig_joints(arr_vel);	
}


void CNode::advertiseRedColumn(boost::shared_ptr<Object> obj, std::string topic)
{
	ros::NodeHandle handle_node; // handle of the ROS node
	this->redColumnPtr = obj;
    CT2CA pszConvertedAnsiString (this->node_name); std::string node_name_str(pszConvertedAnsiString);
	this->pubRedColumn = handle_node.advertise<geometry_msgs::PoseStamped>("/"+node_name_str+"/"+topic, 1);
	AfxBeginThread(publishRedColumn,this);
}

void CNode::advertiseGreenColumn(boost::shared_ptr<Object> obj, std::string topic)
{
	ros::NodeHandle handle_node; // handle of the ROS node
	this->greenColumnPtr = obj;
    CT2CA pszConvertedAnsiString (this->node_name); std::string node_name_str(pszConvertedAnsiString);
	this->pubGreenColumn = handle_node.advertise<geometry_msgs::PoseStamped>("/"+node_name_str+"/"+topic, 1);
	AfxBeginThread(publishGreenColumn,this);
}

void CNode::advertiseBlueColumn(boost::shared_ptr<Object> obj, std::string topic)
{
	ros::NodeHandle handle_node; // handle of the ROS node
	this->blueColumnPtr = obj;
    CT2CA pszConvertedAnsiString (this->node_name); std::string node_name_str(pszConvertedAnsiString);
	this->pubBlueColumn = handle_node.advertise<geometry_msgs::PoseStamped>("/"+node_name_str+"/"+topic, 1);
	AfxBeginThread(publishBlueColumn,this);
}

void CNode::advertiseMagentaColumn(boost::shared_ptr<Object> obj, std::string topic)
{
	ros::NodeHandle handle_node; // handle of the ROS node
	this->magentaColumnPtr = obj;
    CT2CA pszConvertedAnsiString (this->node_name); std::string node_name_str(pszConvertedAnsiString);
	this->pubMagentaColumn = handle_node.advertise<geometry_msgs::PoseStamped>("/"+node_name_str+"/"+topic, 1);
	AfxBeginThread(publishMagentaColumn,this);
}

void CNode::advertiseTarget(std::vector<float>& tar_ppos, Quaternionf& tar_q_oor, std::string topic)
{
	ros::NodeHandle handle_node; // handle of the ROS node
	this->tar_pos = tar_ppos;
	this->tar_q_or = tar_q_oor;
    CT2CA pszConvertedAnsiString (this->node_name); std::string node_name_str(pszConvertedAnsiString);
	this->pubTarget = handle_node.advertise<geometry_msgs::PoseStamped>("/"+node_name_str+"/"+topic, 1);
	AfxBeginThread(publishTarget,this);
}

void CNode::advertiseJoints(Joint_States& jstate, std::string topic)
{
	ros::NodeHandle handle_node; // handle of the ROS node
	this->jstate = jstate;
	CT2CA pszConvertedAnsiString (this->node_name); std::string node_name_str(pszConvertedAnsiString);
	this->pubJoints = handle_node.advertise<sensor_msgs::JointState>("/"+node_name_str+"/"+topic, 1);
	AfxBeginThread(publishJoints,this);
}

void CNode::advertiseSrvOpenCloseBH()
{
	ros::NodeHandle handle_node; // handle of the ROS node
	CT2CA pszConvertedAnsiString (this->node_name); std::string node_name_str(pszConvertedAnsiString);
	this->srvOpenClose = handle_node.advertiseService("/"+node_name_str+"/open_close_BH_srv", &CNode::open_close_BH,this);
}

bool CNode::open_close_BH(std_srvs::OpenClose_BH::Request &req, std_srvs::OpenClose_BH::Response &res)
{
	res.success = sig_open_close(req.close);
	return res.success;
}


BEGIN_MESSAGE_MAP(CNode, CWinThread)
END_MESSAGE_MAP()


// gestori di messaggi CNode
