#pragma once

#include <map>
#include <utility>

#include <glm/glm.hpp>

#include "AntiAliaser.hpp"

#include "../RayTracer.hpp"

class AdaptiveAA : public AntiAliaser {
public:
  AdaptiveAA(RayTracer *rt);
  
  glm::vec3 get(int x, int y, float x1, float x2, float y1, float y2);

private:
  glm::vec3 getColour(float x, float y);

  std::map<std::pair<float, float>, glm::vec3> col_map;
};
