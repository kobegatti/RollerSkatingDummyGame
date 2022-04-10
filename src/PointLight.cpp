#include "PointLight.h"

PointLight::PointLight(glm::vec3 pos, float cons, float lin, float quad) {
	position = pos;
	constant = cons;
	linear = lin;
	quadratic = quad;
}