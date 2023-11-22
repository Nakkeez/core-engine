#include "../include/Node.h"

Node::Node() :
	m_mModel(1.0f),
	m_pParent(nullptr),
	m_vRotationAxis(0.0f, 0.0f, -1.0f),
	m_fRotationAngle(0.0f),
	m_fRotationSpeed(0.0f),
	m_vVelocity(0.0f),
	m_fRadius(1.0f)
{
}

Node::Node(const std::string_view& name) :
	m_mModel(1.0f),
	m_pParent(nullptr),
	m_vRotationAxis(0.0f, 0.0f, -1.0f),
	m_fRotationAngle(0.0f),
	m_fRotationSpeed(0.0f),
	m_vVelocity(0.0f),
	m_fRadius(1.0f),
	m_strName(name)
{
}

Node::~Node()
{
}

void Node::Update(float frametime)
{
	// Apply velocity by moving position by vector length per second
	auto pos = GetPos();
	pos += m_vVelocity * frametime;

	// Update rotations
	if (m_fRotationSpeed != 0.0f)
	{
		m_mModel = glm::rotate(glm::mat4(1.0f), m_fRotationAngle, m_vRotationAxis);

		m_fRotationAngle += m_fRotationSpeed * frametime;
		constexpr float pi2 = glm::two_pi<float>();
		while (m_fRotationAngle > pi2) m_fRotationAngle -= pi2;
		while (m_fRotationAngle < -pi2) m_fRotationAngle += pi2;
	}

	// Set updated position back to the model matrix
	SetPos(pos);

	// Update child nodes
	for (auto& node : m_arrNodes)
	{
		node->Update(frametime);
	}
}

void Node::Render(IRenderer& renderer, GLuint program)
{
	// Render child nodes
	for (auto& node : m_arrNodes)
	{
		node->Render(renderer, program);
	}
}

void Node::AddNode(std::shared_ptr<Node> node)
{
	// Link new child to a parent
	node->m_pParent = this;

	// Add child node
	m_arrNodes.push_back(node);
}

Node* Node::FindNode(const std::string_view& name)
{
	if (GetName() == name) return this;

	for (const auto& node : m_arrNodes)
	{
		auto n = node->FindNode(name);
		if (n)
		{
			return n;
		}
	}

	return nullptr;
}
