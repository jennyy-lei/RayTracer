#pragma once

#include <glm/glm.hpp>
#include <list>
#include <vector>

#include "SceneNode.hpp"
#include "Material.hpp"

#include "Image.hpp"
#include "Light/Light.hpp"
#include "Primitive.hpp"
#include "CausticMap.hpp"
#include "AntiAliaser/AntiAliaser.hpp"

class RayTracer {
public:
  RayTracer(
		// What to render
		SceneNode *root,

		// Image to write to, set to a given width and height
		Image &image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
    const std::list<Light *> & lights
  );
  ~RayTracer();

  void render();
  glm::vec3 recursiveColour(double x, double y, glm::vec3 src, glm::vec3 ray, uint depth);

  inline glm::mat4 get_cameraToWorld() { return cameraToWorld; }
  inline glm::vec3 get_eye() { return eye; }

private:
  SceneNode *root;
  Image &im;

  glm::vec3 eye;
  glm::vec3 view;
  glm::vec3 up;
  double fovy;

  glm::mat4 cameraToWorld;

  // Lighting parameters
  glm::vec3 ambient;
  std::list<Light *> lights;

  size_t h, w;

  CausticMap *causticMap;

  AntiAliaser *AA;

  glm::vec3 pixelToColour(int x, int y);
  void colourRow(int y);
  glm::vec3 colour(double x, double y, glm::vec3 src, glm::vec3 ray, POI *pt, uint depth);
};