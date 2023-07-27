// Termm--Fall 2022
#include "SphericalLight.hpp"

#include <iostream>

#include <glm/ext.hpp>

#include "../Globals.hpp"

SphericalLight::SphericalLight(double radius) : radius{radius}, gen{rd()}, dis{0, 1} {}

std::list<glm::vec3> SphericalLight::sample_points(glm::vec3 line) {
  glm::vec3 dx = glm::normalize(glm::vec3(line.x, 0, line.x * line.x / -line.z));
  glm::vec3 dy = glm::normalize(glm::vec3(0, line.y, line.y * line.y / -line.z));

  std::list<glm::vec3> pts;
  for (uint i = 0; i < SOFT_SHADOW_RAY_NUM; ++i) {
    // glm::vec3 perturb = glm::vec3(dis(gen), dis(gen), dis(gen));
    // pts.push_back(perturb + position);
    
    float theta = dis(gen) * 2 * M_PI;
    float r = radius * sqrt(dis(gen));

    glm::vec3 perturb = r * (dx * cos(theta) + dy * sin(theta));
    pts.push_back(perturb + position);
  }
  return pts;
}
