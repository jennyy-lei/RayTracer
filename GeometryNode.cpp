// Termm--Fall 2022

#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
	, m_caustic( false )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

void GeometryNode::setTexture( TextureMap *tex, uint pixelsPerUnit = 100 )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	if (m_material) m_material->set_texture(tex);
	texturePixelsPerUnit = pixelsPerUnit;
}

void GeometryNode::setBump( BumpMap *bump, uint pixelsPerUnit = 100 )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	if (m_material) m_material->set_bump(bump);
	bumpPixelsPerUnit = pixelsPerUnit;
}

void GeometryNode::setCaustic()
{
	m_caustic = true;
}

POI *GeometryNode::intersectPrimitive(glm::vec3 e, glm::vec3 r, glm::mat4 trans) {
	POI *poi = m_primitive->intersect(e, r, trans);
	if (poi) poi->gn = this;
	return poi;
}

//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const GeometryNode & node) {
	//os << "Primitive:[Type: ___]"
	os << "Primitive:[";
	os << "Type: ";
	switch(node.m_primitive->type) {
		case PrimitiveType::Sphere:
			os << "Sphere";
			break;
		case PrimitiveType::Cube:
			os << "Cube";
			break;
		case PrimitiveType::NHSphere:
			os << "NHSphere";
			break;
		case PrimitiveType::NHCube:
			os << "NHCube";
			break;
		case PrimitiveType::Mesh:
			os << "Mesh";
			break;
		default:
			os << "None";
	}
	os << "]\n";

	return os;
}
