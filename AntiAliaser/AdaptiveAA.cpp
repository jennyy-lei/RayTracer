#include "AdaptiveAA.hpp"

#include <vector>

using namespace std;
using namespace glm;

#include "../Globals.hpp"
#include "../RayTracer.hpp"

AdaptiveAA::AdaptiveAA(RayTracer *rt) : AntiAliaser{rt} {}

vec3 AdaptiveAA::get(int x, int y, float x1, float x2, float y1, float y2) {
  if (x2 - x1 < 0.001 && y2 - y1 < 0.001) {
    float xmid = (x2 + x1)/2.0f, ymid = (y2 + y1)/2.0f;
    return getColour(xmid, ymid);
  } else {
    vector<vec3> cols = {
      vec3(x1, y1, -1),
      vec3(x2, y1, -1),
      vec3(x2, y2, -1),
      vec3(x1, y2, -1)
    };
    vec3 avgCol(0);
    for (size_t i = 0; i < cols.size(); ++i) {
      cols[i] = getColour(cols[i].x, cols[i].y);
      avgCol += cols[i];
    }
    avgCol /= 4.0f;

    double maxColDiff = 0;
    for (int i = 0; i < cols.size(); ++i) {
      maxColDiff = fmax(length(cols[i] - cols[(i + 1) % cols.size()]), maxColDiff);
    }

    // cout << maxColDiff << endl << endl;
    // cout << avgCol << endl << endl;
    if (maxColDiff > ADAPTIVE_AA_THRESHOLD) {
#if ADAPTIVE_AA_DEBUG
      return vec3(1, 0, 0);
#else
      // cout << x1 << " " << x2 << " / " << y1 << " " << y2 << endl;
      // cout << x2 - x1 << " / " << y2 - y1 << endl;
      float xmid = (x2 + x1)/2.0f, ymid = (y2 + y1)/2.0f;
      // cout << xmid << " " << ymid << endl << endl;
      vec3 subdivide = get(x, y, x1, xmid, y1, ymid)
                     + get(x, y, xmid, x2, y1, ymid)
                     + get(x, y, x1, xmid, ymid, y2)
                     + get(x, y, xmid, x2, ymid, y2);
      avgCol = subdivide / 4.0f;
#endif
    }

    // vec3 ray = normalize(vec3(rt->get_cameraToWorld() * vec4(vec3((x1 + x2)/2, (y1 + y2)/2, -1), 0)));
    // cout << avgCol << colour(x, y, rt->get_eye(), ray, 0, RayType::PRIMARY) << endl << endl;
    return avgCol;
}
}

glm::vec3 AdaptiveAA::getColour(float x, float y) {
  if (col_map.count({x, y}) > 0)
    return col_map[{x, y}];

  vec3 ray = normalize(vec3(rt->get_cameraToWorld() * vec4(x, y, -1, 0)));
  vec3 col = rt->recursiveColour(x, y, rt->get_eye(), ray, 0);
  col_map[{x, y}] = col;
  return col;
}