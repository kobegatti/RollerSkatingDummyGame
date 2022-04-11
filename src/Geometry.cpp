#include "Geometry.h"
#include <iostream>

#define RIGHT_FOOT 0
#define RIGHT_ANKLE 1
#define RIGHT_LOWER_LEG 2
#define RIGHT_KNEE 3
#define RIGHT_UPPER_LEG 4
#define RIGHT_PELVIS 5
#define LEFT_FOOT 6
#define LEFT_ANKLE 7
#define LEFT_LOWER_LEG 8
#define LEFT_KNEE 9
#define LEFT_UPPER_LEG 10
#define LEFT_PELVIS 11
#define HIPS 12
#define BELLY 13
#define TORSO 14
#define RIGHT_SHOULDER 15
#define RIGHT_BICEP 16
#define RIGHT_ELBOW 17
#define RIGHT_FOREARM 18
#define RIGHT_WRIST 19
#define RIGHT_HAND 20
#define LEFT_SHOULDER 21
#define LEFT_BICEP 22
#define LEFT_ELBOW 23
#define LEFT_FOREARM 24
#define LEFT_WRIST 25
#define LEFT_HAND 26
#define NECK 27
#define HEAD 28

using namespace std;

Geometry::Geometry() {
   gMin = glm::vec3(0);
   gMax = glm::vec3(0);
   translate_to_origin = glm::vec3(0);
   pos = glm::vec3(0);
   orientation = glm::vec3(0);
   velocity = glm::vec3(0);
   direction_flag = 1.0f;
   radius = 0;
   //x_maxExtent = 0;
   //y_maxExtent = 0;
   //z_maxExtent = 0;
}

void Geometry::drawShapes(const shared_ptr<Program> prog) {
   for (long unsigned int i = 0; i < shapes.size(); i++) {
	shapes[i]->draw(prog);
   }
}

void Geometry::drawHeadAndBody(const shared_ptr<Program> prog) {
	for (long unsigned int i = 0; i < shapes.size(); i++) {
		if (i == HEAD || i == NECK || i == TORSO || i == HIPS || i == BELLY || i == LEFT_SHOULDER || i == RIGHT_SHOULDER) {
			shapes[i]->draw(prog);
		}
	}
}

void Geometry::drawUpperRightLeg(const shared_ptr<Program> prog) {
	for (long unsigned int i = 0; i < shapes.size(); i++) {
		if (i == RIGHT_PELVIS || i == RIGHT_UPPER_LEG) {
			shapes[i]->draw(prog);
		}
	}
}

void Geometry::drawLowerRightLeg(const shared_ptr<Program> prog) {
	for (long unsigned int i = 0; i < shapes.size(); i++) {
		if (i == RIGHT_KNEE || i == RIGHT_LOWER_LEG) {
			shapes[i]->draw(prog);
		}
	}
}

void Geometry::drawInstanced(const shared_ptr<Program> prog) {
	for (long unsigned int i = 0; i < shapes.size(); i++) {
		shapes[i]->instanceDraw(prog);
	}
}

void Geometry::calculateOriginTranslate() {
   translate_to_origin.x = -((gMin.x + gMax.x) / 2);
   translate_to_origin.y = -((gMin.y + gMax.y) / 2);
   translate_to_origin.z = -((gMin.z + gMax.z) / 2);
}
