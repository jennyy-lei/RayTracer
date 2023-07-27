// Termm--Fall 2022

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"


struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{}
};

struct Vertex
{
	glm::vec3 v, n;
	size_t n_faces;

	Vertex( glm::vec3 v ) : v{v}, n{glm::vec3()}, n_faces{0} {} 
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );

	inline std::vector<Vertex> vertices() { return m_vertices; }
	inline Vertex vertex(uint i) { return m_vertices[i]; }
	inline std::vector<Triangle> faces() { return m_faces; }
  
  inline BBox bb() { return BBox(m_bs_o - glm::vec3(m_bs_radius), m_bs_o + glm::vec3(m_bs_radius)); };
	
private:
	std::vector<Vertex> m_vertices;
	std::vector<Triangle> m_faces;

	glm::vec3 m_bs_o;
	double m_bs_radius;

  POI *intersectPrimitive(glm::vec3 e, glm::vec3 r);

	friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
