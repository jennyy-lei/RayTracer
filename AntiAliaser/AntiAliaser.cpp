#include "AntiAliaser.hpp"

#include "Supersampling.hpp"
#include "AdaptiveAA.hpp"
#include "../RayTracer.hpp"

using namespace glm;

AntiAliaser::AntiAliaser() : rt{nullptr} {}
AntiAliaser::AntiAliaser(RayTracer *rt) : rt{rt} {}

vec3 AntiAliaser::get(int x, int y, float x1, float x2, float y1, float y2) {
  float dx = x2 - x1;
  float dy = y2 - y1;

  vec3 coord = vec3(x2 + 2*dx, y2 + 2*dy, -1); // 5
  vec3 ray = normalize(vec3(rt->get_cameraToWorld() * vec4(coord, 0)));
  return rt->recursiveColour(x, y, rt->get_eye(), ray, 0);
}

AntiAliaser *AntiAliaser::build(RayTracer *rt, AAType type) {
  switch (type) {
    case NONE: return new AntiAliaser(rt);
    case SUPERSAMPLE: return new Supersampling(rt);
    case ADAPTIVE: return new AdaptiveAA(rt);
  };
  return nullptr;
}
