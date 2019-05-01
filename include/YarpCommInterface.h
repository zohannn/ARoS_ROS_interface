// YarpCommInterface.h: YARP communication for the ARoS ROS interface.
//
//////////////////////////////////////////////////////////////////////

#ifndef _YARPCOMMINTERFACE_H_
#define _YARPCOMMINTERFACE_H_


#ifndef DEG_TO_RAD_F
#define DEG_TO_RAD_F 0.017453292519943295769236907684886f
#endif

#ifndef RAD_TO_DEG_F
#define RAD_TO_DEG_F 57.295779513082320876798154814105f
#endif

#include <queue>
#include "../include/stdafx.h"
#include <boost\thread.hpp>
#include <boost\atomic.hpp>
#include <YarpCommunication.h>
#include <YarpConnector.h>
#include <yarp/os/Network.h>
#include "Joint_States.h"
#include "common.h"

// Boost logging
#include <boost\log\core.hpp>
#include <boost\log\trivial.hpp>
#include <boost\log\expressions.hpp>
#include <boost\log\sinks\text_file_backend.hpp>
#include <boost\log\utility\setup\file.hpp>
#include <boost\log\utility\setup\common_attributes.hpp>
#include <boost\log\sources\severity_logger.hpp>
#include <boost\log\sources\record_ostream.hpp>

// Boost Signals
#include <boost/signal.hpp>

class CYarpCommInterface : public CYarpCommunication
{
public:
	CYarpCommInterface( const std::string name, const std::string peerName = "" );

	~CYarpCommInterface();

	void Process( CMessage &msgIn, CMessage &msgOut, void *private_data = nullptr );

public:
	//CMessage msg;

	//Flag
	bool connected;
	boost::atomic<bool> joint_states_connected;
	bool joint_states_opened;
	std::string yarp_name;
	std::string yarp_name_joints_state_receiver;
	std::string yarp_name_sender;
	boost::signal<void ()> sig_update;
	//int flag_all;// 1 : OK; 0: Error
	//int flag_comm;
	//int flag_dev;

	//Vision
	//int neck;//Allows to trigger the rotation
	//float v_pan,v_tilt;

	// Upper limb: Arm + Hand
	std::vector<float> current_right_posture; // current posture of the right upper limb 
	std::vector<float> current_right_velocities; // current velocities of the right upper limb 
	std::vector<float> w_max; // max angular velocity of the joints
	std::vector<float> upperlimb_offset; //aros upperlimb positions offset
	std::queue<Joint_States> joint_states_queue;
	boost::mutex joint_states_queue_mtx;
	boost::condition_variable joint_states_queue_cv;
	boost::thread joint_states_receiver_thd;
	void joint_states_receiver_job();
	boost::atomic<bool> stop_receiver;
	yarp::os::BufferedPort<Joint_States> joint_states_in_port;
	bool start_joint_states_recever(std::string sender_name);
	void stop_joint_states_receiver();
	bool getJointState(Joint_States& jstate);

	bool homeArmJoint(int nj); // home the joint nj
	bool softStopRightArm(); // soft stop right arm
	bool initRightHand(); // init right hand
	bool openRightHand(); // open right hand
	bool moveRightHandFingers(std::vector<float> fingers_pos); // move the fingers of the hand
	bool openRightHandPark(); // open the right hand and park the fingers
	bool openRightHandHome(); // open the right hand and home the fingers
	bool openRightHand(float pos);
	bool closeRightHand(); // close the right hand
	bool homeRightUpperLimb(); // home the right upperlimb
	bool parkRightUpperLimb(); // park the right upperlimb
	bool getCurrentRightPositions(std::vector<float>& curr_pos); //get the current positions of the right upper limb
	bool getCurrentRightVelocities(std::vector<float>& curr_vel); //get the current velocities of the right upper limb

	// stop the execution of the trajectory
	bool sendUpperlimbStop();
	void stopMovement();
	// wait asynchronously for the end of the execution
	boost::atomic<bool> async_finished; boost::atomic<bool> vel_async_finished; boost::atomic<bool> stop_mov;
	boost::thread wait_async_thd; boost::thread vel_wait_async_thd;
	boost::mutex async_finished_mtx; boost::mutex vel_async_finished_mtx;
	boost::condition_variable async_finished_cv; boost::condition_variable vel_async_finished_cv;
	std::string async_str; std::string vel_async_str;
	bool b_error;
	void trajectoryFinishedAsync(CMessage &msgIn);
	void velTrajectoryFinishedAsync(CMessage &msgIn);
	bool sendWaitTrajEndAsync();
	bool sendWaitVelTrajEndAsync();
	void waitTrajEndAsync();
	void waitVelTrajEndAsync();
	// add a trajectory to the UpperLimb Server
	bool addTrajectory(std::vector<float> fPos, int nsteps, bool b_arm, bool b_hand);
	// get the direct trajectory
	bool directTrajectory(std::vector<float> init_posture,
						  std::vector<float> goal_posture,
						  int steps,
						  std::vector<float> w_max,
						  std::vector<std::vector<float>>& traj);
	// get the direct movement 
	bool directMovement(std::vector<float> init_posture,
						std::vector<float> goal_posture,
						int steps,
						std::vector<std::vector<float>>& traj);

	void waitAsync(); // wait asynchronously for the execution of the movement
	void waitVelAsync(); // wait asynchronously for the execution of the movement

	bool setRightVelocities(std::vector<float> fVel); // set the velocities of the upperlimb
	bool addVelTrajectoryExec(std::vector<std::vector<float>>& j_vel_traj); // set the trajectory of velocities 
	bool addVelTrajectory(std::vector<std::vector<float>>& fVel); // set the trajectory of velocities 
	void interpolate(std::vector<float> j_a,std::vector<float> j_b,float m,std::vector<float>& j_x);
	float l2_norm(std::vector<float>& v);
	bool inRange(float a,float b, float x);

	// VisionBot
	bool lookAtPosition(float dX, float dY, float dZ);
	bool getVisionInfo();
	bool getObjectPos(int type);
	bool getObjectOr(int type);

	// logging
	void init_logging();
	boost::log::sources::severity_logger< boost::log::trivial::severity_level > lg; // logger

	//float j_pos[7];//Joint position values
	//float j_vel[7];//Joint velocity values
	//float gripper;// Gripper value
	//float fW[6];//Force at wrist

	/*
	//Object detection
	int object_type;
	int refresh;//Start from the first element of obs array
	float obs[20][4];// xyz+size	
	int obs_num; // Obs number
	int obs_f[20];// 1-Normal <0-Others
	//Target detection
	int mode;//1-obstacle 2-target
	float target[4][3];// x,y,z
	int tar_num;

	//Hand
	float jpos[4];  //0.6857;

	//Voice
	int command;
	//Mobile platform
	float mvel[2];
	//Obstacle from laser range finder
	float obs_g[85];
	*/

};

#endif // _YARPCOMMINTERFACE_H_
