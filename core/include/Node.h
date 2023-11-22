#pragma once

#include "../include/OpenGLRenderer.h"

class Node
{
public:
	Node();
	Node(const std::string_view& name);
	virtual ~Node();

	/*
	 * Update a node and all of its children
	 * @param frametime frame delta time
	 */
	virtual void Update(float frametime);

	/**
	 * Render a node and all of its children.
	 * Virtual function, base implementation is empty
	 * @param renderer renderer to use
	 * @param program handle to shader program
	 */
	virtual void Render(IRenderer& renderer, GLuint program);

	/**
	 * Add new child node into the node
	 * @param node a new child to add
	 */
	void AddNode(std::shared_ptr<Node> node);

	/**
	 * Get parent of a node
	 * @return parent node or nullptr if node has no parent
	 */
	inline Node* GetParent() { return m_pParent; }

	/**
	 * Get all child nodes
	 * @return child nodes
	 */
	inline auto& GetNodes() { return m_arrNodes; }
	inline const auto& GetNodes() const { return m_arrNodes; }

	/**
	 * Set position into the local model matrix (last row) of this node
	 * @param pos position to set to model matrix
	 */
	inline void SetPos(const glm::vec3& pos)
	{
		m_mModel[3][0] = pos.x;
		m_mModel[3][1] = pos.y;
		m_mModel[3][2] = pos.z;
	}

	/**
	 * Set position into the local model matrix (last row) of this node
	 * @param x,y,z position to set to model matrix
	 */
	inline void SetPos(float x, float y, float z)
	{
		m_mModel[3][0] = x;
		m_mModel[3][1] = y;
		m_mModel[3][2] = z;
	}

	/**
	 * Get position of the node from model matrix
	 * @return position of the node
	 */
	inline glm::vec3 GetPos() const { return m_mModel[3]; }

	/**
	 * Get node's model matrix
	 * @return a reference to the local model matrix of the node
	 */
	inline auto& GetMatrix() { return m_mModel; }

	/**
	 * Set model matrix to the node.
	 * @param m matrix to set to node
	 */
	inline void SetMatrix(const glm::mat4& m) { m_mModel = m; }


	 /**
	 * Get absolute matrix in world coordinates by multiplying node's parent's
	 * model matrix with node's own model matrix if there is a parent,
	 * otherwise return node's model matrix
	 * @return a model matrix combined with parent's matrix
	 */
	inline glm::mat4 GetWorldMatrix() const { return (m_pParent) ? m_pParent->GetWorldMatrix() * m_mModel : m_mModel; }

	/**
	 * Get velocity of the node
	 * @return reference to node's velocity vector
	 */
	inline auto& GetVelocity() { return m_vVelocity; }

	/**
	 * Set the node's velocity vector
	 * @param velocity new velocity
	 */
	inline void SetVelocity(const glm::vec3& velocity) { m_vVelocity = velocity; }

	/**
	 * Rotate object per axis and angle by applying a rotation matrix to it.
	 * @param axis axis to rotate around
	 * @param angle rotation angle in radians
	 */
	inline void RotateAxisAngle(const glm::vec3& axis, float angle)
	{
		m_vRotationAxis = glm::normalize(axis);
		m_fRotationAngle = angle;

		auto pos = GetPos();
		m_mModel = glm::rotate(glm::mat4(1.0f), m_fRotationAngle, m_vRotationAxis);
		SetPos(pos);
	}

	/**
	 * Get node's rotation axis
	 * @return reference to current rotation axis
	 */
	inline auto& GetRotationAxis() { return m_vRotationAxis; }

	/**
	 * Set node's rotation axis by calling the RotateAxisAngle
	 * @param axis axis to rotate around
	 */
	inline void SetRotationAxis(const glm::vec3& axis)
	{
		RotateAxisAngle(axis, m_fRotationAngle);
	}

	/**
	 * Get node's rotation angle
	 * @return current rotation angle in radians
	 */
	inline float GetRotationAngle() const { return m_fRotationAngle; }

	/**
	 * Set node's rotation angle by calling RotateAxisAngle
	 * @param angle rotation angle in radians
	 */
	inline void SetRotationAngle(float angle)
	{
		RotateAxisAngle(m_vRotationAxis, angle);
	}

	/**
	 * Get node's rotation speed
	 * @return current rotation speed in radians per second
	 */
	inline float GetRotationSpeed() const { return m_fRotationSpeed; }

	/**
	 * Set node's rotation speed
	 * @param speed rotation speed in radians per second
	 */
	inline void SetRotationSpeed(float speed) { m_fRotationSpeed = speed; }

	/**
	 * Get node's radius
	 * @return radius of the node in radians
	 */
	inline float GetRadius() const { return m_fRadius; }

	/**
	 * Set node's radius
	 * @param radius radius to set to the node
	 */
	inline void SetRadius(float radius) { m_fRadius = radius; }

	/**
	 * Get node's name
	 * @return a name of a single node
	 */
	inline const std::string& GetName() const { return m_strName; }

	/**
	 * Set a name for a node
	 * @param name name to be set for the node
	 */
	inline void SetName(const std::string_view name) { m_strName = name; }

	/**
	 * Find a node with specific name by using recursion.
	 * Return a raw pointer that is a temporary pointer and it should not be
	 * stored to any variable as it is not guaranteed to stay valid later on.
	 * @param name name of the node
	 * @return raw pointer of the node if found, null if not
	 */
	Node* FindNode(const std::string_view& name);

protected:
	glm::mat4									m_mModel;
	Node* m_pParent;
	std::vector<std::shared_ptr<Node>>			m_arrNodes;

	// Velocity and rotations
	glm::vec3									m_vVelocity;

	glm::vec3									m_vRotationAxis;
	float										m_fRotationAngle;
	float										m_fRotationSpeed;

	// Size
	float										m_fRadius;

private:
	std::string									m_strName;
};
