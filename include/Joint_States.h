#ifndef JOINTS_STATES_H
#define JOINTS_STATES_H

#include <vector>

// yarp
#include <yarp\os\Portable.h>
#include <yarp\os\ConnectionWriter.h>
#include <yarp\os\ConnectionReader.h>

// low pass filtering
#include <LowPassFilter.hpp>

// boost
//#include <boost\shared_ptr.hpp>


class Joint_States : public yarp::os::Portable
{

public:
	/*
	enum EndEffector
	{
		Grasping,
		Moving,
		Releasing
	};
	*/

	std::vector<float> position;
	//std::vector<float> position_filtered;
	std::vector<float> velocity;
	std::vector<float> velocity_der; // velocity derived from position
	std::vector<float> acceleration_der; // acceleration derived from velocity_der
	//EndEffector end_effector_state;

	// low pass filter of the joints
	//std::vector<boost::shared_ptr<LowPassFilter>> lpf_joints;
	
	Joint_States();

	//copy constructor
	Joint_States(const Joint_States& other);

	//move semantics
	Joint_States(Joint_States && other);
	
	Joint_States& operator=(Joint_States other);

	//void setLowPassFilters(float freq, float dt);

	//void getFilteredPositions(std::vector<float>& ppos);

	friend void swap(Joint_States& first, Joint_States& second)
	{
		// enable ADL (not necessary in our case, but good practice)
		using std::swap;

		// by swapping the members of two classes,
		// the two classes are effectively swapped
		swap(first.position, second.position);
		//swap(first.position_filtered, second.position_filtered);
		swap(first.velocity, second.velocity);
		swap(first.velocity_der, second.velocity_der);
		swap(first.acceleration_der, second.acceleration_der);
		
		//swap(first.lpf_joints, second.lpf_joints);
		
		//swap(first.end_effector_state, second.end_effector_state);
	}

	virtual bool write(yarp::os::ConnectionWriter& connection);
	virtual bool read(yarp::os::ConnectionReader& connection);


};

#endif //JOINTS_STATES_H