#pragma once

#include <list>
#include <vector>
#include <random>
#include <string>
#include <utility>

#include <glm/glm.hpp>

#include "Light/Light.hpp"
#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "kdtree_adaptor.hpp"

class CausticMap {
  struct Ray {
    glm::vec3 o, d;

    Ray(glm::vec3 o, glm::vec3 d) : o{o}, d{d} {}
  };

  SceneNode *m_root;
  std::list<Light *> m_lights;
  std::list<std::pair<SceneNode *, glm::mat4>> m_objs;

  // https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
  inline double random() { return dis(gen); }
  std::random_device rd;  // Will be used to obtain a seed for the random number engine
  std::mt19937 gen; // Standard mersenne_twister_engine seeded with rd()
  std::uniform_real_distribution<> dis;

  void tracePhoton(glm::vec3 power, Ray ray, Light *light, uint depth, bool store);
  std::list<Photon> m_photons;

  std::vector<KNNResult> NN(glm::vec3 point);
  KDTreeAdaptor *m_tree;
  std::vector<Photon> m_photons_vector;

public:
  CausticMap(std::list<Light *> lights, SceneNode *root);
  ~CausticMap();

  inline std::list<Photon> photons() { return m_photons; }

  void generate();
  glm::vec3 estimateIrradiance(glm::vec3 pt, uint id);
};