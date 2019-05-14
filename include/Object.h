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

	//setters
	void setPos(std::vector<float>& obj_pos);
	void setSize(std::vector<float>& obj_size);
	void setOr(std::vector<float>& obj_or);
	void setLowPassFilters(float freq, float dt);
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
	Matrix3f RotMat;

	// low pass filter for position and orientation
	boost::shared_ptr<LowPassFilter> lpf_obj_pos_x;
	boost::shared_ptr<LowPassFilter> lpf_obj_pos_y;
	boost::shared_ptr<LowPassFilter> lpf_obj_pos_z;
	boost::shared_ptr<LowPassFilter> lpf_obj_qor_x;
	boost::shared_ptr<LowPassFilter> lpf_obj_qor_y;
	boost::shared_ptr<LowPassFilter> lpf_obj_qor_z;
	boost::shared_ptr<LowPassFilter> lpf_obj_qor_w;

};

#endif // OBJECT_HPP