#ifndef OBJECT_HPP
#define OBJECT_HPP

#ifndef DEG_TO_RAD_F
#define DEG_TO_RAD_F 0.017453292519943295769236907684886f
#endif

#ifndef RAD_TO_DEG_F
#define RAD_TO_DEG_F 57.295779513082320876798154814105f
#endif

#include <Eigen/Dense>
#include <vector>
#include <boost\shared_ptr.hpp>
#include <LowPassFilter.hpp>

using namespace Eigen;

class Object
{

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	// constructor
	Object();
	Object(int type, std::string name,std::vector<float>& obj_size);
	//copy constructor
	Object(const Object& other);

	// getters
	void getPos(std::vector<float>& obj_ppos);
	void getSize(std::vector<float>& obj_size);
	void getOr(std::vector<float>& obj_oor);
	Quaternionf getQOr();
	Matrix3f getRot();

	void getTarPos(std::vector<float>& tar_ppos);
	void getTarOr(std::vector<float>& tar_oor);
	Quaternionf getTarQOr();
	Matrix3f getTarRot();

	//setters
	void setPos(std::vector<float>& obj_pos);
	void setSize(std::vector<float>& obj_size);
	void setOr(Matrix3f& Rot);
	void setLowPassFilters(float freq_pos, float dt_pos,float freq_or, float dt_or);
	//void setQOr(Quaternionf& q);
	//void setRot(Matrix3f& Rot);


private:
	int type;
	std::string name;
	std::vector<float> obj_pos;
	std::vector<float> obj_size;
	std::vector<float> obj_rpy_or; 
	Quaternionf obj_q_or;
	Matrix3f obj_rot;
	Matrix3f RotMat_w; // rotation matrix for the simulated world frame
	Matrix3f RotMat_obj; // rotation matrix for the simulated object frame
	Matrix3f RotMat_obj_tar; // rotation matrix from the object frame to the target frame. 

	// low pass filter for position and orientation
	boost::shared_ptr<LowPassFilter> lpf_obj_pos_x;
	boost::shared_ptr<LowPassFilter> lpf_obj_pos_y;
	boost::shared_ptr<LowPassFilter> lpf_obj_pos_z;
	boost::shared_ptr<LowPassFilter> lpf_obj_qor_x;
	boost::shared_ptr<LowPassFilter> lpf_obj_qor_y;
	boost::shared_ptr<LowPassFilter> lpf_obj_qor_z;
	boost::shared_ptr<LowPassFilter> lpf_obj_qor_w;

	boost::shared_ptr<LowPassFilter> lpf_tar_pos_x;
	boost::shared_ptr<LowPassFilter> lpf_tar_pos_y;
	boost::shared_ptr<LowPassFilter> lpf_tar_pos_z;
	boost::shared_ptr<LowPassFilter> lpf_tar_qor_x;
	boost::shared_ptr<LowPassFilter> lpf_tar_qor_y;
	boost::shared_ptr<LowPassFilter> lpf_tar_qor_z;
	boost::shared_ptr<LowPassFilter> lpf_tar_qor_w;

	// target info 
	std::vector<float> tar_pos;
	std::vector<float> tar_rpy_or; 
	Quaternionf tar_q_or;
	Matrix3f tar_rot;

};

#endif // OBJECT_HPP