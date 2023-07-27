// Termm--Fall 2022

#pragma once

#include <iosfwd>
#include <list>
#include <random>

#include <glm/glm.hpp>

#include "Light.hpp"

// Represents a simple spherical light
struct SphericalLight : Light {
  SphericalLight(double radius = 5);
  
  double radius;

  std::random_device rd;
  std::mt19937 gen;
  std::uniform_real_distribution<> dis;

  std::list<glm::vec3> sample_points(glm::vec3 line);
};
