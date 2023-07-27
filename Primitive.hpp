// Termm--Fall 2022

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

class GeometryNode;

enum class PrimitiveType {
	None,
	Sphere,
	Cube,
  Cylinder,
  Cone,
  NHSphere,
  NHCube,
  Mesh
};

struct POI {
  double t;
  glm::vec3 point;
  glm::vec3 norm;
  GeometryNode *gn;

  uint faceNum;
  glm::vec2 tex_coord;
  glm::vec3 tangent_x;
  glm::vec3 tangent_y;

  std::string name;

  POI(double t, glm::vec3 point, glm::vec3 norm, uint faceNum = -1) : t{t}, point{point}, norm{norm}, faceNum{faceNum}, tex_coord{0, 0}, tangent_x{0}, tangent_y{0} {}
  void transform(glm::mat4 trans) {
    point = glm::vec3(trans * glm::vec4(point, 1));
    norm = normalize(transpose(inverse(glm::mat3(trans))) * norm);

    if (length(tangent_x) != 0) tangent_x = normalize(glm::mat3(trans) * tangent_x);
    if (length(tangent_y) != 0) tangent_y = normalize(glm::mat3(trans) * tangent_y);
  }
};

struct BBox {
  glm::vec3 min, max;

  BBox(glm::vec3 min, glm::vec3 max) : min{min}, max{max} {}
};



class Primitive {
public:
  virtual ~Primitive();

  POI *intersect(glm::vec3 e, glm::vec3 r, glm::mat4 trans);

  virtual BBox bb() = 0;

  PrimitiveType type = PrimitiveType::None;

protected:
  POI *intersectConvexPolygon(glm::vec3 e, glm::vec3 r, std::vector<glm::vec3> pts);
  POI *intersectSphere(glm::vec3 e, glm::vec3 r, double radius);
  POI *intersectBox(glm::vec3 e, glm::vec3 r, double radius);

private:
  virtual POI *intersectPrimitive(glm::vec3 e, glm::vec3 r) = 0;
  virtual void setTexCoords(POI *poi) {};
  virtual void setTangentPlane(POI *poi) {};
};



class Sphere : public Primitive {
public:
  Sphere() { type = PrimitiveType::Sphere; }
  virtual ~Sphere();

  inline BBox bb() { return BBox(glm::vec3(-1), glm::vec3(1)); };

private:
  POI *intersectPrimitive(glm::vec3 e, glm::vec3 r);
  void setTexCoords(POI *poi);
  void setTangentPlane(POI *poi);
};

class Cube : public Primitive {
public:
  Cube() { type = PrimitiveType::Cube; }
  virtual ~Cube();

  inline BBox bb() { return BBox(glm::vec3(0), glm::vec3(1)); };

private:
  POI *intersectPrimitive(glm::vec3 e, glm::vec3 r);
  void setTexCoords(POI *poi);
  void setTangentPlane(POI *poi);
};

class Cylinder : public Primitive {
public:
  Cylinder() { type = PrimitiveType::Cylinder; }
  virtual ~Cylinder();

  inline BBox bb() { return BBox(glm::vec3(-1), glm::vec3(1)); };

private:
  POI *intersectPrimitive(glm::vec3 e, glm::vec3 r);
  void setTexCoords(POI *poi);
  void setTangentPlane(POI *poi);
};

class Cone : public Primitive {
public:
  Cone() { type = PrimitiveType::Cone; }
  virtual ~Cone();

  inline BBox bb() { return BBox(glm::vec3(0), glm::vec3(1)); };

private:
  POI *intersectPrimitive(glm::vec3 e, glm::vec3 r);
  void setTexCoords(POI *poi);
  void setTangentPlane(POI *poi);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
    type = PrimitiveType::NHSphere;
  }
  virtual ~NonhierSphere();

  inline BBox bb() { return BBox(glm::vec3(-m_radius) + m_pos, glm::vec3(m_radius) + m_pos); };

private:
  glm::vec3 m_pos;
  double m_radius;

  POI *intersectPrimitive(glm::vec3 e, glm::vec3 r);
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
    type = PrimitiveType::NHCube;
  }
  virtual ~NonhierBox();

  inline BBox bb() { return BBox(glm::vec3(0) + m_pos, glm::vec3(m_size) + m_pos); };

private:
  glm::vec3 m_pos;
  double m_size;

  POI *intersectPrimitive(glm::vec3 e, glm::vec3 r);
};
