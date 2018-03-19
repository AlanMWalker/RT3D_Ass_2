#include "DynamicBody.h"
#include "Application.h"
#include "HeightMap.h"
#include "CommonMesh.h"
#include "PhysicsWorld.h"

using namespace DirectX;

// Dynamic Body \\

DynamicBody::DynamicBody(CommonMesh* pCommonMesh, ColliderBase* pCollider)
	: m_pCommonMesh(pCommonMesh), m_bIsActive(true), m_pBaseCollider(pCollider)
{
	assert(pCommonMesh);
	assert(pCollider);
	setVelocity(XMFLOAT3(0, 0, 0));
	setPosition(XMFLOAT3(0, 0, 0));
	m_pHeightMapCollision = new CollisionPOD;

	memset(m_pHeightMapCollision, 0, sizeof(CollisionPOD));
	m_pHeightMapCollision->pBodyA = this;

	m_worldMatrix = XMMatrixTranslation(XMVectorGetX(m_position), XMVectorGetY(m_position), XMVectorGetZ(m_position));
}

DynamicBody::~DynamicBody()
{
	SAFE_FREE(m_pBaseCollider);
	SAFE_FREE(m_pHeightMapCollision);
}

void DynamicBody::updateDynamicBody(float dt)
{
	if (!m_bIsActive)
	{
		return;
	}
	checkHeightMapCollision();

	//m_velocity += dt * XMVectorSet(0.0f, G_VALUE, 0.0f, 0.0f);
	m_velocity += (dt / 2.0f) * XMVectorSet(0.0f, G_VALUE, 0.0f, 0.0f); // step acceleration and apply this change to the velocity 
	m_position += dt * m_velocity; // step velocity and calculate the change in position and apply this translation to the current position 
	m_velocity += (dt / 2.0f) * XMVectorSet(0.0f, G_VALUE, 0.0f, 0.0f); // step acceleration and apply this change to the velocity 

	m_worldMatrix = XMMatrixTranslation(XMVectorGetX(m_position), XMVectorGetY(m_position), XMVectorGetZ(m_position));
}

void DynamicBody::setPosition(const DirectX::XMVECTOR & pos)
{
	m_position = pos;
}

void DynamicBody::setPosition(const DirectX::XMFLOAT3 & pos)
{
	m_position = XMLoadFloat3(&pos);
}

void DynamicBody::setVelocity(const DirectX::XMVECTOR & vel)
{
	m_velocity = vel;
}

void DynamicBody::setVelocity(const DirectX::XMFLOAT3 & vel)
{
	m_velocity = XMLoadFloat3(&vel);
}

void DynamicBody::applyImpulse(const XMFLOAT3 & impulse)
{
	XMVECTOR impulseVec = XMVectorSet(impulse.x, impulse.y, impulse.z, 0.0f);
	XMVECTOR vel = getVelocity();
	vel += (m_invMass * impulseVec);

	setVelocity(vel);
}

void DynamicBody::setMass(float mass)
{
	if (mass != 0.0f)
	{
		m_mass = mass;
		m_invMass = 1.0f / mass;
	}
	else
	{
		m_mass = 0;
		m_invMass = 0.0f;
	}
}

void DynamicBody::checkHeightMapCollision()
{
	float e = 0.4f;
	HeightMap* pCurrentHeightmap = Application::s_pApp->GetHeightmap();
	assert(pCurrentHeightmap);

	switch (m_pBaseCollider->colliderType)
	{

	default:
	case Ray:
	{
		XMVECTOR colPos;
		XMVECTOR colNormal;

		const bool bCollided = pCurrentHeightmap->RayCollision(m_position, m_velocity, XMVectorGetX(XMVector3Length(m_velocity)), colPos, colNormal);
		if (bCollided)
		{
			setPosition(colPos);

			XMVECTOR relativeVel = -m_velocity;
			const float velAlongNormal = XMVectorGetX(XMVector3Dot(relativeVel, colNormal));
			const float j = -(1 + e) * velAlongNormal;
			XMVECTOR impulse = j * colNormal;

			setVelocity(m_velocity - impulse);
		}
		break;
	}
	case Sphere:
	{
		XMVECTOR colPos;
		XMVECTOR colNormal;
		float radius = static_cast<SphereCollider*>(m_pBaseCollider)->radius;
		float penetration;
		m_bDidHeightmapCollide = pCurrentHeightmap->SphereCollision(m_position, radius, colNormal, penetration);

		if (m_bDidHeightmapCollide)
		{
			m_pHeightMapCollision->normal = colNormal;
			m_pHeightMapCollision->penetration = penetration;

			return;
		}
		break;
	}
	}
}

const CollisionPOD * const DynamicBody::getHeightmapCollisionData() const
{
	return m_pHeightMapCollision;
}
