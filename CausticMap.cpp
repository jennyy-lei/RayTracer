#include "CausticMap.hpp"

#include <glm/gtx/io.hpp>

#include <iostream>
#include <stack>

#include "Globals.hpp"
#include "Mesh.hpp"
#include "GeometryNode.hpp"
#include "Material.hpp"
#include "helper_functions.hpp"
#include "Primitive.hpp"
#include "cs488-framework/MathUtils.hpp"

using namespace std;
using namespace glm;

CausticMap::CausticMap(std::list<Light *> lights, SceneNode *root) : m_lights{lights}, m_root{root}, gen{rd()}, dis{0.0, 1.0}, m_tree{nullptr} {
  stack<pair<SceneNode *, mat4>> s;
  s.push({root, mat4(1.0)});

  while(!s.empty()) {
    pair<SceneNode *, mat4> top = s.top();
    s.pop();

    SceneNode *n = top.first;
    mat4 trans = top.second * n->get_transform();
  
    if (n->m_nodeType == NodeType::GeometryNode) {
      GeometryNode *gn = static_cast<GeometryNode *>(n);
      if (gn->m_caustic) {
        m_objs.push_back(std::pair<SceneNode *, mat4>(n, trans));
      }
    }

    for (SceneNode *c : n->children) {
      s.push({c, trans});
    }
  }
}

CausticMap::~CausticMap() {
  delete m_tree;
}

void CausticMap::generate() {
  cout << "== Generating Caustic Map ===" << endl;
  Mesh mesh = Mesh(CAUSTIC_MAP_OBJFILE);

  uint total = mesh.faces().size() * CAUSTIC_MAP_RAYS_PER_FACE;
  for (Light *l : m_lights) {
    cout << "light" << endl;
    for (Triangle tri : mesh.faces()) {
      vec3 a = mesh.vertex(tri.v1).v,
           b = mesh.vertex(tri.v2).v,
           c = mesh.vertex(tri.v3).v;

      // https://www.cs.princeton.edu/~funk/tog02.pdf
      for (int i = 0; i < CAUSTIC_MAP_RAYS_PER_FACE; ++i) {
        float r1 = sqrt(random()), r2 = random();
        
        vec3 dir = (1 - r1)*a + r1*(1 - r2)*b + r1*r2*c;
        Ray ray = Ray(l->position, normalize(dir));

        // std::cout << (l->colour / float(total)).r << std::endl;
        // tracePhoton(l->colour / float(total), ray, l, 0);
        tracePhoton(vec3(1) / float(CAUSTIC_MAP_RAYS_PER_FACE), ray, l, 0, false);
      }
    }
  }
  // cout << "num rays " << rays.size() << endl;

  if (m_photons.empty()) return;

  m_photons_vector = std::vector<Photon>(m_photons.begin(), m_photons.end());
  m_tree = new KDTreeAdaptor(m_photons_vector);
  cout << "=========== Done ============" << endl;
  return;

  cout << "== Generating Caustic Map ===" << endl;

  std::vector<Ray> rays;
  for (Light *l : m_lights) {
    for (std::pair<SceneNode *, mat4> p : m_objs) {
      cout << p.first->m_name << endl;
      GeometryNode *gn = static_cast<GeometryNode *>(p.first);
      BBox bb = gn->m_primitive->bb();

      vec3 min = vec3(p.second * vec4(bb.min, 1));
      vec3 max = vec3(p.second * vec4(bb.max, 1));

      // https://www.cs.princeton.edu/~funk/tog02.pdf
      double pct = 0;
      for (int i = 0; i < CAUSTIC_MAP_RAYS_PER_OBJECT; ++i) {
        vec3 rand(random(), random(), random());
        rand = rand * (max - min) + min;
        
        vec3 dir = rand - l->position;
        Ray ray = Ray(l->position, normalize(dir));

        if (i * 100.0 / CAUSTIC_MAP_RAYS_PER_OBJECT > pct + PROGRESS_STEP) {
          pct += PROGRESS_STEP;
          cout << pct << "%" << endl;
        }
        tracePhoton(vec3(1) / float(CAUSTIC_MAP_RAYS_PER_OBJECT), ray, l, 0, false);
        // tracePhoton(l->colour, ray, l, 0);
      }
    }
  }
  // cout << "num rays " << rays.size() << endl;

  if (m_photons.empty()) return;

  m_photons_vector = std::vector<Photon>(m_photons.begin(), m_photons.end());
  m_tree = new KDTreeAdaptor(m_photons_vector);
  cout << "=========== Done ============" << endl;
}

vec3 CausticMap::estimateIrradiance(vec3 pt, uint id) {
  if (!m_tree) return vec3(0);
  
  std::vector<KNNResult> res = NN(pt);
  vec3 irradiance = vec3(0);
  float maxDist = 0;
  uint num = 0;
  for (KNNResult n : res) {
    // cout << n.photon.i * n.photon.light->colour << irradiance << endl;
      // cout << n.photon.poi->point << " " << n.photon.poi->name << ", " << n.photon.i << ", dist: " << n.distance << endl;
    if (n.distance < CAUSTIC_MAP_MAX_DIST) {
      vec3 kd = n.photon.poi->gn->m_material->kd(n.photon.poi->tex_coord.x, n.photon.poi->tex_coord.y, n.photon.poi->gn->texturePixelsPerUnit);
      irradiance += n.photon.i * n.photon.light->colour * abs(dot(n.photon.ray_d, n.photon.poi->norm)) * kd * (float)CAUSTIC_MAP_CONSTANT;
      maxDist = glm::max(n.distance, maxDist);
      ++num;
    }
  }
  // cout << maxDist << endl;

  if (num < 5) return vec3(0);

  if (maxDist > 0) irradiance /= M_PI * maxDist * maxDist;
  // std::cout << irradiance << num << std::endl;
  // ifirradiance /= float(num);
  return irradiance;
}


std::vector<KNNResult> CausticMap::NN(vec3 point) {
  const float pt[3] = {point.x, point.y, point.z};

  std::vector<size_t> indices(CAUSTIC_MAP_K);
  std::vector<float> dists(CAUSTIC_MAP_K);
  nanoflann::KNNResultSet<float> resultSet(CAUSTIC_MAP_K);
  resultSet.init(&indices[0], &dists[0]);
  
  m_tree->index->findNeighbors(resultSet, &pt[0]);

  std::vector<KNNResult> res;
  for (size_t i = 0; i < resultSet.size(); i++) {
    res.push_back(KNNResult(m_photons_vector[indices[i]], dists[i]));
  }
  return res;
}

void CausticMap::tracePhoton(vec3 power, Ray ray, Light *light, uint depth, bool store) {
  if (power.r <= 0 || power.g <= 0 || power.b <= 0) return;
  if (depth > CAUSTIC_MAP_DEPTH) return;

  POI *poi = intersectScene(m_root, ray.o, ray.d);
  if (poi) {
    Material *mat = poi->gn->m_material;
    vec3 kd = mat->kd(poi->tex_coord.x, poi->tex_coord.y, poi->gn->texturePixelsPerUnit);
    vec3 bump = mat->bump(poi->tex_coord.x, poi->tex_coord.y, poi->gn->bumpPixelsPerUnit);
    vec3 norm = poi->tangent_x * bump.x + poi->tangent_y * bump.y + poi->norm * bump.z;;

    float ior1 = 1, ior2 = mat->ior();
    float cosi = dot(norm, -ray.d);
    // cout << "kr" << ior1 << "/" << ior2 << "\taoi:" << norm << endl;
    if (cosi < 0) {
      cosi = -cosi;
      norm = -norm;
      std::swap(ior1, ior2);
    }

    float n = ior1 / ior2;
    float sin2t = n*n*(1 - cosi*cosi);
    float kr = 1;
    if (sin2t < 1) {
      float cost = sqrt(1 - sin2t);
      
      float Rr = (ior1*cosi - ior2*cost)/(ior1*cosi + ior2*cost);
      Rr = Rr * Rr;

      float Rt = (ior2*cosi - ior1*cost)/(ior2*cosi + ior1*cost);
      Rt = Rt * Rt;

      kr = (Rr + Rt) / 2;
    }

    float trans = mat->transmission();

    float p_d = (1 - trans) * (kd.r + kd.g + kd.b) / 3;
    float p_r = trans * kr;
    float p_t = trans * (1 - kr);

    // std::cout << vec3(diffProb, transProb, refProb + specProb) << std::endl;
    
    double rand = random();
    vec4 probs = vec4(p_d, p_d + p_r, p_d + p_r + p_t, 1);
    if (rand <= probs[0]) { // diffuse
      // if (store)
        // m_photons.push_back(Photon(poi, power, depth, light, ray.d));

      // std::cout << "diffuse" << std::endl;
      vec3 D = normalize(vec3(random(), random(), random()));
      if (dot(D, norm) < 0) D = -D;
      vec3 pow_d = power / p_d;

      tracePhoton(pow_d, Ray(poi->point, D), light, depth + 1, store);
    } else if (rand <= probs[1]) { // reflect
      // std::cout << "reflect" << std::endl;

      vec3 R = normalize(ray.d + 2*cosi*norm);
      vec3 pow_r = power / p_r;

      tracePhoton(pow_r, Ray(poi->point, R), light, depth + 1, true);
    } else if (rand <= probs[2]) { // transmiss
      // std::cout << "transmiss" << std::endl;

      vec3 T = normalize(n*ray.d + (n*cosi - sqrt(1 - sin2t))*norm);
      vec3 pow_t = power / p_t;
      
      tracePhoton(pow_t, Ray(poi->point, T), light, depth + 1, true);
    } else { // absorb
      // std::cout << "absorb" << std::endl;

      if (store) {
        float p = 1 - (p_d + p_r + p_t);
        m_photons.push_back(Photon(poi, power / p, depth, light, ray.d));
      }
    }
  }
}
