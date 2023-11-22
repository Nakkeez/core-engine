#pragma once

// Include glm, do this before including windows to prevent collisions
#include "../glm-master/glm/glm.hpp"
#include "../glm-master/glm/gtc/matrix_transform.hpp"
#include "../glm-master/glm/gtc/random.hpp"
#include <string_view>

class IRenderer
{
public:
	IRenderer() :
		m_mView(1.0f),
		m_mProjection(1.0f),
		m_vLightPosition(0.0f, 1.0f, 0.0f)
	{
		m_mShadowBias = glm::mat4(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);

	}

	/**
	 * Pure virtual create method
	 * @return true if succesful, or false to cancel app startup
	 */
	virtual bool Create() = 0;

	/**
	 * Pure virtual page flipper function, all renderers must implement.
	 * Transfer graphics that are produced by one frame to the screen
	 */
	virtual void Flip() = 0;

	/**
	 * Clear the color buffer, depth buffer and stencil
	 */
	virtual void Clear(float r, float g, float b, float a, float depth = 1.0f, int32_t stencil = 0) = 0;
	void Clear(const glm::vec4& color, float depth = 1.0f, int32_t stencil = 0)
	{
		Clear(color.r, color.g, color.b, color.a, depth, stencil);
	}

	/**
	 * Specify the pixel area of graphics context where to render
	 * @param area, x, y, width, height of the viewport area
	 */
	virtual void SetViewport(const glm::ivec4& area) = 0;

	/**
	 * Set the texture active into rendering
	 * @param program
	 * @param texture handle
	 * @param slot index of the texture slot
	 * @param uniformName name of the uniform in shader
	 * @return true if uniform is found and set
	 */
	virtual bool SetTexture(uint32_t program, uint32_t texture, int32_t slot, const std::string_view& uniformName) = 0;

	// Access to view and projection matrices
	glm::mat4& GetViewMatrix() { return m_mView; }
	glm::mat4& GetProjectionMatrix() { return m_mProjection; }
	const glm::mat4& GetViewMatrix() const { return m_mView; }
	const glm::mat4& GetProjectionMatrix() const { return m_mProjection; }
	void SetViewMatrix(const glm::mat4& m) { m_mView = m; }
	void SetProjectionMatrix(const glm::mat4& m) { m_mProjection = m; }

	// Shadow bias
	const glm::mat4& GetShadowBiasMatrix() const { return m_mShadowBias; }
	void SetShadowBiasMatrix(const glm::mat4& m) { m_mShadowBias = m; }

	// Light position
	const glm::vec3& GetLightPos() const { return m_vLightPosition; }
	void SetLightPos(const glm::vec3& lightPos) { m_vLightPosition = lightPos; }
	void SetLightPos(float x, float y, float z) { m_vLightPosition = glm::vec3(x, y, z); }


protected:
	// View and perpective projection matrices
	glm::mat4		m_mView;
	glm::mat4		m_mProjection;

	// Lights & shadows
	glm::mat4		m_mShadowBias;
	glm::vec3		m_vLightPosition;
};
