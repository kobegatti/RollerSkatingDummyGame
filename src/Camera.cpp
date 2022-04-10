#include "Camera.h"

Camera::Camera()
{
	eye = glm::vec3(0, 0, 0);
	lookAt = glm::vec3(0, 0, 0);
	up = glm::vec3(0, 1, 0);
}

Camera::Camera(glm::vec3 eye_in, glm::vec3 lookAt_in, glm::vec3 up_in)
{
	eye = eye_in;
	lookAt = lookAt_in;
	up = up_in;
}

Camera::~Camera()
{
}

glm::vec3 Camera::getEye()
{
	return eye;
}

void Camera::setEye(glm::vec3 eye_updated)
{
	eye = eye_updated;
}

void Camera::incrementEye(glm::vec3 add_on)
{
	eye += add_on;
}

glm::vec3 Camera::getLookAt()
{
	return lookAt;
}

void Camera::setLookAt(float x_pos, float y_pos, float z_pos)
{
	lookAt.x = x_pos;
	lookAt.y = y_pos;
	lookAt.z = z_pos;	
}

void Camera::setLookAtWithVec(glm::vec3 updated_lookAt) {
	lookAt = updated_lookAt;
}

void Camera::incrementLookAt(glm::vec3 add_on)
{
	lookAt += add_on;
}

glm::vec3 Camera::getUp()
{
	return up;
}

void Camera::setUp(glm::vec3 up_updated)
{
	up = up_updated;
}

void Camera::rotLeft(float new_x, float new_z, float angle, float turn_rate) {
	eye.x += cos(angle) * turn_rate;
	eye.z -= sin(angle) * turn_rate;
	lookAt.x += cos(angle) * turn_rate;
	lookAt.z -= sin(angle) * turn_rate;
}

void Camera::rotRight(float new_x, float new_z, float angle, float turn_rate) {

}