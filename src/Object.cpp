#include "../include/Object.h"
#include <iostream>

Object::Object()
{
	this->type = 0;
	this->name = "";
	this->obj_pos.resize(3,0.0);
	this->obj_size.resize(3,0.0);
	this->obj_rpy_or.resize(3,0.0);
	this->obj_q_or.x()=0; this->obj_q_or.y()=0; this->obj_q_or.z()=0; this->obj_q_or.w()=1;
	this->obj_rot = this->obj_q_or.toRotationMatrix();
	Vector3f obj_rpy = this->obj_q_or.toRotationMatrix().eulerAngles(2, 1, 0);
	VectorXf::Map(&this->obj_rpy_or[0], obj_rpy.size()) = obj_rpy;
	this->RotMat_w << -1,0,0,
						0,-1,0,
						0,0,1;
	this->RotMat_obj << 0,0,1,
						0,1,0,
						-1,0,0;
	this->RotMat_obj_tar << 0,0,-1,
							0,1,0,
							1,0,0;
	this->tar_pos = this->obj_pos;
	this->tar_rot = this->RotMat_obj_tar*this->obj_rot;
	this->tar_q_or = this->tar_rot;
	Vector3f tar_rpy = this->tar_q_or.toRotationMatrix().eulerAngles(2, 1, 0);
	this->tar_rpy_or.resize(3,0.0);
	VectorXf::Map(&this->tar_rpy_or[0], tar_rpy.size()) = tar_rpy;

	this->lpf_obj_pos_x.reset(new LowPassFilter());
	this->lpf_obj_pos_y.reset(new LowPassFilter());
	this->lpf_obj_pos_z.reset(new LowPassFilter());
	this->lpf_obj_qor_x.reset(new LowPassFilter());
	this->lpf_obj_qor_y.reset(new LowPassFilter());
	this->lpf_obj_qor_z.reset(new LowPassFilter());
	this->lpf_obj_qor_w.reset(new LowPassFilter());

	this->lpf_tar_pos_x.reset(new LowPassFilter());
	this->lpf_tar_pos_y.reset(new LowPassFilter());
	this->lpf_tar_pos_z.reset(new LowPassFilter());
	this->lpf_tar_qor_x.reset(new LowPassFilter());
	this->lpf_tar_qor_y.reset(new LowPassFilter());
	this->lpf_tar_qor_z.reset(new LowPassFilter());
	this->lpf_tar_qor_w.reset(new LowPassFilter());


}

Object::Object(int type, std::string name,std::vector<float>& obj_size)
{
	this->type = type;
	this->name = name;
	this->obj_pos.resize(3,0.0);
	this->obj_size = obj_size;
	this->obj_rpy_or.resize(3,0.0);
	this->obj_q_or.x()=0; this->obj_q_or.y()=0; this->obj_q_or.z()=0; this->obj_q_or.w()=1;
	this->obj_rot = this->obj_q_or.toRotationMatrix();
	Vector3f obj_rpy = this->obj_q_or.toRotationMatrix().eulerAngles(2, 1, 0);
	VectorXf::Map(&this->obj_rpy_or[0], obj_rpy.size()) = obj_rpy;
	this->RotMat_w << -1,0,0,
						0,-1,0,
						0,0,1;
	this->RotMat_obj << 0,0,1,
						0,1,0,
						-1,0,0;
	this->RotMat_obj_tar << 0,0,-1,
							0,1,0,
							1,0,0;
	this->tar_pos = this->obj_pos;
	this->tar_rot = this->RotMat_obj_tar*this->obj_rot;
	this->tar_q_or = this->tar_rot;
	Vector3f tar_rpy = this->tar_q_or.toRotationMatrix().eulerAngles(2, 1, 0);
	this->tar_rpy_or.resize(3,0.0);
	VectorXf::Map(&this->tar_rpy_or[0], tar_rpy.size()) = tar_rpy;

	this->lpf_obj_pos_x.reset(new LowPassFilter());
	this->lpf_obj_pos_y.reset(new LowPassFilter());
	this->lpf_obj_pos_z.reset(new LowPassFilter());
	this->lpf_obj_qor_x.reset(new LowPassFilter());
	this->lpf_obj_qor_y.reset(new LowPassFilter());
	this->lpf_obj_qor_z.reset(new LowPassFilter());
	this->lpf_obj_qor_w.reset(new LowPassFilter());

	this->lpf_tar_pos_x.reset(new LowPassFilter());
	this->lpf_tar_pos_y.reset(new LowPassFilter());
	this->lpf_tar_pos_z.reset(new LowPassFilter());
	this->lpf_tar_qor_x.reset(new LowPassFilter());
	this->lpf_tar_qor_y.reset(new LowPassFilter());
	this->lpf_tar_qor_z.reset(new LowPassFilter());
	this->lpf_tar_qor_w.reset(new LowPassFilter());
}

Object::Object(const Object& other)
{
	this->type = other.type;
	this->name = other.name;
	this->obj_pos = other.obj_pos;
	this->obj_size = other.obj_size;
	this->obj_rpy_or = other.obj_rpy_or;
	this->obj_q_or = other.obj_q_or;
	this->obj_rot = other.obj_rot;
	this->RotMat_w = other.RotMat_w;
	this->RotMat_obj = other.RotMat_obj;
	this->RotMat_obj_tar = other.RotMat_obj_tar;
	this->tar_pos = other.tar_pos;
	this->tar_rpy_or = other.tar_rpy_or; 
	this->tar_q_or = other.tar_q_or;
	this->tar_rot = other.tar_rot;

	this->lpf_obj_pos_x = other.lpf_obj_pos_x;
	this->lpf_obj_pos_y = other.lpf_obj_pos_y;
	this->lpf_obj_pos_z = other.lpf_obj_pos_z;
	this->lpf_obj_qor_x = other.lpf_obj_qor_x;
	this->lpf_obj_qor_y = other.lpf_obj_qor_y;
	this->lpf_obj_qor_z = other.lpf_obj_qor_z;
	this->lpf_obj_qor_w = other.lpf_obj_qor_w;

	this->lpf_tar_pos_x = other.lpf_tar_pos_x;
	this->lpf_tar_pos_y = other.lpf_tar_pos_y;
	this->lpf_tar_pos_z = other.lpf_tar_pos_z;
	this->lpf_tar_qor_x = other.lpf_tar_qor_x;
	this->lpf_tar_qor_y = other.lpf_tar_qor_y;
	this->lpf_tar_qor_z = other.lpf_tar_qor_z;
	this->lpf_tar_qor_w = other.lpf_tar_qor_w;

}

// getters
void Object::getPos(std::vector<float>& obj_ppos)
{
	//obj_ppos = this->obj_pos;
	obj_ppos.resize(this->obj_pos.size());
	obj_ppos.at(0) = this->lpf_obj_pos_x->update(this->obj_pos.at(0));
	obj_ppos.at(1) = this->lpf_obj_pos_y->update(this->obj_pos.at(1));
	obj_ppos.at(2) = this->lpf_obj_pos_z->update(this->obj_pos.at(2));
}

void Object::getTarPos(std::vector<float>& tar_ppos)
{
	tar_ppos.resize(this->tar_pos.size());
	tar_ppos.at(0) = this->lpf_tar_pos_x->update(this->tar_pos.at(0));
	tar_ppos.at(1) = this->lpf_tar_pos_y->update(this->tar_pos.at(1));
	tar_ppos.at(2) = this->lpf_tar_pos_z->update(this->tar_pos.at(2));
}


void Object::getSize(std::vector<float>& obj_size)
{
	obj_size = this->obj_size;
}

void Object::getOr(std::vector<float>& obj_oor)
{
	Quaternionf q = this->getQOr();
	Matrix3f mat = q.toRotationMatrix();
	Vector3f rpy = mat.eulerAngles(2, 1, 0);
	obj_oor.resize(rpy.size());
	VectorXf::Map(&obj_oor[0], rpy.size()) = rpy*RAD_TO_DEG_F;
}

void Object::getTarOr(std::vector<float>& tar_oor)
{
	Quaternionf q = this->getTarQOr();
	Matrix3f mat = q.toRotationMatrix();
	Vector3f rpy = mat.eulerAngles(2, 1, 0);
	tar_oor.resize(rpy.size());
	VectorXf::Map(&tar_oor[0], rpy.size()) = rpy*RAD_TO_DEG_F;
}

Quaternionf Object::getQOr()
{
	//return this->obj_q_or;
	Quaternionf q;
	q.x() = this->lpf_obj_qor_x->update(this->obj_q_or.x());
	q.y() = this->lpf_obj_qor_y->update(this->obj_q_or.y());
	q.z() = this->lpf_obj_qor_z->update(this->obj_q_or.z());
	q.w() = this->lpf_obj_qor_w->update(this->obj_q_or.w());
	return q;
}

Quaternionf Object::getTarQOr()
{
	Quaternionf q;
	q.x() = this->lpf_tar_qor_x->update(this->tar_q_or.x());
	q.y() = this->lpf_tar_qor_y->update(this->tar_q_or.y());
	q.z() = this->lpf_tar_qor_z->update(this->tar_q_or.z());
	q.w() = this->lpf_tar_qor_w->update(this->tar_q_or.w());
	return q;
}

Matrix3f Object::getRot()
{
	return this->obj_rot;
	//Quaternionf q = this->getQOr();
	//return q.toRotationMatrix();
}

Matrix3f Object::getTarRot()
{
	return this->tar_rot;
	//Quaternionf q = this->getTarQOr();
	//return q.toRotationMatrix();
}

// setters
void Object::setPos(std::vector<float>& obj_pos)
{
	Vector3f obj_ppos(obj_pos.data());
	Vector3f obj_pppos = this->RotMat_obj*this->RotMat_w*obj_ppos;
	VectorXf::Map(&this->obj_pos[0], obj_pppos.size()) = obj_pppos;
	this->tar_pos = this->obj_pos;
}

void Object::setSize(std::vector<float>& obj_size)
{
	this->obj_size = obj_size;
}

void Object::setOr(std::vector<float>& obj_or)
{
	float roll=obj_or.at(0)*DEG_TO_RAD_F;
	float pitch=obj_or.at(1)*DEG_TO_RAD_F;
	float yaw=obj_or.at(2)*DEG_TO_RAD_F;
	Matrix3f Rot;
	//Rot = AngleAxisf(roll, Vector3f::UnitZ())*AngleAxisf(pitch, Vector3f::UnitY())*AngleAxisf(yaw, Vector3f::UnitX());
	// Rot = Rot_z * Rot_y * Rot_x
    Rot(0,0) = cos(roll)*cos(pitch);  Rot(0,1) = cos(roll)*sin(pitch)*sin(yaw)-sin(roll)*cos(yaw); Rot(0,2) = sin(roll)*sin(yaw)+cos(roll)*sin(pitch)*cos(yaw);
    Rot(1,0) = sin(roll)*cos(pitch);  Rot(1,1) = cos(roll)*cos(yaw)+sin(roll)*sin(pitch)*sin(yaw); Rot(1,2) = sin(roll)*sin(pitch)*cos(yaw)-cos(roll)*sin(yaw);
    Rot(2,0) = -sin(pitch);           Rot(2,1) = cos(pitch)*sin(yaw);                              Rot(2,2) = cos(pitch)*cos(yaw);
	
	this->obj_rot = this->RotMat_obj*this->RotMat_w*Rot;
	Vector3f rpy = this->obj_rot.eulerAngles(2, 1, 0);
	this->obj_rpy_or.resize(rpy.size());
	VectorXf::Map(&this->obj_rpy_or[0], rpy.size()) = rpy*RAD_TO_DEG_F;
	this->obj_q_or = this->obj_rot;

	this->tar_rot = this->RotMat_obj_tar*this->obj_rot;
	this->tar_q_or = this->tar_rot;
	Vector3f tar_rpy = this->tar_q_or.toRotationMatrix().eulerAngles(2, 1, 0);
	this->tar_rpy_or.resize(tar_rpy.size());
	VectorXf::Map(&this->tar_rpy_or[0], tar_rpy.size()) = tar_rpy;
}

void Object::setLowPassFilters(float freq_pos, float dt_pos,float freq_or, float dt_or)
{
	this->lpf_obj_pos_x->setCutOffFrequency(freq_pos); this->lpf_obj_pos_x->setDeltaTime(dt_pos);
	this->lpf_obj_pos_y->setCutOffFrequency(freq_pos); this->lpf_obj_pos_y->setDeltaTime(dt_pos);
	this->lpf_obj_pos_z->setCutOffFrequency(freq_pos); this->lpf_obj_pos_z->setDeltaTime(dt_pos);
	this->lpf_obj_qor_x->setCutOffFrequency(freq_or); this->lpf_obj_qor_x->setDeltaTime(dt_or);
	this->lpf_obj_qor_y->setCutOffFrequency(freq_or); this->lpf_obj_qor_y->setDeltaTime(dt_or);
	this->lpf_obj_qor_z->setCutOffFrequency(freq_or); this->lpf_obj_qor_z->setDeltaTime(dt_or);
	this->lpf_obj_qor_w->setCutOffFrequency(freq_or); this->lpf_obj_qor_w->setDeltaTime(dt_or);

	this->lpf_tar_pos_x->setCutOffFrequency(freq_pos); this->lpf_tar_pos_x->setDeltaTime(dt_pos);
	this->lpf_tar_pos_y->setCutOffFrequency(freq_pos); this->lpf_tar_pos_y->setDeltaTime(dt_pos);
	this->lpf_tar_pos_z->setCutOffFrequency(freq_pos); this->lpf_tar_pos_z->setDeltaTime(dt_pos);
	this->lpf_tar_qor_x->setCutOffFrequency(freq_or); this->lpf_tar_qor_x->setDeltaTime(dt_or);
	this->lpf_tar_qor_y->setCutOffFrequency(freq_or); this->lpf_tar_qor_y->setDeltaTime(dt_or);
	this->lpf_tar_qor_z->setCutOffFrequency(freq_or); this->lpf_tar_qor_z->setDeltaTime(dt_or);
	this->lpf_tar_qor_w->setCutOffFrequency(freq_or); this->lpf_tar_qor_w->setDeltaTime(dt_or);
}

