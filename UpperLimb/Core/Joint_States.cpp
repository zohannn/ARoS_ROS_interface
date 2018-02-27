#include "Joint_States.h"

Joint_States::Joint_States()
{
	end_effector_state = EndEffector::Moving;
}

Joint_States::Joint_States(const Joint_States & other)
{
	position = other.position;
	end_effector_state = other.end_effector_state;
}

//move semantics
Joint_States::Joint_States(Joint_States && other)
{
	swap(*this, other);
}
Joint_States& Joint_States::operator = (Joint_States other)
{
	swap(*this, other); // (2)

	return *this;
}

/*

bool Joint_States::write(yarp::os::ConnectionWriter& connection) {
	//write joints' number
	connection.appendInt(static_cast<int>(position.size()));
	//write objects
	for (size_t i = 0; i < position.size(); i++)
		connection.appendDouble(position[i]);

	connection.appendInt(static_cast<int>(end_effector_state));

	return true;
}

*/
/*
bool Joint_States::read(yarp::os::ConnectionReader& connection) {
	//read number of incoming joints
	size_t joints_nr = static_cast<size_t>(connection.expectInt());
	position.resize(joints_nr);
	for (size_t i = 0; i < position.size(); i++)
		position[i] = static_cast<float>(connection.expectDouble());

	end_effector_state = static_cast<EndEffector>(connection.expectInt());

	return !connection.isError();
}
*/