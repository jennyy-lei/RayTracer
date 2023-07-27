// Termm--Fall 2022

#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include "ImageMap.hpp"

class Material {
public:
  Material(const glm::vec3& kd, const glm::vec3& ks, double shininess, double transmission, double ior);
  virtual ~Material();

  inline glm::vec3 kd() { return m_kd; }
  inline glm::vec3 ks() { return m_ks; }
  inline double shininess() { return m_shininess; }
  inline double reflection() { return m_reflection; }
  inline double transmission() { return m_transmission; }
  inline double ior() { return m_ior; }

  inline glm::vec3 kd(float x, float y, uint scale) { return textureMap ? textureMap->get_val(x, y, scale) : m_kd; }
  inline glm::vec3 bump(float x, float y, uint scale) { return bumpMap ? bumpMap->get_val(x, y, scale) : glm::vec3(0, 0, 1);}
  inline void set_texture(TextureMap *t) { textureMap = t; }
  inline void set_bump(BumpMap *b) { bumpMap = b; }
  inline void set_reflection(double reflection) { m_reflection = reflection; }

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
  double m_reflection;
  double m_transmission;
  double m_ior;

  TextureMap *textureMap;
  BumpMap *bumpMap;
};
