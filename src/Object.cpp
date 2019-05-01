#include "../include/Object.h"

Object::Object()
{
	this->type = 0;
	this->name = "";
	this->obj_pos.resize(3,0.0);
	this->obj_size.resize(3,0.0);
	this->obj_rpy_or.resize(3,0.0);
	this->obj_q_or.x()=0; this->obj_q_or.y()=0; this->obj_q_or.z()=0; this->obj_q_or.w()=1;
	this->obj_rot = this->obj_q_or.toRotationMatrix();
	Vector3f rpy = this->obj_q_or.toRotationMatrix().eulerAngles(0, 1, 2);
	VectorXf::Map(&this->obj_rpy_or[0], rpy.size()) = rpy;
}

Object::Object(int type, std::string name)
{
	this->type = type;
	this->name = name;
	this->obj_pos.resize(3,0.0);
	this->obj_size.resize(3,0.0);
	this->obj_rpy_or.resize(3,0.0);
	this->obj_q_or.x()=0; this->obj_q_or.y()=0; this->obj_q_or.z()=0; this->obj_q_or.w()=1;
	this->obj_rot = this->obj_q_or.toRotationMatrix();
	Vector3f rpy = this->obj_q_or.toRotationMatrix().eulerAngles(0, 1, 2);
	VectorXf::Map(&this->obj_rpy_or[0], rpy.size()) = rpy;
}

Object::Object(const Object& other)
{
	this->type = other.type;
	this->name = other.name;
	this->obj_pos = other.obj_pos;
	this->obj_size = other.obj_size;
	this->obj_rpy_or = other.obj_rpy_or;
	this->obj_q_or = other.obj_q_or;
	this-> obj_rot = other.obj_rot;
}

// getters
void Object::getPos(std::vector<float>& obj_pos)
{
	obj_pos = this->obj_pos;
}

void Object::getSize(std::vector<float>& obj_size)
{
	obj_size = this->obj_size;
}

void Object::getOr(std::vector<float>& obj_or)
{
	obj_or = this->obj_rpy_or;
}

Quaternionf Object::getQOr()
{
	return this->obj_q_or;
}

Matrix3f Object::getRot()
{
	return this->obj_rot;
}

// setters
void Object::setPos(std::vector<float>& obj_pos)
{
	this->obj_pos = obj_pos;
}

void Object::setSize(std::vector<float>& obj_size)
{
	this->obj_size = obj_size;
}

void Object::setOr(std::vector<float>& obj_or)
{
	float roll=obj_or.at(0); float pitch=obj_or.at(1); float yaw=obj_or.at(2);
	this->obj_rpy_or = obj_or;
	this->obj_q_or = AngleAxisf(roll, Vector3f::UnitX())
					* AngleAxisf(pitch, Vector3f::UnitY())
					* AngleAxisf(yaw, Vector3f::UnitZ());
	this->obj_rot = this->obj_q_or.toRotationMatrix();
}

