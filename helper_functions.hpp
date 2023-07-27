#pragma once

#include <glm/glm.hpp>

#include "Primitive.hpp"
#include "SceneNode.hpp"

POI *intersectScene(SceneNode *root, glm::vec3 src, glm::vec3 ray);