// Termm--Fall 2022

#include "Material.hpp"

Material::Material(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double transmission, double ior )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_reflection(0)
	, m_transmission(transmission)
	, m_ior(ior)
	, textureMap{nullptr}
	, bumpMap{nullptr}
{}

Material::~Material()
{}
