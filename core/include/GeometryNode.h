#pragma once

#include "../include/Node.h"

// Forward declarations
class Geometry;
struct Material;

class GeometryNode : public Node
{
public:
	GeometryNode(
		const std::shared_ptr<Geometry>& geometry,
		const std::shared_ptr<Material>& material) :
			m_pGeometry(geometry),
			m_pMaterial(material)
	{
	}

	/**
	 * Render geometry object with material
	 * @param renderer renderer to use
	 * @param program handle to shader program
	 */
	void Render(IRenderer& renderer, GLuint program) override;

	/**
	 * Set, switch or disable geometry
	 * @param geometry geometry to be set to the geometry node
	 */
	void SetGeometry(const std::shared_ptr<Geometry>& geometry) { m_pGeometry = geometry; }

	/**
	 * Set, switch or disable material
	 * @param material material to be set to the geometry node
	 */
	void SetMaterial(const std::shared_ptr<Material>& material) { m_pMaterial = material; }

protected:
	std::shared_ptr<Geometry>		m_pGeometry;
	std::shared_ptr<Material>		m_pMaterial;
};