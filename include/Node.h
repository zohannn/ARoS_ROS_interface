#ifndef NODE_HPP
#define NODE_HPP

// ROS includes
#include <ros\ros.h>
#include <ros\network.h>
#include <std_msgs/String.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Pose.h>
#include <sensor_msgs\JointState.h>
#include <std_msgs\MultiArrayDimension.h>
#include <std_msgs\MultiArrayLayout.h>
#include <std_msgs\Float32MultiArray.h>

// Boost Logging
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

// Boost Signals
#include <boost/signal.hpp>

// Boost shared Ptr
#include <boost\shared_ptr.hpp>

// Object
#include "Object.h"

// Joints 
#include "Joint_States.h"

// CNode
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;


using namespace logging::trivial;

class CNode : public CWinThread
{
	DECLARE_DYNCREATE(CNode)

//protected:
public:
	CNode();           
	virtual ~CNode();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	bool on_init();
	bool on_init(CString master,CString ip);
	bool on_end();
	int Run();
	CString getNodeName();
	void setNodeName(CString name);
	//void listen();
	//void advertise(std::string topic);
	boost::signal< void(CString) > sig_log;
	boost::signal< void(std::vector<float>&) > sig_joints;
	//ros::Publisher getPublisher();

	// red column publishing
	boost::shared_ptr<Object> getRedColPtr(); ros::Publisher getRedColPublisher();
	void advertiseRedColumn(boost::shared_ptr<Object> obj, std::string topic);

	// green column publishing
	boost::shared_ptr<Object> getGreenColPtr(); ros::Publisher getGreenColPublisher();
	void advertiseGreenColumn(boost::shared_ptr<Object> obj, std::string topic);

	// blue column publishing
	boost::shared_ptr<Object> getBlueColPtr(); ros::Publisher getBlueColPublisher();
	void advertiseBlueColumn(boost::shared_ptr<Object> obj, std::string topic);

	// magenta column publishing
	boost::shared_ptr<Object> getMagentaColPtr(); ros::Publisher getMagentaColPublisher();
	void advertiseMagentaColumn(boost::shared_ptr<Object> obj, std::string topic);

	// target publishing
	void getTargetPos(std::vector<float>& ppos); Quaternionf getTargetQOr(); ros::Publisher getTargetPublisher();
	void advertiseTarget(std::vector<float>& tar_ppos, Quaternionf& tar_q_oor, std::string topic);

	// joints state publishing
	void getJointStates(Joint_States& js); ros::Publisher getJointStatePublisher();
	void advertiseJoints(Joint_States& jstate, std::string topic);
	
	// joints subscribing 
	void SetJointsCallback(const std_msgs::Float32MultiArray::ConstPtr& arr);
	void subscribeSetJoints(std::string topic);
	void unsubscribeSetJoints();

	bool isConnected();

protected:
	const char * WinGetEnv(const char * name);
	DECLARE_MESSAGE_MAP()

private:
	CString node_name;
	void init_log();
	bool connected; // true if the node is connected to the ROS network
	//void chatterCallback(const std_msgs::String::ConstPtr& msg);
	//ros::Subscriber subChat; /**< ROS subscriber for information about /chatter */
	//ros::Publisher pubChat;
	// publisher objects
	boost::shared_ptr<Object> redColumnPtr; ros::Publisher pubRedColumn;
	boost::shared_ptr<Object> greenColumnPtr; ros::Publisher pubGreenColumn;
	boost::shared_ptr<Object> blueColumnPtr; ros::Publisher pubBlueColumn;
	boost::shared_ptr<Object> magentaColumnPtr; ros::Publisher pubMagentaColumn;

	// publisher target
	std::vector<float> tar_pos; Quaternionf tar_q_or; ros::Publisher pubTarget;

	// publisher joints state
	Joint_States jstate; ros::Publisher pubJoints;

	// subscriber set_real_joints;
	ros::Subscriber subJoints;

};

#endif // NODE_HPP


