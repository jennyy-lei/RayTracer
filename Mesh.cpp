// Termm--Fall 2022

#include <iostream>
#include <fstream>
#include <vector>

#include <glm/ext.hpp>
#include <glm/gtx/io.hpp>
#include <iostream>

using namespace glm;
using namespace std;

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#include "Globals.hpp"

namespace {
	float area(vec3 a, vec3 b, vec3 c) {
		return length(cross(b - a, c - a)) / 2;
	}
}

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( ("Assets/Obj/" + fname).c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;

			m_bs_o += glm::vec3(vx, vy, vz);
			m_vertices.push_back( Vertex( glm::vec3( vx, vy, vz ) ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}

	for (Triangle f : m_faces) {
		glm::vec3 v1 = m_vertices[f.v1].v, v2 = m_vertices[f.v2].v, v3 = m_vertices[f.v3].v;
		glm::vec3 n = normalize(cross(v1 - v2, v2 - v3));
		m_vertices[f.v1].n += n;
		++m_vertices[f.v1].n_faces;
		m_vertices[f.v2].n += n;
		++m_vertices[f.v2].n_faces;
		m_vertices[f.v3].n += n;
		++m_vertices[f.v3].n_faces;
	}

	for (Vertex v : m_vertices) {
		// cout << v.n << v.n_faces << endl;
		v.n = normalize(v.n / (float)v.n_faces);
	}

	m_bs_o /= m_vertices.size();
	m_bs_radius = 0;
	for (Vertex v : m_vertices) {
		if (length(v.v - m_bs_o) > m_bs_radius)
			m_bs_radius = length(v.v - m_bs_o);
	}
  
	type = PrimitiveType::Mesh;
}

POI *Mesh::intersectPrimitive(glm::vec3 e, glm::vec3 r) {
	POI *bb = intersectSphere(e - m_bs_o, r, m_bs_radius);
	if (!bb) return nullptr;
	else {
		#if RENDER_BOUNDING_VOLUMES
			return bb;
		#endif
		delete bb;
	}

	POI *poi = nullptr;
	for (Triangle tri : m_faces) {
		std::vector<vec3> v = {m_vertices[tri.v1].v, m_vertices[tri.v2].v, m_vertices[tri.v3].v};
		POI * temp = intersectConvexPolygon(e, r, v);

		if ((!poi && temp) || (temp && temp->t < poi->t)) {
			delete poi;
			poi = temp;

#if PHONG_SHADING
			// std::cout << "before ps "<<poi->norm << std::endl;
			vec3 p = poi->point,
					 a = m_vertices[tri.v1].v,
					 b = m_vertices[tri.v2].v,
					 c = m_vertices[tri.v3].v;
			vec3 uvw = vec3(area(c,a,p) / area(a,b,c), area(a,b,p) / area(a,b,c), area(b,c,p) / area(a,b,c));
			// std::cout << "uvw "<< uvw << std::endl;
			// std::cout << "tri "<< m_vertices[tri.v1].v << m_vertices[tri.v2].v  << m_vertices[tri.v3].v << std::endl;
			// std::cout << "tri "<< m_vertices[tri.v1].n << m_vertices[tri.v2].n  << m_vertices[tri.v3].n << std::endl;

			poi->norm = normalize(uvw[2] * m_vertices[tri.v1].n + uvw[0] * m_vertices[tri.v2].n + uvw[1] * m_vertices[tri.v3].n);
			// std::cout << "after ps "<<poi->norm << std::endl;
#endif
		} else {
			delete temp;
		}
	}

	return poi;
}


std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}
