#pragma once

#include <string>
#include <lodepng/lodepng.h>
#include <glm/glm.hpp>

typedef unsigned int uint;

class ImageMap {
public:
  ImageMap(std::string name);

protected:
  std::vector<unsigned char> img;
  uint w, h;

  glm::vec3 get_col(float x, float y, uint scale);
};

class BumpMap : ImageMap {
public:
  BumpMap(std::string name) : ImageMap{name} {};
  glm::vec3 get_val(float x, float y, uint scale);
};

class TextureMap : ImageMap {
public:
  TextureMap(std::string name) : ImageMap{name} {};
  glm::vec3 get_val(float x, float y, uint scale);
};