#include "RayTracer.hpp"

#include <iostream>
#include <stack>
#include <map>
#include <thread>
using namespace std;

#include <glm/gtx/io.hpp>

using namespace glm;

#include "cs488-framework/MathUtils.hpp"

#include "GeometryNode.hpp"
#include "Globals.hpp"
#include "helper_functions.hpp"
#include "AntiAliaser/AdaptiveAA.hpp"
#include "AntiAliaser/Supersampling.hpp"

namespace {
  void dumpSettings() {
    cout << "========== Settings =========" << endl
         << "Transmission          " << TRANS << endl
         << "  Recursion Depth       " << RECURSION_DEPTH << endl
         << "Shadow Rays           " << SHADOW_RAYS << endl
         << "  Soft Shadow Ray Num   " << SOFT_SHADOW_RAY_NUM << endl
         << "Adaptive AA           " << ADAPTIVE_AA << endl
         << "Phong Shading         " << PHONG_SHADING << endl
         << "Caustic Map           " << CAUSTIC_MAP << endl
         << "  Rays Per Face         " << CAUSTIC_MAP_RAYS_PER_FACE << endl
         << "  Constant              " << CAUSTIC_MAP_CONSTANT << endl
         << "  k                     " << CAUSTIC_MAP_K << endl
         << "=============================" << endl;
  }

  void printNode(SceneNode *node, int indent = 0) {
    for (int i = 0; i < indent; ++i) cout << " ";
    cout << *node;

    if (node->m_nodeType == NodeType::GeometryNode) {
      GeometryNode *gn = static_cast<GeometryNode *>(node);
      
      for (int i = 0; i < indent; ++i) cout << " ";
      cout << " | " << *gn;
    }

    cout << endl;
    for (auto c : node->children) {
      printNode(c, indent + 2);
    }
  }

  vec3 background(float pct) {
    pct = glm::clamp(pct, 0.0f, 1.0f);
    // vec3 bg = gradient(vec3(54/255.0, 54/255.0, 89/255.0), vec3(255/255.0, 147/255.0, 130/255.0), float(y) / h);
    vec3 a = vec3(227/255.0, 237/255.0, 247/255.0);
    // vec3 b = vec3(255/255.0, 255/255.0, 255/255.0);
    vec3 c = vec3(244/255.0, 232/255.0, 237/255.0);
    // vec3 b = vec3(148/255.0, 213/255.0, 225/255.0);
    // vec3 a = vec3(0, 0, 0);
    // vec3 b = vec3(0, 0, 1);
    // if (pct < 0.5) {
    //   return a + (b - a) * 2.0f * pct;
    // } else {
    //   return b + (c - b) * 2.0f * (pct - 0.5f);
    // }
    return a + (c-a) * pct;
  }

  vec3 perturbedNorm(POI *poi, vec3 bump) {
    // cout << poi->tangent_x << poi->tangent_y << endl;
    return poi->tangent_x * bump.x + poi->tangent_y * bump.y + poi->norm * bump.z;
  }
}

RayTracer::RayTracer(
  // What to render
  SceneNode *root,

  // Image to write to, set to a given width and height
  Image &image,

  // Viewing parameters
  const vec3 & eye,
  const vec3 & view,
  const vec3 & up,
  double fovy,

  // Lighting parameters
  const vec3 & ambient,
  const list<Light *> & lights
) : root(root),
    im(image),
    eye(eye),
    view(view),
    up(up),
    fovy(fovy),
    ambient(ambient),
    lights(lights),
    causticMap(new CausticMap(lights, root))
{
  dumpSettings();

	h = im.height();
	w = im.width();

  vec3 camDir = normalize(-view);
  vec3 camRight = normalize(cross(up, camDir));
  vec3 camUp = normalize(cross(camDir, camRight));
  cameraToWorld = inverse(
    transpose(mat4(
      vec4(camRight, 0),
      vec4(camUp, 0),
      vec4(camDir, 0),
      vec4(0, 0, 0, 1))) *
    transpose(mat4(
      1, 0, 0, -eye.x,
      0, 1, 0, -eye.y,
      0, 0, 1, -eye.z,
      0, 0, 0, 1)));

  // printNode(root);

#if CAUSTIC_MAP
  causticMap->generate();
#endif

#if SUPERSAMPLING
  AA = AntiAliaser::build(this, AAType::SUPERSAMPLE);
#elif ADAPTIVE_AA
  AA = AntiAliaser::build(this, AAType::ADAPTIVE);
#else
  AA = AntiAliaser::build(this, AAType::NONE);
#endif
}

RayTracer::~RayTracer() {
  // delete causticMap;
  // delete AA;
}

void RayTracer::render() {
  double pct = 0;
  cout << "========= Rendering =========" << endl;
  cout << pct << "%" << endl;
#if defined(RENDER_X) && defined(RENDER_Y)
  for (uint y = RENDER_Y - RANGE; y < RENDER_Y + RANGE + 1; ++y) {
		for (uint x = RENDER_X - RANGE; x < RENDER_X + RANGE + 1; ++x) {
      vec3 col = pixelToColour(x, y);

      im(x, y, 0) = col.r;
      im(x, y, 1) = col.g;
      im(x, y, 2) = col.b;
    }

    if ((y + 1) * 100.0 / h >= pct + PROGRESS_STEP) {
      pct += PROGRESS_STEP;
      cout << pct << "%" << endl;
    }
	}
#else
  list<thread> threads;

	for (uint y = 0; y < h; ++y) {
#if NUM_THREADS > 1
    threads.emplace_back(&RayTracer::colourRow, this, y);
    // cout << "Thread: " << threads.size() << endl;
    
    if (threads.size() == NUM_THREADS) {
      cout << "join: " << threads.size() << endl;
      for (thread &th : threads) {
        th.join();
      }
      threads.clear();
      cout << "cleared: " << endl;
    }
#else
    colourRow(y);
#endif

    if ((y + 1) * 100.0 / h >= pct + PROGRESS_STEP) {
      pct += PROGRESS_STEP;
      cout << pct << "%" << endl;
    }
	}
  for (thread &th : threads) {
    th.join();
  }
#endif
  cout << "=========== Done ============" << endl;

#if CAUSTIC_MAP && CAUSTIC_MAP_PHOTONS
  cout << "====== Drawing Photons ======" << endl;
  double aspect = w / h;
  double theta = fovy / 2;

  double fovh = tan(degreesToRadians(theta));
  double fovw = aspect*fovh;

  double stepx = 2 * fovw / w;
  double stepy = 2 * fovh / h;

  cout << "Num photons: " << causticMap->photons().size() << endl;
  for (Photon p : causticMap->photons()) {
    vec3 dir = p.poi->point - eye;
    dir = vec3(inverse(cameraToWorld) * vec4(dir, 0));
    dir /= -dir.z;
    // cout << p.poi->name << ": " << p.poi->point << endl;

    int x = floor((dir.x + fovw)/stepx);
    int y = floor((fovh - dir.y)/stepy);

    // cout << x << " " << y << endl;
    // if (p.depth == 0 && p.poi->name == "wall") {
    if (0 <= x && x < w && 0 <= y && y < h) {
      im(x, y, 0) = 1;
      im(x, y, 1) = 0;
      im(x, y, 2) = 0;
      // cout << p.i << p.poi->name << endl;
    }
    // }
  }
  cout << "=========== Done ============" << endl;
#endif
}

void RayTracer::colourRow(int y) {
  for (uint x = 0; x < w; ++x) {
    vec3 col = pixelToColour(x, y);
    im(x, y, 0) = col.r;
    im(x, y, 1) = col.g;
    im(x, y, 2) = col.b;
  }
}


vec3 RayTracer::pixelToColour(int x, int y) {
  double aspect = w / h;
  double theta = fovy / 2;

  double fovh = tan(degreesToRadians(theta));
  double fovw = aspect*fovh;

  double stepx = 2 * fovw / w;
  double stepy = 2 * fovh / h;

  double x2 = -fovw + stepx*x;
  double y2 = fovh - stepy*y;

  /*
    +---------+
    | 1  2  3 |
    | 4  5  6 |
    | 7  8  9 |
    +---------+
  */
  
  vec3 col = vec3(0);
  col = AA->get(x, y, x2, x2 + stepx, y2, y2 + stepy);
  return col;
}

vec3 RayTracer::recursiveColour(double x, double y, vec3 src, vec3 ray, uint depth = 0) {
  vec3 col = vec3(0);

  POI *poi = intersectScene(root, src, ray);
  if (poi) {
    col = colour(x, y, src, ray, poi, depth);
  } else {
    // cout << "miss?" << endl;
    col = background(float(y)/h);
  }

  // cout << "col " << col << endl;
  return col;
}

vec3 RayTracer::colour(double x, double y, vec3 src, vec3 ray, POI *pt, uint depth) {
  Material *mat = pt->gn->m_material;

	// std::cout << "??" << pt->name << std::endl;
  vec3 kd = mat->kd(pt->tex_coord.x, pt->tex_coord.y, pt->gn->texturePixelsPerUnit);
  vec3 ks = mat->ks();
  vec3 bump = mat->bump(pt->tex_coord.x, pt->tex_coord.y, pt->gn->bumpPixelsPerUnit);

  // cout << "before: " << pt->norm  << " , " << bump << endl;
  // cout << "tangents: " << pt->tangent_x << pt->tangent_y << endl;
  vec3 norm = perturbedNorm(pt, bump);
  // cout << "after: " << norm << endl << endl;

  // -----------
  // | ambient |
  // -----------
  vec3 l_ambient = ambient;
  
  // ----------------------
  // | diffuse & specular |
  // ----------------------
  vec3 l_diffuse = vec3(), l_specular = vec3();
  for (Light *light : lights) {
    vec3 light_col = light->colour;
    
    vec3 v = normalize(eye - pt->point);
    vec3 l = normalize(light->position - pt->point);
    vec3 n = normalize(norm);

    // diffuse
    float ln_dot = dot(l, n);

    // specular
    vec3 r = normalize(-l + 2*(ln_dot)*n);
    float rv_dot = dot(r, v);

    float d = length(light->position - pt->point);
    float attenuation = 1 / (light->falloff[0] + light->falloff[1]*d + light->falloff[2]*d*d);
    // if (d > 25) cout << "lds: " << attenuation<< endl << endl;

    // cout << "shadow" << endl;
    // cout <<  pt->point << normalize(l) << endl;
    float visibility = 1;
    // vec3 refractedLight(0);
#if SHADOW_RAYS
    int visible = 0;
    list<vec3> light_pts = light->sample_points(pt->point - light->position);
    for (vec3 light_pt : light_pts) {
      POI *poi = intersectScene(root, pt->point, normalize(light_pt - pt->point));
      if (poi) {
    //     if (poi->gn->m_material->transmission() != 0) {
    //       refractedLight += (float)poi->gn->m_material->transmission() * causticMap->estimateIrradiance(poi->point, poi->gn->m_nodeId);
    //     }
        
        delete poi;
      } else {
        ++visible;
      }
    }
    visibility = float(visible) / light_pts.size();
#endif
    l_diffuse += fmax(ln_dot, 0.0f) * visibility * light_col * attenuation;
    if (rv_dot > 0)
      l_specular += float(pow((rv_dot), mat->shininess())) * visibility * light_col * attenuation;
  }

  // -----------------------------
  // |           mirror           |
  // -----------------------------
  vec3 mirror_dir = normalize(ray + 2*dot(norm, -ray)*norm);
  vec3 l_mirror = mat->reflection() > 0 ? recursiveColour(x, y, pt->point, mirror_dir, depth + 1) : vec3(0);

  // -----------------------------
  // | reflection & transmission |
  // -----------------------------
  vec3 l_reflection = vec3(), l_transmission = vec3();

#if TRANS
  if (depth < RECURSION_DEPTH && mat->transmission() != 0) {
    vec3 norm = pt->norm;
    float ior1 = 1, ior2 = mat->ior();
    float cosi = dot(norm, ray);

    if (cosi <= 0) {
      cosi = -cosi;
      norm = -norm;
      std::swap(ior1, ior2);
    }
    float n = ior1 / ior2;
    // cout << "n: " << n << endl;
    float sin2t = n*n*(1 - cosi*cosi);

    vec3 R = normalize(ray + 2*cosi*norm);
    vec3 T = vec3(0);

    float kr = 1;
    if (sin2t < 1) {
      float cost = sqrt(1 - sin2t);
      T = normalize(n*ray + (n*cosi - cost)*norm);
      
      float Rr = (ior1*cosi - ior2*cost)/(ior1*cosi + ior2*cost);
      Rr = Rr * Rr;
      float Rt = (ior2*cosi - ior1*cost)/(ior2*cosi + ior1*cost);
      Rt = Rt * Rt;

      kr = glm::min((Rr + Rt) / 2, 1.0f);
    } else {
      // cout << "TIR: " << sin2t <<endl;
    }

    // cout << "src: " << src << endl;
    // cout << "ray: " << ray << endl;
    // cout << "norm: " << norm << endl;
    // cout << "hit: " << pt->point << endl;
    // cout << "t: " << pt->t << endl;
    // cout << "T: " << T << endl;
    // cout << "R: " << R << endl;
    // cout << "kr: " << kr << endl;
    // cout << "cosi: " << cosi << endl;
    // cout << "sin2t: " << sin2t << endl;

    l_reflection = kr * recursiveColour(x, y, pt->point, R, depth + 1);
    l_transmission = (1 - kr) * recursiveColour(x, y, pt->point, T, depth + 1);
  } else if (depth == RECURSION_DEPTH) {
    l_reflection = l_ambient;
    l_transmission = l_ambient;
  }
#endif

  float kt = mat->transmission();
  float km = mat->reflection();

#if CAUSTIC_MAP
  // vec3 l_caustic = causticMap->estimateIrradiance(pt->point, pt->gn->m_nodeId);
  // cout << "preestimate" << endl;
  vec3 l_caustic = causticMap->estimateIrradiance(pt->point, pt->gn->m_nodeId);
  // cout << "postestimate" << endl << endl;
#else
  vec3 l_caustic = vec3(0);
#endif

#if TRANS
  // vec3 lout = (1 - km) * (1 - kt) * kd * l_ambient
  //             + (1 - km) * (1 - kt) * kd * l_diffuse
  //             + kt * l_reflection
  //             + kt * l_transmission
  //             + km * l_mirror
  //             + ks * l_specular
  //             + l_caustic;
  
  vec3 lout = (1 - km) * (1 - kt) * kd * l_ambient
              + (1 - km) * (1 - kt) * kd * l_diffuse
              + kt * l_reflection
              + kt * l_transmission
              + km * l_mirror
              + ks * l_specular
              + l_caustic;  
#else
  vec3 lout = (1 - km) * kd * l_ambient
              + (1 - km) * kd * l_diffuse
              + km * l_mirror
              + ks * l_specular
              + l_caustic;
#endif

  return lout;
}
