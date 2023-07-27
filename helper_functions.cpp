#include "helper_functions.hpp"

#include <stack>
#include <iostream>

#include "GeometryNode.hpp"

using namespace std;
using namespace glm;

POI *intersectScene(SceneNode *root, vec3 src, vec3 ray) {
  // cout << "intersect " << src << ray << endl; 
  stack<pair<SceneNode *, mat4>> s;
  s.push({root, mat4(1.0)});

  POI *poi = nullptr;

  while(!s.empty()) {
    pair<SceneNode *, mat4> top = s.top();
    s.pop();

    SceneNode *n = top.first;
    mat4 trans = top.second * n->get_transform();
  
    if (n->m_nodeType == NodeType::GeometryNode) {
      GeometryNode *gn = static_cast<GeometryNode *>(n);
      POI *pt = gn->intersectPrimitive(src, ray, trans);
      if (pt && ((!poi || pt->t < poi->t))) {
        delete poi;
        poi = pt;

        if (poi) poi->name = gn->m_name;
      } else {
        delete pt;
      }
    }

    for (SceneNode *c : n->children) {
      s.push({c, trans});
    }
  }
  // std::cout << "... " << (poi ? poi->t : 0) << std::endl;

  return poi;
}
