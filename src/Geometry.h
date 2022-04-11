#pragma once
#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <vector>
#include "../src/Shape.h"
#include "../src/Program.h"
#include <memory>
#include <glm/fwd.hpp>
#include <glm/ext/vector_float3.hpp>

class Geometry
{
public:
   Geometry();
   virtual ~Geometry() {}
   void drawShapes(const std::shared_ptr<Program> prog);
   void drawHeadAndBody(const std::shared_ptr<Program> prog);
   void drawUpperRightLeg(const std::shared_ptr<Program> prog);
   void drawLowerRightLeg(const std::shared_ptr<Program> prog);
   void drawInstanced(const std::shared_ptr<Program> prog);
   void calculateOriginTranslate();

   std::vector<std::shared_ptr<Shape>> shapes;
   glm::vec3 gMin;
   glm::vec3 gMax;
   glm::vec3 translate_to_origin;
   glm::vec3 pos;
   glm::vec3 orientation;
   glm::vec3 velocity;
   float direction_flag;
   float radius;
   //float x_maxExtent;
   //float y_maxExtent;
   //float z_maxExtent;
};

#endif // GEOMETRY_H_
