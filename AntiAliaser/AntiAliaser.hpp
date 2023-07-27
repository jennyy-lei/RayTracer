#pragma once

#include <glm/glm.hpp>

#include "../Primitive.hpp"

enum AAType {
  NONE = 0,
  SUPERSAMPLE,
  ADAPTIVE
};

class RayTracer;

class AntiAliaser {
public:
  AntiAliaser();
  AntiAliaser(RayTracer *rt);
  virtual glm::vec3 get(int x, int y, float x1, float x2, float y1, float y2);

  static AntiAliaser *build(RayTracer *rt, AAType type);

protected:
  RayTracer *rt;
  AAType type;
};
