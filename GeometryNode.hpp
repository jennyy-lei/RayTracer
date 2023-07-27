// Termm--Fall 2022

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "ImageMap.hpp"

#include <glm/glm.hpp>
class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );
	void setTexture( TextureMap *texture, uint pixelsPerUnit );
	void setBump( BumpMap *bump, uint pixelsPerUnit );
	void setCaustic();
	POI *intersectPrimitive(glm::vec3 e, glm::vec3 r, glm::mat4 trans);
	
	friend std::ostream & operator << (std::ostream & os, const GeometryNode & node);
	
	Material *m_material;
	Primitive *m_primitive;
	bool m_caustic;
	uint texturePixelsPerUnit, bumpPixelsPerUnit;
};
