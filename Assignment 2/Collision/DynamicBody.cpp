#include "DynamicBody.h"
#include "Application.h"
#include "HeightMap.h"
#include "CommonMesh.h"

using namespace DirectX;

// Dynamic Body \\

DynamicBody::DynamicBody(CommonMesh* pCommonMesh, ColliderBase* pCollider, HeightMap* pHeightMap)
	: m_pCommonMesh(pCommonMesh), m_pHeightMap(pHeightMap), m_bCollided(false), m_bIsActive(true), m_pBaseCollider(pCollider)
{
	assert(pCommonMesh);
	assert(pHeightMap);
	assert(pCollider);
	setVelocity(XMFLOAT3(0, 0, 0));
	setPosition(XMFLOAT3(0, 0, 0));
	m_worldMatrix = XMMatrixTranslation(XMVectorGetX(m_position), XMVectorGetY(m_position), XMVectorGetZ(m_position));
}

DynamicBody::~DynamicBody()
{
	SAFE_FREE(m_pBaseCollider);
}

void DynamicBody::updateDynamicBody(float dt)
{
	if (m_bCollided || !m_bIsActive)
	{
		return;
	}

	m_velocity += dt * XMVectorSet(0.0f, G_VALUE, 0.0f, 0.0f); // step acceleration and apply this change to the velocity 
	m_position += dt * m_velocity; // step velocity and calculate the change in position and apply this translation to the current position 

	checkHeightMapCollision();
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

void DynamicBody::checkHeightMapCollision()
{
	switch (m_pBaseCollider->colliderType)
	{

	default:
	case Ray:
	{
		XMVECTOR colPos;
		XMVECTOR colNormal;

		m_bCollided = m_pHeightMap->RayCollision(m_position, m_velocity, XMVectorGetX(XMVector3Length(m_velocity)), colPos, colNormal);
		if (m_bCollided)
		{
			float e = 0.9f;
			setPosition(colPos);

			XMVECTOR relativeVel = -m_velocity;
			const float velAlongNormal = XMVectorGetX(XMVector3Dot(relativeVel, colNormal));
			const float j = -(1 + e) * velAlongNormal;
			XMVECTOR impulse = j * colNormal;

			setVelocity(m_velocity - impulse);
			m_bCollided = false;
		}
		break;
	}
	case Sphere:
	{
		XMVECTOR colPos;
		XMVECTOR colNormal;
		float radius = static_cast<SphereCollider*>(m_pBaseCollider)->radius;
		float penetration;
		//m_bCollided = m_pHeightMap->RayCollision(m_position, m_velocity, radius, colPos, colNormal);
		m_bCollided = m_pHeightMap->SphereCollision(m_position, radius, colNormal,penetration);

		if (m_bCollided)
		{
			float e = 0.2f;
			//setPosition(colPos);

			XMVECTOR relativeVel = -m_velocity;
			const float velAlongNormal = XMVectorGetX(XMVector3Dot(relativeVel, colNormal));
			const float j = -(1 + e) * velAlongNormal;
			XMVECTOR impulse = j * colNormal;

			setVelocity(m_velocity - impulse);

			//float penetration = radius - XMVectorGetX(XMVector3Length(colPos - m_position));
			const static float correctionThreshold = 0.0001f;
			const static float correctPercentage = 0.8f;
			XMVECTOR correction = (max(penetration - correctionThreshold, 0.0f)) * correctPercentage * colNormal;
			
			m_position += correction;
			m_bCollided = false;

		}
		break;
	}
	}
}
