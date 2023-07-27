#include "Supersampling.hpp"

#include <vector>

using namespace std;
using namespace glm;

#include "../Globals.hpp"
#include "../RayTracer.hpp"

Supersampling::Supersampling(RayTracer *rt) : AntiAliaser{rt} {}

vec3 Supersampling::get(int x, int y, float x1, float x2, float y1, float y2) {
  double dx = (x2 - x1) / 4;
  double dy = (y2 - y1) / 4;

  vector<vec3> coords = {
    vec3(x2 + dx, y2 + dy, -1), // 1
    vec3(x2 + 2*dx, y2 + dy, -1), // 2
    vec3(x2 + 3*dx, y2 + dy, -1), // 3
    vec3(x2 + dx, y2 + 2*dy, -1), // 4
    vec3(x2 + 2*dx, y2 + 2*dy, -1), // 5
    vec3(x2 + 3*dx, y2 + 2*dy, -1), // 6
    vec3(x2 + dx, y2 + 3*dy, -1), // 7
    vec3(x2 + 2*dx, y2 + 3*dy, -1), // 8
    vec3(x2 + 3*dx, y2 + 3*dy, -1), // 9
  };

  vec3 col = vec3(0);
  for (vec3 coord : coords) {
    vec3 ray = normalize(vec3(rt->get_cameraToWorld() * vec4(coord, 0)));
    col += rt->recursiveColour(x, y, rt->get_eye(), ray, 0);
  }
  return col / float(coords.size());
}
