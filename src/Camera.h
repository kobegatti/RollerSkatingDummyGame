#pragma once
#ifndef CAMERA_H_
#define CAMERA_H_

#include "glm/glm.hpp"

class Camera
{
public:
	Camera();
	Camera(glm::vec3 eye_in, glm::vec3 lookAt_in, glm::vec3 up_in);
	~Camera();

	glm::vec3 getEye();
	void setEye(glm::vec3 eye_updated);
	void incrementEye(glm::vec3 add_on);

	glm::vec3 getLookAt();
	void setLookAt(float x_pos, float y_pos, float z_pos);
	void Camera::setLookAtWithVec(glm::vec3 updated_lookAt);
	void incrementLookAt(glm::vec3 add_on);

	glm::vec3 getUp();
	void setUp(glm::vec3 up_updated);

	void rotLeft(float new_x, float new_z, float angle, float turn_rate);
	void rotRight(float new_x, float new_z, float angle, float turn_rate);

private:
	glm::vec3 eye;
	glm::vec3 lookAt;
	glm::vec3 up;
};

#endif //CAMERA_H_
