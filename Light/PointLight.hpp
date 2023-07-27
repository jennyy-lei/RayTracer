// Termm--Fall 2022

#pragma once

#include <iosfwd>

#include <glm/glm.hpp>

#include "Light.hpp"

// Represents a simple spherical light
struct PointLight : Light {
  PointLight();

  std::list<glm::vec3> sample_points(glm::vec3 line);
};
