#include "DynamicBody.h"
#include "Application.h"
#include "HeightMap.h"
#include "CommonMesh.h"

using namespace DirectX;

// Dynamic Body \\

DynamicBody::DynamicBody(CommonMesh* pCommonMesh, ColliderBase* pCollider)
	: m_pCommonMesh(pCommonMesh), m_bIsActive(true), m_pBaseCollider(pCollider)
{
	assert(pCommonMesh);
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
	if (!m_bIsActive)
	{
		return;
	}

	m_velocity += (dt / 2.0f) * XMVectorSet(0.0f, G_VALUE, 0.0f, 0.0f); // step acceleration and apply this change to the velocity 
	m_position += dt * m_velocity; // step velocity and calculate the change in position and apply this translation to the current position 
	m_velocity += (dt / 2.0f) * XMVectorSet(0.0f, G_VALUE, 0.0f, 0.0f); // step acceleration and apply this change to the velocity 

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
	float e = 0.6f;
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
		const bool bCollided = pCurrentHeightmap->SphereCollision(m_position, radius, colNormal, penetration);

		if (bCollided)
		{

			XMVECTOR relativeVel = -m_velocity;
			const float velAlongNormal = XMVectorGetX(XMVector3Dot(relativeVel, colNormal));
			if (velAlongNormal < 0)
			{

				return;
			}
			const float j = -(1 + e) * velAlongNormal;
			XMVECTOR impulse = j * colNormal;

			setVelocity(m_velocity - impulse);

			relativeVel = -m_velocity;
			XMVECTOR t = relativeVel - (colNormal * XMVectorGetX(XMVector3Dot(colNormal, relativeVel)));
			t = XMVector3Normalize(t);

			const float staticFric = 0.1f;
			const float dynFric = 0.2f;

			float jTangent = -XMVectorGetX(XMVector3Dot(relativeVel, t));

			if (jTangent != 0.0f)
			{
				if (fabs(jTangent) < j * staticFric)
				{
					setVelocity(m_velocity + (t* jTangent));
				}
				else
				{
					setVelocity(m_velocity + (t * -j * dynFric));
				}
			}
			XMVECTOR correction = (max(penetration - Application::CollisionThreshold, 0.0f)) * Application::CollisionPercentage* colNormal;

			m_position += correction;
		}
		break;
	}
	}
}
