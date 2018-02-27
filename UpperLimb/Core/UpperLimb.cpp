//------------------------------------------------------------------------------
//#include "stdafx.h"


//------------------------------------------------------------------------------
// Standard Template Library
#include <algorithm>
#include <vector>
#include <string>
#include <exception>
#include <fstream>
#include <iostream>
#include <conio.h>

//------------------------------------------------------------------------------
//Boost
#include <boost\program_options.hpp>
#include <boost\shared_ptr.hpp>
#include <boost\make_shared.hpp>
#include <boost\bind.hpp>
#include <boost\thread.hpp>
#include <boost\thread\future.hpp>

#include "../amtec_lwa_7dof/AmtecLWA7dof.h"
#include "../barrett_hand_826X/barrett_hand_826X.h"
#include "../Amtec_PStepServer/include/AmtecPStepServer.h"
#include "UpperLimbReqProcessor.h"



#if defined WIN32
#if !defined _WINDOWS_
#include <Windows.h>
#endif
#else
#include <ace/OS_NS_signal.h>
#endif

//------------------------------------------------------------------------------

const std::string default_device_name = "/aros/upperlimb";
const std::string default_config_file = "upperlimb.ini";

const bool default_enable_arm = true;
const bool default_enable_hand = true;

//------------------------------------------------------------------------------
bool end_execution = false;
boost::mutex wait_end_mtx;
boost::condition_variable wait_end_cv;

// Handler to catch INT and TERM signal
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	static int ct = 0;
	ct++;
	if (ct > 2)
	{
		std::cout << "Aborting..." << std::endl;
		//ACE_OS::exit(1);
	}
	std::cout << std::endl << "[try " << ct << " of 2] Trying to shut down" << std::endl;

	{
		boost::unique_lock<boost::mutex> lock(wait_end_mtx);
		end_execution = true;
	}
	wait_end_cv.notify_all();

	return 1;
}

void ArmRestPosition(boost::shared_ptr<AmtecLWA7dof> arm)
{
	//---------------------------------------------------------------------
	// Perform safety check. If an error is present abort maintenance.

	if (!arm->isArmStatusOK())
	{
		std::cerr << std::endl << std::endl <<
			"***** One of the arm modules has an error! *****" <<
			std::endl << std::endl;

		throw std::exception(arm->Get_Error_Str().data(), 9);
	}

	//---------------------------------------------------------------------
	// Move arm to rest position
	if(!arm->positionMoveRamp(1, 90.0f*3.14f / 180.0f))
		throw std::exception(arm->Get_Error_Str().data(), 10);
	if(!arm->positionMoveRamp(3, 90.0f*3.14f / 180.0f))
		throw std::exception(arm->Get_Error_Str().data(), 10);
	if(!arm->positionMoveRamp(0, -90.0f*3.14f / 180.0f))
		throw std::exception(arm->Get_Error_Str().data(), 10);
	if(!arm->positionMoveRamp(1, 0.0f))
		throw std::exception(arm->Get_Error_Str().data(), 10);
}

void ArmGoHome(boost::shared_ptr<AmtecLWA7dof> arm)
{
	//---------------------------------------------------------------------
	// Perform safety check. If an error is present abort maintenance.

	if (!arm->isArmStatusOK())
	{
		std::cerr << std::endl << std::endl <<
			"***** One of the arm modules has an error! *****" <<
			std::endl << std::endl;

		throw std::exception(arm->Get_Error_Str().data(), 8);
	}

	//---------------------------------------------------------------------
	// Move arm from rest position to home position safely

	if(!arm->HomeJoint(1))
		throw std::exception(arm->Get_Error_Str().data(), 10);

	if(!arm->positionMoveRamp(1, 90.0f*3.14f / 180.0f))
		throw std::exception(arm->Get_Error_Str().data(), 10);

	if(!arm->HomeJoint(0))
		throw std::exception(arm->Get_Error_Str().data(), 10);

	if(!arm->HomeAll())
		throw std::exception(arm->Get_Error_Str().data(), 10);

	std::vector< float > vPos;
	vPos.assign(8, 0.0f);
	//vPos[3] = 90.0f*DEG_TO_RAD_F;
	vPos.back() = 10000.0f; // Time to complete movement in ms
	if(!arm->positionMove(vPos.data(), static_cast<unsigned short>(vPos.back()), true)) // Put the arm straight
		throw std::exception(arm->Get_Error_Str().data(), 10); 
}

void Print_Error_Options(void)
{
	std::cout << std::endl << std::endl <<
		"--------------------------------------------------------------------------------" << std::endl <<
		"Specific Arm recovery menu:" << std::endl <<
		"joint status x:\tGet joint 'x' state. 'x' can be the joint number or 'all';" << std::endl <<
		"joint free x:\tRelease joint 'x' break. 'x' can be the joint number or 'all';" << std::endl <<
		"joint reset x:\tReset joint 'x'. 'x' can be the joint number or 'all';" << std::endl <<
		"disable sync motion:\tDisables arm sync motion flag;" << std::endl <<
		"enable sync motion:\tEnables arm sync motion flag;" << std::endl <<
		"reset executor:\tReset error state from executor and continue process trajectories;" << std::endl <<
		"help:\tDisplay this help;" << std::endl <<
		"return:\tGo back to main menu" << std::endl <<
		"exit:\tExit." << std::endl <<
		"--------------------------------------------------------------------------------" << std::endl << std::endl;
}


int ErrorRecovery(boost::shared_ptr<AmtecLWA7dof> arm, UpperLimbReqProcessor &upperlimbproc)
{
	bool finished = false;

	boost::function<std::pair<int,int>(unsigned int, std::string &)> parse_joints_nr = [arm](unsigned int off, std::string &cmd) 
		-> std::pair<int, int>
	{
		std::pair<int, int> range (-1, -1);
		if (cmd.length() > off)
		{
			int nJoints = arm->getNumberOfJoints();

			//get the 'x'
			std::string joint = cmd.substr(off);
			if (joint == "all")
			{
				range.first = 0;
				range.second = nJoints - 1;
			}
			else
			{
				int joint_nr = boost::lexical_cast<int>(joint);
				range.first = joint_nr;
				range.second = joint_nr;
			}
		}
		else
			throw std::string("Missing joint number: Please enter a number or <all>");

		return range;
	};

	do 
	{
		try
		{
			//flush input 
			//std::cin.ignore(std::numeric_limits<std::streamsize>::max(), EOF);
			std::string cmd = "";
			//get command
			std::cout << std::endl << ">> " << std::flush;
			std::getline(std::cin, cmd);

			if (std::cin.fail())
			{
				std::cout << "failed to read from console..." << std::endl;
				finished = true;
				continue;
			}

			//parse: joint status x
			if (cmd.compare(0, 13, "joint status ") == 0)
			{
				std::pair<int, int> range = parse_joints_nr(13, cmd);

				for (int i = range.first; i <= range.second; i++)
				{
					std::cout << std::endl << "Joint " << i << ": ";
					if (!arm->Test_Module(i + 1))
						std::cout << "ERR!: " << arm->Get_Error_Str() << std::endl << std::endl;
					else
						std::cout << "OK." << std::endl;
				}
			}
			else if (cmd.compare(0, 11, "joint free ") == 0)
			{
				std::pair<int, int> range = parse_joints_nr(11, cmd);

				for (int i = range.first; i <= range.second; i++)
				{
					std::cout << std::endl << "Joint " << i << ": ";
					if (!arm->freeJoint(i))
						std::cout << "ERR!: " << arm->Get_Error_Str() << std::endl << std::endl;
					else
						std::cout << "OK." << std::endl;
				}
			}
			else if (cmd.compare(0, 12, "joint reset ") == 0)
			{
				std::pair<int, int> range = parse_joints_nr(12, cmd);

				for (int i = range.first; i <= range.second; i++)
				{
					std::cout << std::endl << "Joint " << i << ": ";
					if (!arm->Reset(i))
						std::cout << "ERR!: " << arm->Get_Error_Str() << std::endl << std::endl;
					else
						std::cout << "OK." << std::endl;
				}
			}
			else if (cmd == "disable sync motion")
			{
				//not yet implemented
				throw std::string("The command is not yet implemented.");
			}
			else if (cmd == "enable sync motion")
			{
				//not yet implemented
				throw std::string("The command is not yet implemented.");
			}
			else if (cmd == "reset executor")
				upperlimbproc.ResetExecutor();
			else if (cmd == "help")
				Print_Error_Options();
			else if (cmd == "return")
				finished = true;
			else if (cmd == "exit")
			{
				finished = true;
				end_execution = true;
			}
			else
				throw std::string("Could not parse the specified command!");
		}
		catch (boost::bad_lexical_cast &ex)
		{
			std::cout << "The specified joint number could not be parsed!: " << ex.what() << std::endl;
		}
		catch (std::string &ex)
		{
			std::cout << ex << std::endl;
		}

	} while (!finished);
	return 0;
}

void Print_Options(void)
{
	std::cout << std::endl << std::endl <<
		"--------------------------------------------------------------------------------" << std::endl <<
		"Arm options:" << std::endl <<
		"F1:\tDisplay this help;" << std::endl <<
		"H:\tGo home from rest position;" << std::endl <<
		"R:\tGo to rest position from home;" << std::endl <<
		"S:\tArm status;" << std::endl <<
		"Hand options:" << std::endl <<
		"I:\tInitialize Hand;" << std::endl <<
		"ESC:\tExit." << std::endl <<
		"--------------------------------------------------------------------------------" << std::endl << std::endl;
}

int WaitUserInput(boost::shared_ptr<AmtecLWA7dof> arm, boost::shared_ptr<BarrettHand826X> hand)
{
	int nJoints;
	
	char key;

	// Test if there was a key pressed.
	if (!_kbhit()) 
		return 0;

	// Get the pressed key.
	key = _getch();
		
	//----------------------------------------------------------------------
	// Handle key presses
	try
	{
		switch (key)
		{
		case 'H':
		case 'h':
			if (arm)
			{
				std::cout << "Take arm to home position from rest... ";
				std::cout.flush();
				ArmGoHome(arm);
				std::cout << "Done." << std::endl;
			}
			else
				std::cout << "Arm is disabled." << std::endl;
			break;

		case 'R':
		case 'r':
			if (arm)
			{
				std::cout << "Take arm to rest position from home... ";
				std::cout.flush();
				ArmRestPosition(arm);
				std::cout << "Done." << std::endl;
			}
			else
				std::cout << "Arm is disabled." << std::endl;
			break;

		case 'S':
		case 's':
			if (arm)
			{
				std::cout << "Arm status:" << std::endl;
				nJoints = arm->getNumberOfJoints();
				for (int iJoint = 0; iJoint < nJoints; iJoint++)
				{
					std::cout << "Joint " << iJoint << ": ";
					if (!arm->Test_Module(iJoint + 1))
						std::cout << "ERR!: " << arm->Get_Error_Str() << std::endl << std::endl;
					else
						std::cout << "OK." << std::endl;
				}
			}
			else
				std::cout << "Arm is disabled." << std::endl;
			break;

		case 'I':
		case 'i':
			if (hand)
			{
				std::cout << "Initializing hand... ";
				std::cout.flush();
				if (hand->Initialize())
					std::cout << "OK." << std::endl;
				else
					std::cout << "Failed!" << std::endl;
			}
			else
				std::cout << "Hand is disabled." << std::endl;
			break;

		case 59: // F1
			Print_Options();
			break;

		case 27: //ESC
			return 1;
		break;
		default:
			break;
		}
	}
	catch (std::exception const&ex)
	{
		std::cout << ex.what() << std::endl;
		Print_Options();
	}

	return 0;
}

/*
//------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
	//Install Handlers for not abruptly shut down
#ifndef WIN32
	ACE_OS::signal(SIGINT, (ACE_SignalHandler) handler);
	ACE_OS::signal(SIGTERM, (ACE_SignalHandler) handler);
#else
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
#endif
	namespace po = boost::program_options;
	using std::string;

	string config_file;
	string net_name;

	string arm_config_file;
	string hand_config_file;
	bool enable_arm;
	bool enable_hand;
	bool init_hand;
	bool arm_gohome;
	bool arm_restposition;
	string maintenance_file;
	yarp::os::Property prop;

	std::cout << std::endl << std::endl <<
		"------------------------ UpperLimb Wrapper  -----------------------" << std::endl << std::endl;

	try
	{
		//Group of options for command line
		po::options_description desc("Generic Options");
		desc.add_options()
			("help,h", "produce help message")
			("config,c", po::value<string>(&config_file)->default_value(default_config_file), "set configuration file name")
			;

		po::options_description config_options("Configuration");
		config_options.add_options()
			("name", po::value<string>(&net_name)->default_value(default_device_name), "set the name to use on the network")
			;

		po::options_description devices_options("Devices configuration");
		devices_options.add_options()
			("Arm.config", po::value<string>(&arm_config_file), "set arm device configuration file name")
			("Arm.enable", po::value<bool>(&enable_arm)->default_value(default_enable_arm), "set if arm device should be enabled")
			("Hand.config", po::value<string>(&hand_config_file), "set arm device configuration file name")
			("Hand.enable", po::value<bool>(&enable_hand)->default_value(default_enable_hand), "set if hand device should be enabled")
			("Hand.init", po::value<bool>(&init_hand)->default_value(false), "set if hand device should be initialized upon startup")
			;

		po::options_description arm_device_options("Arm options");
		arm_device_options.add_options()
			("Arm.gohome", po::value<bool>(&arm_gohome)->default_value(false), "open arm device and perform a Home movement (from rest position)")
			("Arm.restposition", po::value<bool>(&arm_restposition)->default_value(false), "open arm device and perform a RestPosition movement")
			("Arm.maintenance", po::value<string>(&maintenance_file)->default_value(""), "open arm device and perform maintenance script")
			;

		//Enable command line parser
		po::options_description cmdline_options;
		cmdline_options.add(desc).add(config_options).add(devices_options).add(arm_device_options);

		//Enable config file parser
		po::options_description config_file_options;
		config_file_options.add(config_options).add(devices_options);

		po::options_description visible("Allowed options");
		visible.add(desc).add(config_options).add(devices_options);


		//parse from the command line
		po::variables_map vm;
		po::parsed_options parsed_cmd = po::command_line_parser(argc, argv).
			options(cmdline_options).run();
		store(parsed_cmd, vm);
		notify(vm);

		//ignore the remainig command line options
		//unrecognized_options = po::collect_unrecognized( parsed_cmd.options, po::collect_unrecognized_mode::include_positional );

		if (vm.count("help")) {
			std::cout << visible << std::endl;
			return 0;
		}

		//parse from the config file
		ifstream ifs(config_file);
		if (!ifs)
		{
			std::cout << "can not open config file: '" << config_file << "'" << std::endl;
			return 2;
		}
		else
		{
			po::parsed_options parsed_file = parse_config_file(ifs, config_file_options);
			store(parsed_file, vm);
			notify(vm);
		}
		// Show the result
		std::cout << "Device network name was set to '" << net_name << "'." << std::endl;

		std::cout << "Configuration file was set to '" << config_file << "'." << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << "Could not correctly read options: " << e.what() << std::endl;
		return 2;
	}

	bool success;
	yarp::os::Property arm_prop, hand_prop;

	if (enable_arm)
	{
		std::cout << "Loading arm config file... " << std::flush;
		//check configuration files for arm and hand
		success = arm_prop.fromConfigFile(arm_config_file);
		if (!success)
		{
			std::cout << "Error: Could not read from " << arm_config_file << std::endl;
			return 3;
		}
		std::cout << "OK." << std::endl;
	}
	else
		std::cout << "Arm module is disabled." << std::endl;

	if (enable_hand)
	{
		std::cout << "Loading hand config file... " << std::flush;
		success = hand_prop.fromConfigFile(hand_config_file);
		if (!success)
		{
			std::cout << "Error: Could not read from " << hand_config_file << std::endl;
			return 3;
		}
		std::cout << "OK." << std::endl;
	}
	else
		std::cout << "Hand module is disabled." << std::endl;

	std::cout << "Setting process priority... " << std::flush;
	//Set this process as high priority to have better accuracy in sleep_for
	BOOL setHighprio = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	if (!setHighprio)
	{
		//std::string str = GetLastErrorAsString();
		std::cout<< "Error: Failed to set priority"<<std::endl;
	}
	std::cout << "OK." << std::endl;

	//Create instance of yarp network for handling initialization/final procedures.
	yarp::os::Network yarp_net;
	CAmtecPStepServer PStepServer;

	if (enable_arm)
	{
		//PStepserver instance
		std::cout << "Initializing PStepServer instance... " << std::flush;
		if (!PStepServer.Initialize())
			return 4;
		std::cout << "OK." << std::endl;
	}

	//upper limb devices
	boost::shared_ptr<yarp::dev::AmtecLWA7dof> arm;
	boost::shared_ptr<yarp::dev::BarrettHand826X> hand;
	if (enable_arm)
		arm = boost::make_shared<yarp::dev::AmtecLWA7dof>();
	if (enable_hand)
		hand = boost::make_shared<yarp::dev::BarrettHand826X>();

	//open devices
	if (enable_arm)
	{
		//arm must have sync_motion enabled
		arm_prop.unput("use_sync_motion");
		arm_prop.put("use_sync_motion", yarp::os::Value::makeValue("true"));

		std::cout << "Opening arm device... " << std::flush;
		success = arm->open(arm_prop);
		if (!success)
		{
			std::cout << "Error: Could not open ARM device: " << arm->Get_Error_Str() << std::endl;
			return 5;
		}
		std::cout << "OK." << std::endl;
	}
	if (enable_hand)
	{
		//overwrite config file option of initialization of hand
		hand_prop.unput("initialize");
		hand_prop.put("initialize", yarp::os::Value::makeInt(init_hand? 1 : 0));

		std::cout << "Opening hand device... " << std::flush;
		success = hand->open(hand_prop);
		if (!success)
		{
			std::cout << "Error: Could not open HAND device." << std::endl;
			return 6;
		}
		std::cout << "OK." << std::endl;
	}

	//check whether user requested arm go home or restposition
	if (enable_arm)
	{
		try
		{
			if (arm_gohome)
			{
				std::cout << "Take arm to home position from rest... ";
				std::cout.flush();		
				ArmGoHome(arm);
				std::cout << "Done." << std::endl;
			}
			else if (arm_restposition)
			{
				std::cout << "Take arm to rest position from home... ";
				std::cout.flush();
				ArmRestPosition(arm);
				std::cout << "Done." << std::endl;
				return 0;
			}
		}
		catch (std::exception const&ex)
		{
			std::cout << ex.what() << std::endl;
			return 9;
		}
	}

	UpperLimbReqProcessor upperlimbproc;

	YarpConnector limb_req_channel("Upper Limb requests", net_name, "", CYarpCommBase::Server);
	upperlimbproc.SetDevices(arm, hand);
	limb_req_channel.SetReceiveHandler(boost::bind(&UpperLimbReqProcessor::ProcessReq, &upperlimbproc, _1, _2, boost::ref(limb_req_channel)));

	std::cout << "Opening yarp communication channel... " << std::flush;
	success = limb_req_channel.open();
	if (!success)
	{
		std::cout << "Error: Could not open Communications channel: " << limb_req_channel.get_ErrorString() << std::endl;
		return 7;
	}
	std::cout << "OK." << std::endl;

	//start joints publisher
	const std::string joints_name = "/aros/upperlimb_right/joint_states";
	std::cout << "Opening yarp communication channel for joints states publisher... " << std::flush;
	success = upperlimbproc.start_joints_states_sender(joints_name);
	if (!success)
	{
		std::cout << "Error: Could not open port with name '" << joints_name << "'."<< std::endl;
		return 8;
	}
	std::cout << "OK." << std::endl;

	//boost::unique_future<void> fut = boost::async(boost::launch::async, boost::bind(WaitUserInput, arm, hand));

	//{
	//	boost::unique_lock<boost::mutex> lck(wait_end_mtx);
	//	wait_end_cv.wait(lck, [](){ return end_execution; });
	//}


	Print_Options();

	int ret = 0;
	do
	{
		bool do_recovery = upperlimbproc.GetExecutorState() == UpperLimbReqProcessor::ExecutorState::Error;
		//do_recovery = true;
		//check if module is in error state,
		if (do_recovery)
		{
			Print_Error_Options();

			//in that case, show the error recovery options...
			ErrorRecovery(arm, upperlimbproc);
		}
		else
		{
			ret = WaitUserInput(arm, hand);
		}
		boost::this_thread::sleep_for(boost::chrono::milliseconds(200));
	} while (!end_execution && ret == 0);

	std::cout << "Closing process... " << std::flush;
	upperlimbproc.stop_joints_states_sender();
	limb_req_channel.close();
	PStepServer.Terminate();
	//fut.wait();
	std::cout << "OK." << std::endl;

	return 0;
}

*/