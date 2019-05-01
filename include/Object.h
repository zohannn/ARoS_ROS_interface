#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <Eigen/Dense>
#include <vector>

using namespace Eigen;

class Object
{

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	// constructor
	Object();
	Object(int type, std::string name);
	//copy constructor
	Object(const Object& other);

	// getters
	void getPos(std::vector<float>& obj_pos);
	void getSize(std::vector<float>& obj_size);
	void getOr(std::vector<float>& obj_or);
	Quaternionf getQOr();
	Matrix3f getRot();

	//setters
	void setPos(std::vector<float>& obj_pos);
	void setSize(std::vector<float>& obj_size);
	void setOr(std::vector<float>& obj_or);
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

};

#endif // OBJECT_HPP