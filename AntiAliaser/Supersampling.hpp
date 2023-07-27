#pragma once

#include "AntiAliaser.hpp"

#include "../RayTracer.hpp"

class Supersampling : public AntiAliaser {
public:
  Supersampling(RayTracer *rayTracer);
  
  glm::vec3 get(int x, int y, float x1, float x2, float y1, float y2);
};
