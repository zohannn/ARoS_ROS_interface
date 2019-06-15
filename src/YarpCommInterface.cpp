// YarpCommInterface.cpp: implementation of the CYarpCommInterface class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "../include/YarpCommInterface.h"
#include "../include/stdafx.h"
#include <commands.h>
#include <math.h>
#include <string>

CYarpCommInterface::CYarpCommInterface( const std::string name, const std::string peerName )
: CYarpCommunication( name, peerName, false )
{
	//Reset flags
	connected=false;
	joint_states_connected=false;
	joint_states_opened=false;
	yarp_name = name;
	yarp_name_joints_state_receiver = yarp_name+"/joint_states_receiver";

	// max angular velocities
	w_max.push_back(52.2f*DEG_TO_RAD_F); //joint 0
	w_max.push_back(52.2f*DEG_TO_RAD_F); //joint 1
	w_max.push_back(52.2f*DEG_TO_RAD_F); //joint 2
	w_max.push_back(41.2f*DEG_TO_RAD_F); //joint 3
	w_max.push_back(41.2f*DEG_TO_RAD_F); //joint 4
	w_max.push_back(240.0f*DEG_TO_RAD_F); //joint 5
	w_max.push_back(360.0f*DEG_TO_RAD_F); //joint 6
	w_max.push_back(600.0f*DEG_TO_RAD_F); //joint 7
	w_max.push_back(140.0f*DEG_TO_RAD_F); //joint 8
	w_max.push_back(140.0f*DEG_TO_RAD_F); //joint 9
	w_max.push_back(140.0f*DEG_TO_RAD_F); //joint 10

	// upperlimb positions offsets 
	upperlimb_offset.push_back(0.0f*DEG_TO_RAD_F); //joint 0
	upperlimb_offset.push_back(90.0f*DEG_TO_RAD_F); //joint 1
	upperlimb_offset.push_back(0.0f*DEG_TO_RAD_F); //joint 2
	upperlimb_offset.push_back(90.0f*DEG_TO_RAD_F); //joint 3
	upperlimb_offset.push_back(0.0f*DEG_TO_RAD_F); //joint 4
	upperlimb_offset.push_back(0.0f*DEG_TO_RAD_F); //joint 5
	upperlimb_offset.push_back(0.0f*DEG_TO_RAD_F); //joint 6
	upperlimb_offset.push_back(0.0f*DEG_TO_RAD_F); //joint 7
	upperlimb_offset.push_back(0.0f*DEG_TO_RAD_F); //joint 8
	upperlimb_offset.push_back(0.0f*DEG_TO_RAD_F); //joint 9
	upperlimb_offset.push_back(0.0f*DEG_TO_RAD_F); //joint 10

	// objects
	std::vector<float> obj_size(3,0.0);
	obj_size.at(0) = 90.0; // X [mm]
	obj_size.at(1) = 90.0; // Y [mm]
	obj_size.at(2) = 340.0; // Z [mm]
	this->green_column = objPtr(new Object(OBJECT_COLUMN_1,"Green Column",obj_size));
	this->red_column = objPtr(new Object(OBJECT_COLUMN_2,"Red Column",obj_size));
	this->magenta_column = objPtr(new Object(OBJECT_COLUMN_3,"Magenta Column",obj_size));
	this->blue_column = objPtr(new Object(OBJECT_COLUMN_4,"Blue Column",obj_size));

	// logging
	init_logging();

}

CYarpCommInterface::~CYarpCommInterface()
{

}

void CYarpCommInterface::init_logging()
{
	boost::log::add_common_attributes();

	boost::log::add_file_log
	(
		boost::log::keywords::file_name = "aros_interface_%N.log", // file name pattern
		boost::log::keywords::rotation_size = 10 * 1024 * 1024, // rotation file every 10 MB ... 
		boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0,0,0), // ... or at midnight
		boost::log::keywords::format = "[%TimeStamp%]: %Message%", // log record format
		boost::log::keywords::target = "Boost_logs"
	);

	boost::log::core::get()->set_filter
	(
		boost::log::trivial::severity >= boost::log::trivial::info
	);

}

bool CYarpCommInterface::start_joint_states_recever(std::string sender_name)
{
	if(!joint_states_opened)
		joint_states_opened = joint_states_in_port.open(yarp_name_joints_state_receiver);

	if(joint_states_opened)
	{
		stop_receiver = false;
		yarp_name_sender = sender_name;
		joint_states_connected = yarp::os::Network::connect(yarp_name_sender,yarp_name_joints_state_receiver);
		if(joint_states_connected){
			joint_states_receiver_thd = boost::thread(boost::bind(&CYarpCommInterface::joint_states_receiver_job, this));
		}else{ return false;}
	}else{return false;}

	return true;
}

void CYarpCommInterface::stop_joint_states_receiver()
{
	stop_receiver = true;
}

void CYarpCommInterface::joint_states_receiver_job()
{
	Joint_States jstate;
	while(joint_states_connected)
	{
		if(!stop_receiver){
			jstate = *(joint_states_in_port.read());	
			boost::unique_lock<boost::mutex> lck(joint_states_queue_mtx);
			joint_states_queue.push(jstate);
			joint_states_queue_cv.notify_all();
		}else{
			joint_states_connected = !yarp::os::Network::disconnect(yarp_name_sender,yarp_name_joints_state_receiver);		
			boost::unique_lock<boost::mutex> lck(joint_states_queue_mtx);
			while(!joint_states_queue.empty()){joint_states_queue.pop();}
			joint_states_queue_cv.notify_all();
		}
	}
}

bool CYarpCommInterface::getJointState(Joint_States& jstate)
{
	
	if(joint_states_connected)
	{
		boost::unique_lock<boost::mutex> lck(joint_states_queue_mtx);
		//joint_states_queue_cv.wait(lck, [&]() {return !joint_states_queue.empty(); });
		if(!joint_states_queue.empty()){
			//dequeue a info
			jstate = std::move(joint_states_queue.front());
			joint_states_queue.pop();
			for(int i=0;i<jstate.position.size();++i)
				jstate.position.at(i) -= upperlimb_offset.at(i);
			return true;
		}else{return false;}
	}else{return false;}
	

}

bool CYarpCommInterface::homeArmJoint(int nj)
{
	CMessage msgOut;
	msgOut.uCommand = AMTEC_HOME_JOINT;
	msgOut.uParam.assign(1,nj);
	return Send(msgOut);
	//if(Send(msgOut)){
		//return true;
	//}else{
		//std::string ErrorMessage = "Error: message not received. Command: "+std::to_wstring(msgOut.uCommand);
		//AfxMessageBox(ErrorMessage.c_str());
		//return false;
	//}
}

bool CYarpCommInterface::softStopRightArm()
{
	CMessage msgOut;
	msgOut.uCommand = AMTEC_SOFT_STOP;
	return Send(msgOut);
}

bool CYarpCommInterface::initRightHand()
{
	CMessage msgOut;
	msgOut.uCommand = BARRETTHAND_INITIALIZE;
	return Send(msgOut);
}

bool CYarpCommInterface::openRightHand()
{
	CMessage msgOut;
	msgOut.uCommand = BARRETTHAND_OPEN;
	return Send(msgOut);
}

bool CYarpCommInterface::moveRightHandFingers(std::vector<float> fingers_pos)
{
	CMessage msgOut;
	if(fingers_pos.size()==4){
		if(abs(fingers_pos.at(1)-fingers_pos.at(2))<0.01 && abs(fingers_pos.at(1)-fingers_pos.at(3))<0.01){		
			msgOut.uCommand = BARRETTHAND_MOVEFINGERS;
			msgOut.fData.push_back(fingers_pos.at(1));
		}else if(abs(fingers_pos.at(1)-fingers_pos.at(2))<0.01){
			msgOut.uCommand = BARRETTHAND_MOVETWOFINGERS;
			msgOut.fData.resize(2);
			msgOut.fData.push_back(fingers_pos.at(1));
			msgOut.fData.push_back(fingers_pos.at(3));
		}else{
			msgOut.uCommand = BARRETTHAND_MOVEALL;
			msgOut.fData.assign(4,0.0);
			msgOut.fData = fingers_pos;
		}
	}else{
		return false;
	}
	return Send(msgOut);
}

bool CYarpCommInterface::openRightHandPark()
{
	std::vector<float> fingers_home(4,0.0f); 
	//fingers_home.at(1) = 45.0f; fingers_home.at(2) = 45.0f; fingers_home.at(3) = 45.0f;
	fingers_home.at(1) = 69.958117f; fingers_home.at(2) = 69.958117f; fingers_home.at(3) = 69.958117f;
	return moveRightHandFingers(fingers_home);
}

bool CYarpCommInterface::openRightHandHome()
{
	std::vector<float> fingers_home(4,0.0f); 
	fingers_home.at(1) = 45.0f; fingers_home.at(2) = 45.0f; fingers_home.at(3) = 45.0f;
	//fingers_home.at(1) = 69.958117f; fingers_home.at(2) = 69.958117f; fingers_home.at(3) = 69.958117f;
	return moveRightHandFingers(fingers_home);
}

bool CYarpCommInterface::openRightHand(float pos)
{
	std::vector<float> fingers_home(4,0.0f); 
	fingers_home.at(1) = pos; fingers_home.at(2) = pos; fingers_home.at(3) = pos;
	//fingers_home.at(1) = 69.958117f; fingers_home.at(2) = 69.958117f; fingers_home.at(3) = 69.958117f;
	return moveRightHandFingers(fingers_home);
}

bool CYarpCommInterface::closeRightHand()
{
	CMessage msgOut;
	msgOut.uCommand = BARRETTHAND_TORQUECLOSE;
	return Send(msgOut);
}

bool CYarpCommInterface::homeRightUpperLimb()
{
	CMessage msgOut;

	msgOut.uCommand = AMTEC_HOME;
	msgOut.uParam.assign(7,0);
	msgOut.uParam[0] = 5;
	msgOut.uParam[1] = 6;
	msgOut.uParam[2] = 4;
	msgOut.uParam[3] = 2;
	msgOut.uParam[4] = 0;
	msgOut.uParam[5] = 1;
	msgOut.uParam[6] = 3;
	bool arm_sent = Send(msgOut);

	bool hand_sent = openRightHand();

	return (arm_sent && hand_sent);

}

bool CYarpCommInterface::parkRightUpperLimb()
{
	std::vector<float> park_posture(11); // rad
	std::vector<float> current_posture; // rad
	float velocity = 15.0f; // percentage of the joint max velocity

	// Arm
	park_posture.at(0) = -137.5f*DEG_TO_RAD_F; // joint 0
	park_posture.at(1) = 12.06f*DEG_TO_RAD_F; // joint 1
	park_posture.at(2) = 106.59f*DEG_TO_RAD_F; // joint 2
	park_posture.at(3) = -5.42f*DEG_TO_RAD_F; // joint 3
	park_posture.at(4) = -43.28f*DEG_TO_RAD_F; // joint 4
	park_posture.at(5) = -64.0f*DEG_TO_RAD_F; // joint 5
	park_posture.at(6) = 47.97f*DEG_TO_RAD_F; // joint 6
	//Hand
	park_posture.at(7) = 0.0f*DEG_TO_RAD_F; // joint 8

	//park_posture.at(8) = 58.0f*DEG_TO_RAD_F; // joint 9
	//park_posture.at(9) = 58.0f*DEG_TO_RAD_F; // joint 10
	//park_posture.at(10) = 58.0f*DEG_TO_RAD_F; // joint 11
	park_posture.at(8) = 69.958117f*DEG_TO_RAD_F; // joint 9
	park_posture.at(9) = 69.958117f*DEG_TO_RAD_F; // joint 10
	park_posture.at(10) = 69.958117f*DEG_TO_RAD_F; // joint 11

	

	try{
		// get the current posture
		if(!getCurrentRightPositions(current_posture))
			return false;

		// get the reference velocity of the joints
		std::vector<float> w_ref(w_max.begin(),w_max.end());
		for (int i=0;i<w_ref.size();++i)
			w_ref.at(i) = w_ref.at(i)*velocity*0.01f;

		// compute the trajectory 
		std::vector<std::vector<float>> trajectory;
		if(!directTrajectory(current_posture,park_posture,40,w_ref,trajectory))
			return false;

		std::vector<float> Steps; bool b_arm = true; bool b_hand = true;
		Steps.resize(trajectory.size()*trajectory.at(0).size());
		std::vector<float>::iterator it = Steps.begin();
		for(int k=0;k<trajectory.size();k++)
		{
			std::vector<float> &waypt = trajectory.at(k);
			it = std::copy(waypt.begin(),waypt.end(),it);
		}

		// Send the trajectory to the UpperLimb server
		if(!addTrajectory(Steps, trajectory.size(),b_arm,b_hand))
			return false;

		// wait asynchronously for the end of the execution
		if(!sendWaitTrajEndAsync())
			return false;

		stop_mov = false;
		wait_async_thd = boost::thread(boost::bind(&CYarpCommInterface::waitTrajEndAsync, this));

	}catch (std::runtime_error &ex)
	{
		AfxMessageBox(CA2CT(ex.what()),MB_OK);
	}
	return true;
}

void CYarpCommInterface::waitTrajEndAsync()
{
	boost::unique_lock<boost::mutex> lock(async_finished_mtx);
	async_finished_cv.wait(lock,[this](){return async_finished || stop_mov;});

	if(stop_mov){
		sendUpperlimbStop();
	}else{
		if(async_finished && async_str!="")
			throw std::runtime_error(async_str);
	}
}

void CYarpCommInterface::waitVelTrajEndAsync()
{
	boost::unique_lock<boost::mutex> lock(vel_async_finished_mtx);
	vel_async_finished_cv.wait(lock,[this](){return vel_async_finished || stop_mov;});

	if(stop_mov){
		sendUpperlimbStop();
	}else{
		if(vel_async_finished && vel_async_str!="")
			throw std::runtime_error(vel_async_str);
	}
}

void CYarpCommInterface::stopMovement()
{
	if(!stop_mov)
		stop_mov=true;

	CMessage msgOut;
	msgOut.uCommand = AMTEC_STOP_ALL;
	Send(msgOut);
	openRightHand();
}

bool CYarpCommInterface::sendUpperlimbStop()
{
	CMessage msgOut;
	msgOut.uCommand = UpperLimb::STOP_CLEAR_TRAJECTORY;
	return Send(msgOut);
}


bool CYarpCommInterface::sendWaitTrajEndAsync()
{
	async_finished = false;
	CMessage msgOut;
	msgOut.uCommand = UpperLimb::WAIT_TRAJECTORY_END_ASYNC;
	return Send(msgOut);
}

bool CYarpCommInterface::sendWaitVelTrajEndAsync()
{
	vel_async_finished = false;
	CMessage msgOut;
	msgOut.uCommand = UpperLimb::WAIT_VEL_TRAJECTORY_END_ASYNC;
	return Send(msgOut);
}

bool CYarpCommInterface::addTrajectory(std::vector<float> fPos, int nsteps, bool b_arm, bool b_hand)
{
	CMessage msgOut;
	msgOut.uCommand = UpperLimb::ADD_TRAJECTORY;
	std::swap(msgOut.fData,fPos);

	msgOut.uParam.push_back(nsteps);
	msgOut.uParam.push_back(b_arm ? 1 : 0);
	msgOut.uParam.push_back(b_hand ? 1 : 0);

	return Send(msgOut);

}

bool CYarpCommInterface::addVelTrajectory(std::vector<std::vector<float>>& fVel)
{
	CMessage msgOut;
	msgOut.uCommand = UpperLimb::ADD_VEL_TRAJECTORY;	

	std::vector<float> Steps; 
	Steps.resize(fVel.size()*fVel.at(0).size());
	std::vector<float>::iterator it = Steps.begin();
	for(int k=0;k<fVel.size();k++)
	{
		std::vector<float> &waypt = fVel.at(k);
		it = std::copy(waypt.begin(),waypt.end(),it);
	}

	msgOut.uParam.push_back(fVel.size());
	msgOut.uParam.push_back(1);// enable arm
	msgOut.uParam.push_back(1); // enable hand

	std::swap(msgOut.fData,Steps);

	return Send(msgOut);

}

bool CYarpCommInterface::directTrajectory(std::vector<float> init_posture,std::vector<float> goal_posture,
	int steps,std::vector<float> w_max,std::vector<std::vector<float>>& traj)

{
	std::vector<std::vector<float>> trajectory;
	if(!directMovement(init_posture,goal_posture,steps,trajectory))
		return false;

	// compute the time step of the movement
	int n_steps = trajectory.size();
	int n_joints = trajectory.at(0).size();
	std::vector<float> d_o(n_joints);
	std::vector<float> d_o_m(n_joints);
	std::vector<float> t(n_joints);
	std::vector<float> lambda(n_joints,1.0f);

	for(int i=1;i<n_steps;i++)
	{
		for(int k=0;k<trajectory.at(i).size();k++)
		{
			auto dif = std::abs(trajectory.at(i).at(k) - trajectory.at(i-1).at(k));
			d_o.at(k)+=dif;
			if(dif>d_o_m.at(k))
				d_o_m.at(k)=dif;
		}
	}
	// tk total time of each joint
	for(int k=0;k<t.size();k++)
		t.at(k) = n_steps*d_o_m.at(k)/w_max.at(k)+lambda.at(k)*log(1.0f+d_o.at(k));

	// T total time of the movement
	float aux1 = 0.0f; float aux2 = 0.0f;
	for(int k=0;k<trajectory.at(0).size();k++)
	{
		aux1 += lambda.at(k) * d_o.at(k);
		aux2 += lambda.at(k) * d_o.at(k) * t.at(k);
	}
	auto T = aux2 / aux1; auto tstep = T / n_steps;

	for(int h=0;h<trajectory.size();h++)
	{
		std::vector<float> &waypt = trajectory.at(h);
		waypt.push_back(tstep);
	}

	traj = trajectory;
	return true;
}



bool CYarpCommInterface::directMovement(std::vector<float> init_posture,std::vector<float> goal_posture,int steps,
	std::vector<std::vector<float>>& traj)
{
	if(steps<=0)
		return false;

	std::vector<std::vector<float>> path_points(steps+1,init_posture);
	for(int waypoint_idx=0; waypoint_idx<=steps;waypoint_idx++)
	{
		std::vector<float> &waypoint = path_points.at(waypoint_idx);
		float t = static_cast<float>(waypoint_idx) / static_cast<float>(steps);

		for (int joint_idx = 0; joint_idx < waypoint.size(); joint_idx++)
			waypoint.at(joint_idx) = init_posture.at(joint_idx) + (init_posture.at(joint_idx) - goal_posture.at(joint_idx))*(15.0f*pow(t,4)-6.0f*pow(t,5)-10.0f*pow(t,3));
	}

	traj = path_points;
	return true;
}

bool CYarpCommInterface::getCurrentRightPositions(std::vector<float>& curr_pos)
{
	CMessage msgOut;
	msgOut.uCommand = UpperLimb::GET_POSITIONS;
	if(Send(msgOut)){
		curr_pos = current_right_posture;
		return true;
	}else{return false;}

}

bool CYarpCommInterface::getCurrentRightVelocities(std::vector<float>& curr_vel)
{
	CMessage msgOut;
	msgOut.uCommand = UpperLimb::GET_VELOCITIES;
	if(Send(msgOut)){
		curr_vel = current_right_velocities;
		return true;
	}else{return false;}
}


void CYarpCommInterface::trajectoryFinishedAsync(CMessage &msgIn)
{
		std::string error = ""; 
		if(msgIn.uParam.at(0)==3)
		{
			b_error=true;
			error = "Error in Execution of the Command:	TRAJECTORY_ASYNC_FINISHED: "+msgIn.strText;
		}else if(msgIn.uParam.at(0)==2)
		{
			b_error = true;
			error ="Timeout in Execution of the Command: TRAJECTORY_ASYNC_FINISHED: "+msgIn.strText;
		}

		boost::unique_lock<boost::mutex> async_lock(async_finished_mtx);
		async_finished = true;
		async_str = error;
		async_finished_cv.notify_all();
}

void CYarpCommInterface::velTrajectoryFinishedAsync(CMessage &msgIn)
{
		std::string error = ""; 
		if(msgIn.uParam.at(0)==3)
		{
			b_error=true;
			error = "Error in Execution of the Command:	VEL_TRAJECTORY_ASYNC_FINISHED: "+msgIn.strText;
		}else if(msgIn.uParam.at(0)==2)
		{
			b_error = true;
			error ="Timeout in Execution of the Command: VEL_TRAJECTORY_ASYNC_FINISHED: "+msgIn.strText;
		}

		boost::unique_lock<boost::mutex> async_lock(vel_async_finished_mtx);
		vel_async_finished = true;
		vel_async_str = error;
		vel_async_finished_cv.notify_all();
}

void CYarpCommInterface::waitAsync()
{
	stop_mov = false;
	wait_async_thd = boost::thread(boost::bind(&CYarpCommInterface::waitTrajEndAsync, this));
	while(!async_finished)
	{
		sig_joints_update();
		sig_vision_update();
	}
}

void CYarpCommInterface::waitVelAsync()
{
	stop_mov = false;
	vel_wait_async_thd = boost::thread(boost::bind(&CYarpCommInterface::waitVelTrajEndAsync, this));
	while(!vel_async_finished)
	{
		sig_joints_update();
		sig_vision_update();
	}
}

bool CYarpCommInterface::setRightVelocities(std::vector<float> fVel)
{
	CMessage msgOut;
	msgOut.uCommand = UpperLimb::SET_VELOCITIES;
	std::swap(msgOut.fData,fVel);
	return Send(msgOut);
}


bool CYarpCommInterface::addVelTrajectoryExec(std::vector<std::vector<float>>& j_vel_traj)
{
	bool success = true; 
	std::vector<std::vector<float>> j_acc_mean_traj;
	
	for(int i=0;i<j_vel_traj.size()-1;++i)
	{
		//BOOST_LOG_SEV(lg,boost::log::trivial::info) << "trajectory = " << i;
		//TRACE("trajectory = %d\n",i);

		std::vector<float> j_vel_a = j_vel_traj.at(i); 
		boost::chrono::duration<float, boost::ratio<1>> timestep(j_vel_a.at(11)); 
		std::vector<float> j_vel_b = j_vel_traj.at(i+1);

		std::vector<float> j_acc_mean;
		for(int ii=0;ii<j_vel_a.size();++ii)
			j_acc_mean.push_back((j_vel_b.at(ii)-j_vel_a.at(ii))/timestep.count());

		j_acc_mean_traj.push_back(j_acc_mean);

	}

	for(int i=0;i<j_vel_traj.size()-1;++i)
	{
			boost::chrono::duration<float, boost::ratio<1>> ta,tb,tx;
			std::vector<float> j_vel_a = j_vel_traj.at(i); 
			boost::chrono::duration<float, boost::ratio<1>> timestep(j_vel_a.at(11)); 

			std::vector<float> j_acc_mean = j_acc_mean_traj.at(i);
			std::vector<float> j_vel_x(j_acc_mean.size());

			boost::chrono::system_clock::time_point t_start_interval = boost::chrono::system_clock::now(); 
			ta = boost::chrono::system_clock::now() - t_start_interval;
			tx = ta; 
			tb = ta + timestep;

			while((tx.count() <= tb.count()))
			{
	
				//BOOST_LOG_SEV(lg,boost::log::trivial::info) << "while loop "<< cnt;
				//TRACE("while loop = %d\n",cnt);
				//BOOST_LOG_SEV(lg,boost::log::trivial::info) << "ta [sec] = "<< ta.count();
				//BOOST_LOG_SEV(lg,boost::log::trivial::info) << "tb [sec] = "<< tb.count();
				//BOOST_LOG_SEV(lg,boost::log::trivial::info) << "time step [sec]= "<< timestep.count();
				//BOOST_LOG_SEV(lg,boost::log::trivial::info) << "tx [sec] = "<< tx.count();

				// set the interpolated velocities
				for(int ii=0;ii<j_acc_mean.size();++ii)
				{
					j_vel_x.at(ii) = (j_vel_a.at(ii)+(j_acc_mean.at(ii)*tx.count()));
					
				}

				//BOOST_LOG_SEV(lg,boost::log::trivial::info) << "j_vel_x = "<< j_vel_x.at(0)
					//<< " " << j_vel_x.at(1)
					//<< " " << j_vel_x.at(2)
					//<< " " << j_vel_x.at(3)
					//<< " " << j_vel_x.at(4)
					//<< " " << j_vel_x.at(5)
					//<< " " << j_vel_x.at(6)
					//<< " " << j_vel_x.at(7)
					//<< " " << j_vel_x.at(8)
					//<< " " << j_vel_x.at(9)
					//<< " " << j_vel_x.at(10)
					//;

				success = setRightVelocities(j_vel_x);
				tx = boost::chrono::system_clock::now() - t_start_interval;				
			}// while loop interval
	} // for loop trajectory
	

	// stop the motion
	std::vector<float>j_vel_0(11,0.0);	success = setRightVelocities(j_vel_0);
	//softStopRightArm();
	//BOOST_LOG_SEV(lg,boost::log::trivial::info) << "trajectory executed";
	//TRACE("trajectory executed\n");

	return success;
}

void CYarpCommInterface::interpolate(std::vector<float> j_a,std::vector<float> j_b,float m,std::vector<float>& j_x)
{
	j_x.clear();
	if(!j_a.empty() && !j_b.empty())
	{
		for(int i=0;i<j_a.size()-1;++i)
		{
			j_x.push_back(j_a.at(i)+(j_b.at(i)-j_a.at(i))*m);
		}
	}
}

float CYarpCommInterface::l2_norm(std::vector<float>& v)
{
	float accum = 0;
	for(int i=0;i<v.size();++i)
	{
		accum += v.at(i) * v.at(i);
	}

	return sqrt(accum);
}

bool CYarpCommInterface::inRange(float a,float b, float x)
{
	return ((x-a)*(x-b)<=0);
}

bool CYarpCommInterface::lookAtPosition(float dX, float dY, float dZ)
{
	CMessage msgOut;
	msgOut.uCommand = VISION_BOT_Command::VISION_BOT_AROS_PAN_TILT_INTERFACE;
	msgOut.uParam.assign(1,PANTILT_Command::PANTILT_AROS_LOOK_AT_POSITION);
	msgOut.fData.push_back(dX);
	msgOut.fData.push_back(dY);
	msgOut.fData.push_back(dZ);
	return Send(msgOut);			
}

bool CYarpCommInterface::getVisionInfo()
{
	//GREEN COLUMN
	bool gc_pos = getObjectPos(OBJECT_COLUMN_1); 
	bool gc_or = getObjectOr(OBJECT_COLUMN_1); 
	//RED COLUMN
	bool rc_pos = getObjectPos(OBJECT_COLUMN_2);
	bool rc_or = getObjectOr(OBJECT_COLUMN_2); 
	//MAGENTA COLUMN
	bool mc_pos = getObjectPos(OBJECT_COLUMN_3);
	bool mc_or = getObjectOr(OBJECT_COLUMN_3); 
	//BLUE COLUMN
	bool bc_pos = getObjectPos(OBJECT_COLUMN_4);
	bool bc_or = getObjectOr(OBJECT_COLUMN_4); 

	return gc_pos && gc_or 
		&& rc_pos && rc_or
		&& mc_pos && mc_or
		&& bc_pos && bc_or;
}


bool CYarpCommInterface::getObjectPos(int type)
{
	//this->object_type = type;
	CMessage msgOut;
	msgOut.uCommand = VISION_BOT_Command::VISION_BOT_GET_POSITION_OBJECT_TYPE;
	msgOut.uParam.resize(1);
	msgOut.fData.resize(0);
	msgOut.uParam[0]=type;
	return Send(msgOut);
}

bool CYarpCommInterface::getObjectOr(int type)
{
	//this->object_type = type;
	CMessage msgOut;
	msgOut.uCommand = VISION_BOT_Command::VISION_BOT_GET_ORIENTATION_MATRIX_OBJECT_TYPE;
	msgOut.uParam.resize(1);
	msgOut.fData.resize(0);
	msgOut.uParam[0]=type;
	return Send(msgOut);
}

objPtr CYarpCommInterface::getGreenColumn()
{
	return this->green_column;
}

objPtr CYarpCommInterface::getRedColumn()
{
	return this->red_column;
}

objPtr CYarpCommInterface::getMagentaColumn()
{
	return this->magenta_column;
}

objPtr CYarpCommInterface::getBlueColumn()
{
	return this->blue_column;
}

void CYarpCommInterface::Process( CMessage &msgIn, CMessage &msgOut, void *private_data)
{
	b_error = false;
	std::vector<float> obj_pos(3,0.0);
	//std::vector<float> obj_or(3,0.0);
	Matrix3f Rot_mat;

	switch(msgIn.uCommand)
	{
	case UpperLimb::GET_POSITIONS + ACK:
		current_right_posture = msgIn.fData;
		break;
	case UpperLimb::GET_VELOCITIES + ACK:
		current_right_velocities = msgIn.fData;
		break;
	case UpperLimb::TRAJECTORY_ASYNC_FINISHED:		
		trajectoryFinishedAsync(msgIn);
		break;
	case UpperLimb::VEL_TRAJECTORY_ASYNC_FINISHED:		
		velTrajectoryFinishedAsync(msgIn);
		break;
	case VISION_BOT_Command::VISION_BOT_GET_POSITION_OBJECT_TYPE+ACK:
		if( msgIn.fData.size() < 1 ){break;}
		this->object_type=msgIn.fData[0]; 
		obj_pos.at(0)=msgIn.fData[1];
		obj_pos.at(1)=msgIn.fData[2];
		obj_pos.at(2)=msgIn.fData[3];  
		switch(this->object_type)
		{
			case OBJECT_COLUMN_1: // green column
				this->green_column->setPos(obj_pos);
				break;
			case OBJECT_COLUMN_2: // red column
				this->red_column->setPos(obj_pos);
				break;
			case OBJECT_COLUMN_3: // magenta column
				this->magenta_column->setPos(obj_pos);
				break;
			case OBJECT_COLUMN_4: // blue column
				this->blue_column->setPos(obj_pos);
				break;
		}
		break;
	case VISION_BOT_Command::VISION_BOT_GET_ORIENTATION_MATRIX_OBJECT_TYPE+ACK:
		if( msgIn.fData.size() < 1 ){break;}
		this->object_type=msgIn.fData[0]; 
		Rot_mat(0,0)=msgIn.fData[1]; Rot_mat(0,1)=msgIn.fData[4]; Rot_mat(0,2)=msgIn.fData[7];
		Rot_mat(1,0)=msgIn.fData[2]; Rot_mat(1,1)=msgIn.fData[5]; Rot_mat(1,2)=msgIn.fData[8];
		Rot_mat(2,0)=msgIn.fData[3]; Rot_mat(2,1)=msgIn.fData[6]; Rot_mat(2,2)=msgIn.fData[9]; 
		switch(this->object_type)
		{
			case OBJECT_COLUMN_1: // green column
				this->green_column->setOr(Rot_mat);
				break;
			case OBJECT_COLUMN_2: // red column
				this->red_column->setOr(Rot_mat);
				break;
			case OBJECT_COLUMN_3: // magenta column
				this->magenta_column->setOr(Rot_mat);
				break;
			case OBJECT_COLUMN_4: // blue column
				this->blue_column->setOr(Rot_mat);
				break;
		}
		break;
	//default:
		//break;
	}
}
