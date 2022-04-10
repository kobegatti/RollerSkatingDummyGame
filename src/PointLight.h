#pragma once
#ifndef _POINTLIGHT_H_
#define _POINTLIGHT_H_

#include <glm/fwd.hpp>
#include <glm/ext/vector_float3.hpp>

class PointLight {
public:
	PointLight(glm::vec3 pos, float cons, float lin, float quad);
	virtual ~PointLight() {};

	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

};
#endif // _POINTLIGHT_H_