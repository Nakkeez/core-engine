#include "../include/Material.h"

Material::Material() :
	m_cAmbient(0.1f, 0.1f, 0.1f, 1.0f),
	m_cDiffuse(1.0f),
	m_cSpecular(1.0f),
	m_cEmissive(0.0f),
	m_fSpecularPower(50.0f)
{
}

void Material::SetToProgram(GLuint program)
{
	OpenGLRenderer::SetUniformVec4(program, "materialAmbient", m_cAmbient);
	OpenGLRenderer::SetUniformVec4(program, "materialDiffuse", m_cDiffuse);
	OpenGLRenderer::SetUniformVec4(program, "materialSpecular", m_cSpecular);
	OpenGLRenderer::SetUniformVec4(program, "materialEmissive", m_cEmissive);
	OpenGLRenderer::SetUniformFloat(program, "specularPower", m_fSpecularPower);
}
