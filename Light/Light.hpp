// Termm--Fall 2022

#pragma once

#include <iosfwd>
#include <list>

#include <glm/glm.hpp>

// Represents a simple point light.
struct Light {
  Light();
  
  glm::vec3 colour;
  glm::vec3 position;
  double falloff[3];

  virtual std::list<glm::vec3> sample_points(glm::vec3 line) = 0;
};

std::ostream& operator<<(std::ostream& out, const Light& l);
