#include "../include/CameraNode.h"

CameraNode::CameraNode()
{
	SetProjectionParameters(glm::half_pi<float>(), 1.0f, 1.0f, 500.0f);
}

CameraNode::CameraNode(float fov, float aspect, float nearplane, float farplane)
{
	SetProjectionParameters(fov, aspect, nearplane, farplane);
}

CameraNode::CameraNode(const glm::vec4& parameters)
{
	SetProjectionParameters(parameters);
}

void CameraNode::Update(float frametime)
{
	Node::Update(frametime);
}

void CameraNode::Render(IRenderer& renderer, GLuint program)
{
	Node::Render(renderer, program);
}

void CameraNode::SetProjectionParameters(float fov, float aspect, float nearplane, float farplane)
{
	// Build the projection matrix
	m_mProjection = glm::perspective(fov, aspect, nearplane, farplane);
	// Store new projection parameters
	m_fFov = fov;
	m_fAspect = aspect;
	m_fNearplane = nearplane;
	m_fFarplane = farplane;
}

glm::vec4 CameraNode::GetProjectionParameters() const
{
	return glm::vec4(m_fFov, m_fAspect, m_fNearplane, m_fFarplane);
}
