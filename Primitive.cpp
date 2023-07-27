// Termm--Fall 2022

#include "Primitive.hpp"

#include <glm/gtx/io.hpp>

using namespace glm;

#include "polyroots.hpp"
#include "Globals.hpp"
#include "cs488-framework/MathUtils.hpp"


POI *Primitive::intersectConvexPolygon(vec3 e, vec3 r, std::vector<vec3> pts) {
  if (pts.size() <= 2) return nullptr;

  vec3 norm = normalize(cross(pts[2] - pts[1], pts[0] - pts[1]));
  vec4 plane = vec4(norm, -(norm[0]*pts[0].x + norm[1]*pts[0].y + norm[2]*pts[0].z));

  double t = -(plane[0]*e.x + plane[1]*e.y + plane[2]*e.z + plane[3])/(plane[0]*r.x + plane[1]*r.y + plane[2]*r.z);
  
  vec3 q = e + float(t)*r;
  for (int i = 0; i < pts.size(); ++i) {
    vec3 a = pts[i], b = pts[(i+1) % pts.size()];
    vec3 p = (a + b) / 2.0f;

    vec3 n = cross(norm, (b - a));

    double dotprod = dot((q - p), n);

    if (dotprod <= 0) {
      return nullptr;
    }
  }
  return !glm::isinf(t) && t > EPSILON ? new POI(t, q, norm) : nullptr;
}

POI *Primitive::intersectSphere(glm::vec3 e, glm::vec3 r, double radius) {
  // x^2+ y^2+ z^2- r = 0
  // std::cout << "radius: " << radius << std::endl; 
  // std::cout << "eye: " << e << "ray: " << r << std::endl;
  // std::cout << length(e + r * 910.0f) << std::endl;
  // std::cout <<  << std::endl;
  double a = r.x*r.x + r.y*r.y + r.z*r.z;
  double b = 2*(r.x*e.x + r.y*e.y + r.z*e.z);
  double c = e.x*e.x + e.y*e.y + e.z*e.z - radius*radius;

  double roots[2];
  size_t num = quadraticRoots(a, b, c, roots);
  // std::cout << a << ", " << b << ", " << c<< ", "  << "roots " << num << std::endl;

  double root = -1;
  if (num == 1) {
    // std::cout << roots[0] << std::endl;
    if (roots[0] > EPSILON) root = roots[0];
  }
  else if (num == 2) {
    // std::cout <<"roots : " << roots[0] << ", " << roots[1] << std::endl;
    for (size_t i = 0; i < num; ++i) {
      if (roots[i] > EPSILON && (root < EPSILON || roots[i] <= root))
        root = roots[i];
    }
    // std::cout << root << std::endl;
    // std::cout << length(e + r * float(root)) << std::endl;
    
    // double root2 = -1;
    // if (roots[0] > EPSILON) root2 = roots[0];
    // if (roots[1] > EPSILON) root2 = min(root2, roots[1]);
    // std::cout << root << " vs " << root2 << std::endl << std::endl;
  }
  
  if (root == -1) return nullptr;
  else {
    vec3 poi = e + float(root) * r;
    return new POI(root, poi, normalize(poi), 0);
  }
}

POI *Primitive::intersectBox(glm::vec3 e, glm::vec3 r, double size) {
  std::vector<POI *> pts = {
    // front
    intersectConvexPolygon(e, r, {vec3(size, 0, size), vec3(size, size, size), vec3(0, size, size), vec3(0, 0, size)}),
    // right
    intersectConvexPolygon(e, r, {vec3(size, 0, size), vec3(size, 0, 0), vec3(size, size, 0), vec3(size, size, size)}),
    // back
    intersectConvexPolygon(e, r, {vec3(0, 0, 0), vec3(0, size, 0), vec3(size, size, 0), vec3(size, 0, 0)}),
    // left
    intersectConvexPolygon(e, r, {vec3(0, 0, 0), vec3(0, 0, size), vec3(0, size, size), vec3(0, size, 0)}),
    // top
    intersectConvexPolygon(e, r, {vec3(0, size, 0), vec3(0, size, size), vec3(size, size, size), vec3(size, size, 0)}),
    // bottom
    intersectConvexPolygon(e, r, {vec3(0, 0, 0), vec3(size, 0, 0), vec3(size, 0, size), vec3(0, 0, size)}),
  };


  POI *poi = nullptr;
  for (uint i = 0; i < pts.size(); ++i) {
    if (!poi || (pts[i] && pts[i]->t < poi->t && poi->t > EPSILON)) {
      delete poi;
      poi = pts[i];

      if (poi) poi->faceNum = i;
    } else {
      delete pts[i];
    }
  }
  return poi;
}


Primitive::~Primitive() {}
Sphere::~Sphere() {}
Cube::~Cube() {}
Cylinder::~Cylinder() {}
Cone::~Cone() {}
NonhierSphere::~NonhierSphere() {}
NonhierBox::~NonhierBox() {}

// Returns the intersection point in world coordinates
//
// @param e Source of the ray
// @param r Normalized direction of the ray
// @param trans Transformations applied to the primitive in a hierarchical model
//
// @return point of intersection if one exists and t > EPSILON
POI *Primitive::intersect(vec3 e, vec3 r, mat4 trans)
{
  // std::cout << "intersect nhsphere " <<  e << r << std::endl;
  // std::cout << "e " << e << ", untransformed: " << vec3(inverse(trans) * vec4(e, 1)) << ", trans: " << trans << std::endl;
  POI *pt = intersectPrimitive(
    vec3(inverse(trans) * vec4(e, 1)),
    vec3(inverse(trans) * vec4(r, 0))
  );

  if (pt) {
    setTexCoords(pt);
    setTangentPlane(pt);

    // pt->point = vec3(trans * vec4(pt->point, 1));
    // pt->norm = normalize(transpose(inverse(mat3(trans))) * pt->norm);
    pt->transform(trans);
  }
  return pt;
}



POI *Sphere::intersectPrimitive(vec3 e, vec3 r)
{
  return intersectSphere(e, r, 1);
}

POI *Cube::intersectPrimitive(vec3 e, vec3 r)
{
  return intersectBox(e, r, 1);
}

POI *Cylinder::intersectPrimitive(vec3 e, vec3 r)
{ 
  double root = -1;
  uint faceNum = 0;

  // intersect body
  double a = r.x*r.x + r.z*r.z;
  double b = 2*(e.x*r.x + e.z*r.z);
  double c = e.x*e.x + e.z*e.z - 1;

  double roots[2];
  size_t num = quadraticRoots(a, b, c, roots);

  if (num == 1) {
    if (roots[0] > EPSILON) root = roots[0];
  }
  else if (num == 2) {
    for (size_t i = 0; i < num; ++i) {
      if (roots[i] > EPSILON && (root < EPSILON || roots[i] <= root)) {
        float y = (e + float(roots[i]) * r).y;

        if (fabsf(y) <= 1)
          root = roots[i];
      }
    }
  }
  
  // intersect upper circle
  double t = (1 - e.y) / r.y;
  if (t > EPSILON && (root == -1 || t < root)) {
    vec3 pt = e + float(t) * r;
    if (length(pt - vec3(0, 1, 0)) <= 1) {
      faceNum = 1;
      root = t;
    }
  }

  // intersect lower circle
  t = (-1 - e.y) / r.y;
  if (t > EPSILON && (root == -1 || t < root)) {
    vec3 pt = e + float(t) * r;
    if (length(pt - vec3(0, -1, 0)) <= 1) {
      faceNum = 2;
      root = t;
    }
  }

  if (root == -1) return nullptr;
  else {
    vec3 poi = e + float(root) * r;
    vec3 norm = vec3(poi.x, 0, poi.z);
    if (faceNum != 0) {
      norm = vec3(0, poi.y, 0);
      // std::cout << poi << normalize(norm) << std::endl;
    }
    
    return new POI(root, poi, normalize(norm), faceNum);
  }
}

POI *Cone::intersectPrimitive(vec3 e, vec3 r)
{
  double root = -1;
  uint faceNum = 0;

  // intersect body
  e -= vec3(0, 1, 0);
  double a = r.x*r.x - r.y*r.y + r.z*r.z;
  double b = 2*(e.x*r.x - (e.y)*r.y + e.z*r.z);
  double c = e.x*e.x - (e.y)*(e.y) + e.z*e.z;
  
  double roots[2];
  size_t num = quadraticRoots(a, b, c, roots);

  if (num == 1) {
    if (roots[0] > EPSILON) root = roots[0];
  }
  else if (num == 2) {
    for (size_t i = 0; i < num; ++i) {
      if (roots[i] > EPSILON && (root < EPSILON || roots[i] <= root)) {
        float y = (e + float(roots[i]) * r).y;

        if (-1 <= y && y <= 0)
          root = roots[i];
      }
    }
  }
  e += vec3(0, 1, 0);

  // intersect lower circle
  double t = - e.y / r.y;
  if (t > EPSILON && (root == -1 || t < root)) {
    vec3 pt = e + float(t) * r;
    if (length(pt) <= 1) {
      faceNum = 1;
      root = t;
    }
  }
  
  if (root == -1) return nullptr;
  else {
    vec3 poi = e + float(root) * r;
    vec3 norm = vec3(poi.x, length(vec2(poi.x, poi.z)), poi.z);
    if (faceNum != 0) {
      norm = vec3(0, -1, 0);
    }
    
    return new POI(root, poi, normalize(norm), faceNum);
  }
}

POI *NonhierSphere::intersectPrimitive(vec3 e, vec3 r)
{
  e = e - m_pos;
  POI *poi = intersectSphere(e, r, m_radius);
  
  if (poi) poi->point = poi->point + m_pos;
  return poi;
}

POI *NonhierBox::intersectPrimitive(vec3 e, vec3 r)
{
  e = e - m_pos;
  POI *poi = intersectBox(e, r, m_size);
  
  if (poi) poi->point = poi->point + m_pos;
  return poi;
}



void Sphere::setTexCoords(POI *poi) {
  float rad = length(poi->point);
  float circumference = rad * 2 * M_PI;

  float h = length(vec2(poi->point.x, poi->point.z));
  float azimuth = radiansToDegrees(acos(poi->point.z / h));
  if (poi->point.x < 0) {
    azimuth = 360 - azimuth;
  }

  float altitude = radiansToDegrees(acos(poi->point.y / rad));
  if (altitude < 0) altitude = 180 - altitude;
  // cout << rad << endl;
  // cout << azimuth << ", " << altitude << " | " << step << endl;
  poi->tex_coord = vec2(azimuth * circumference / 360.0f, circumference * altitude / 360.0f);
}

void Cube::setTexCoords(POI *poi) {
  float faceSize = 1;

  switch (poi->faceNum) {
  case 0:
    poi->tex_coord = vec2(poi->point.x, faceSize + 1 - poi->point.y);
    break;
  case 1:
    poi->tex_coord = vec2(faceSize + (1 - poi->point.z), faceSize + 1 - poi->point.y);
    break;
  case 2:
    poi->tex_coord = vec2(faceSize*2 + (1 - poi->point.x), faceSize + 1 - poi->point.y);
    break;
  case 3:
    poi->tex_coord = vec2(faceSize*3 + poi->point.z, faceSize + 1 - poi->point.y);
    break;
  case 4:
    poi->tex_coord = vec2(poi->point.x, poi->point.z);
    break;
  case 5:
    poi->tex_coord = vec2(poi->point.x, faceSize*2 + (1 - poi->point.z));
    break;
  }
}

void Cylinder::setTexCoords(POI *poi) {
  float rad = 1;
  float circumference = rad * 2 * M_PI;

  float h = length(vec2(poi->point.x, poi->point.z));
  float azimuth = radiansToDegrees(acos(poi->point.z / h));
  if (poi->point.x < 0) {
    azimuth = 360 - azimuth;
  }

  if (poi->faceNum == 1) {
    poi->tex_coord = vec2(circumference * azimuth / 360.0f, h);
  } else if (poi->faceNum == 0) {
    poi->tex_coord = vec2(circumference * azimuth / 360.0f, 1 + 1 - poi->point.y);
  }  else if (poi->faceNum == 2) {
    poi->tex_coord = vec2(circumference * azimuth / 360.0f, 3 + 1 - h);
  }
  // std::cout << poi->tex_coord << std::endl;
}

void Cone::setTexCoords(POI *poi) {
  float rad = 1;
  float circumference = rad * 2 * M_PI;

  // cout << "cone " << poi->point << endl;
  float h = length(vec2(poi->point.x, poi->point.z));
  float azimuth = radiansToDegrees(acos(poi->point.z / h));
  if (poi->point.x < 0) {
    azimuth = 360 - azimuth;
  }

  if (poi->faceNum == 0) {
    poi->tex_coord = vec2(circumference * azimuth / 360.0f, 1 - poi->point.y);
  } else if (poi->faceNum == 1) {
    poi->tex_coord = vec2(circumference * azimuth / 360.0f, 1 + (1 - h));
  }
}




void Sphere::setTangentPlane(POI *poi) {
  // std::cout << poi->point.x << ", " << poi->point.y << ", " << poi->point.z << std::endl;
  poi->tangent_x = normalize(vec3(poi->point.x, 0, - poi->point.x * poi->point.x / poi->point.z));
  poi->tangent_y = normalize(vec3(0, poi->point.y, - poi->point.y * poi->point.y / poi->point.z));
}

void Cube::setTangentPlane(POI *poi) {
  // std::cout << poi->point.x << ", " << poi->point.y << ", " << poi->point.z << std::endl;
  switch (poi->faceNum) {
    case 0:
      poi->tangent_x = vec3(1, 0, 0);
      poi->tangent_y = vec3(0, 1, 0);
      break;
    case 1:
      poi->tangent_x = vec3(0, 0, -1);
      poi->tangent_y = vec3(0, 1, 0);
      break;
    case 2:
      poi->tangent_x = vec3(-1, 0, 0);
      poi->tangent_y = vec3(0, 1, 0);
      break;
    case 3:
      poi->tangent_x = vec3(0, 0, 1);
      poi->tangent_y = vec3(0, 1, 0);
      break;
    case 4:
      poi->tangent_x = vec3(1, 0, 0);
      poi->tangent_y = vec3(0, 0, -1);
      break;
    case 5:
      poi->tangent_x = vec3(1, 0, 0);
      poi->tangent_y = vec3(0, 0, 1);
      break;
  }
}

void Cylinder::setTangentPlane(POI *poi) {
  // std::cout << poi->point.x << ", " << poi->point.y << ", " << poi->point.z << std::endl;
  if (poi->faceNum == 0) {
    poi->tangent_x = normalize(vec3(poi->point.x, 0, - poi->point.x * poi->point.x / poi->point.z));
    poi->tangent_y = vec3(0, 1, 0);
  } else {
    poi->tangent_x = normalize(vec3(poi->point.x, 0, - poi->point.x * poi->point.x / poi->point.z));
    poi->tangent_y = normalize(vec3(poi->point.x, 0, poi->point.z));
  }
}

void Cone::setTangentPlane(POI *poi) {
  // std::cout << poi->point.x << ", " << poi->point.y << ", " << poi->point.z << std::endl;
  if (poi->faceNum == 0) {
    poi->tangent_x = normalize(vec3(poi->point.x, 0, poi->point.x * poi->point.x / poi->point.z));
    poi->tangent_y = normalize(vec3(poi->point.y, 0, poi->point.y * poi->point.y / poi->point.z));
  } else if (poi->faceNum == 1) {
    poi->tangent_x = normalize(vec3(poi->point.x, 0, - poi->point.x * poi->point.x / poi->point.z));
    poi->tangent_y = normalize(vec3(poi->point.x, 0, poi->point.z));
  }
}
