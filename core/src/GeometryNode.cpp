#include "../include/GeometryNode.h"
#include "../include/Geometry.h"
#include "../include/Material.h"

void GeometryNode::Render(IRenderer& renderer, GLuint program)
{
	// Check that there is geometry as geometry can be initialized with null pointers
	if (m_pGeometry)
	{
		m_pGeometry->SetAttribs(program);

		// Set model matrix to shader uniform
		// Use node's and its parent's combined matrixes so the node will move relative to its parent
		const glm::mat4 worldMatrix(GetWorldMatrix());
		OpenGLRenderer::SetUniformMatrix4(program, "modelMatrix", worldMatrix);

		// Set model-view-projection matrix to shader uniform
		const glm::mat4 modelViewProjectionMatrix(renderer.GetProjectionMatrix() * renderer.GetViewMatrix() * worldMatrix);
		OpenGLRenderer::SetUniformMatrix4(program, "modelViewProjectionMatrix", modelViewProjectionMatrix);

		if (m_pMaterial)
		{
			m_pMaterial->SetToProgram(program);
		}

		m_pGeometry->Draw(renderer);
	}
	// Make sure that all the child nodes will be rendered
	Node::Render(renderer, program);
}
