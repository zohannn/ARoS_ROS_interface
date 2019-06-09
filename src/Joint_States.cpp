#include "../include/Joint_States.h"

Joint_States::Joint_States()
{
	//end_effector_state = EndEffector::Moving;
	/*
	lpf_joints.resize(11);
	for(int i=0;i<this->lpf_joints.size();++i){
		this->lpf_joints.at(i).reset(new LowPassFilter()); 
	}
	*/
}

Joint_States::Joint_States(const Joint_States & other)
{
	position = other.position;
	//position_filtered = other.position_filtered;
	velocity = other.velocity;
	velocity_der = other.velocity_der;
	acceleration_der = other.acceleration_der;
	
	//lpf_joints = other.lpf_joints;
	//end_effector_state = other.end_effector_state;
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

bool Joint_States::write(yarp::os::ConnectionWriter& connection) {
	//write joints' number
	connection.appendInt(static_cast<int>(position.size()));
	//write objects
	for (size_t i = 0; i < position.size(); i++)
		connection.appendDouble(position[i]);

	for (size_t i = 0; i < velocity.size(); i++)
		connection.appendDouble(velocity[i]);

	//connection.appendInt(static_cast<int>(end_effector_state));

	return true;
}

bool Joint_States::read(yarp::os::ConnectionReader& connection) {
	//read number of incoming joints
	size_t joints_nr = static_cast<size_t>(connection.expectInt());
	position.resize(joints_nr);
	//position_filtered.resize(joints_nr);
	velocity.resize(joints_nr);
	velocity_der.resize(joints_nr);
	acceleration_der.resize(joints_nr);

	for (size_t i = 0; i < position.size(); i++){
		position[i] = static_cast<float>(connection.expectDouble());
		//position_filtered[i] = this->lpf_joints.at(i)->update(position[i]);
		velocity_der[i] = 0.0;
		acceleration_der[i] = 0.0;
	}

	for (size_t i = 0; i < velocity.size(); i++)
		velocity[i] = static_cast<float>(connection.expectDouble());

	//end_effector_state = static_cast<EndEffector>(connection.expectInt());

	return !connection.isError();
}
/*
void Joint_States::setLowPassFilters(float freq, float dt)
{
	for(int i=0;i<this->lpf_joints.size();++i){
		this->lpf_joints.at(i)->setCutOffFrequency(freq); this->lpf_joints.at(i)->setDeltaTime(dt);
	}
}
*/
/*
void Joint_States::getFilteredPositions(std::vector<float>& ppos)
{
	ppos = this->position_filtered;
}
*/