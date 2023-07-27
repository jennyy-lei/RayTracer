// Termm--Fall 2022
#include "PointLight.hpp"

#include "../Globals.hpp"

PointLight::PointLight() {}

std::list<glm::vec3> PointLight::sample_points(glm::vec3 line) {
  return { position };
}
