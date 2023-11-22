#pragma once

#include "../include/Node.h"


class CameraNode : public Node
{
public:
	CameraNode();
	CameraNode(float fov, float aspect, float nearplane, float farplane);
	CameraNode(const glm::vec4& parameters);

	/*
	 * Update node and its children
	 * @param frametime frame delta time
	 */
	void Update(float frametime) override;

	/**
	 * Render node and its children
	 * @param renderer renderer to use
	 * @param program handle to shader program
	 */
	void Render(IRenderer& renderer, GLuint program) override;

	/**
	 * Change camera projection values.
	 * This function builds camera projection matrix with horizontal fov,
	 * but does not set it automatically to rendering device
	 * @param fov field of view angle in radians
	 * @param aspect view aspect ratio
	 * @param nearplane near clipping plane
	 * @param farplane far clipping plane
	 */
	void SetProjectionParameters(
		float fov,
		float aspect,
		float nearplane,
		float farplane);

	/**
	 * Change camera projection values.
	 * This function builds camera projection matrix with horizontal fov,
	 * but does not set it automatically to rendering device
	 * @param parameters vec4 of fov in radians, aspect, nearplane, farplane
	 */
	inline void SetProjectionParameters(const glm::vec4& parameters)
	{
		SetProjectionParameters(parameters.x, parameters.y, parameters.z, parameters.w);
	}

	/**
	 * Get current camera projection parameters
	 * @return vec4 of fov in radians, aspect, nearplane, farplane
	 */
	glm::vec4 GetProjectionParameters() const;

	/**
	 * Get camera's projection matrix
	 * @return reference to camera's projection matrix
	 */
	inline glm::mat4& GetProjectionMatrix() { return m_mProjection; }
	inline const glm::mat4& GetProjectionMatrix() const noexcept { return m_mProjection; }

	/**
	 * Set camera's projection matrix
	 * @param m camera projection matrix to set
	 */
	inline void SetProjectionMatrix(const glm::mat4& m) { m_mProjection = m; }

	/**
	 * Get camera's view matrix
	 * @return camera view matrix
	 */
	inline glm::mat4 GetViewMatrix() const { return glm::inverse(GetWorldMatrix()); }

	/**
	 * LookAt
	 * Make camera node look at
	 * @param from position from where camera is looking
	 * @param at point that camera is looking
	 */
	inline void LookAt(const glm::vec3& from, const glm::vec3& at)
	{
		m_mModel = glm::inverse(glm::lookAt(from, at, glm::vec3(0.0f, 1.0f, 0.0f)));
	}

protected:
	// Camera matrices
	glm::mat4					m_mProjection;

	// Projection parameters
	float						m_fFov;
	float						m_fAspect;
	float						m_fNearplane;
	float						m_fFarplane;
};
