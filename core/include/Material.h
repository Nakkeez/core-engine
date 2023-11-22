#pragma once

#include "../include/OpenGLRenderer.h"

struct Material
{
	Material();
	
	/**
	 * Set material properties for lighting to OpenGL shader program
	 */
	void SetToProgram(GLuint program);

	glm::vec4			m_cAmbient; // A base color that is not affected by lighting
	glm::vec4			m_cDiffuse; // A color that is affected by lighting
	glm::vec4			m_cSpecular; // A highlight of lighting
	glm::vec4			m_cEmissive; // A color that is emitted by the material

	float				m_fSpecularPower; // Sharpness of highlight
};