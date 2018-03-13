#ifndef DYNAMIC_BODY_H
#define DYNAMIC_BODY_H

#include "Macro.h"	

#include <DirectXMath.h>
#include <vector>

class CommonMesh;
class HeightMap;

enum ColliderTypes3D
{
	AABB,
	Sphere,
	OBB,
	Ray
};

struct ColliderBase
{
	ColliderTypes3D colliderType;
};

struct RayCollider : public ColliderBase
{
	RayCollider()
	{
		colliderType = Ray;
	}
};

struct SphereCollider : public ColliderBase
{
	SphereCollider()
	{
		colliderType = Sphere;
	}
	float radius;
};

// class which will manage a fixed pool of dynamic bodies
// and provide a pointer to them when queried
// also provides a stack of all bodies which need to be drawn
// ignoring ones not currently active
// spheres at rest for a certain duration will despawn 
// (DynamicBody is responsible for freeing the collider)
class DX_ALIGNED DynamicBody
{
public:

	DynamicBody(CommonMesh* pCommonMesh, ColliderBase* pBaseCollider);
	~DynamicBody();

	OP_NEW;
	OP_DEL;

	void updateDynamicBody(float dt);
	void updatePositions(float dt);

	void setPosition(const DirectX::XMVECTOR& pos);
	void setPosition(const DirectX::XMFLOAT3& pos);

	void setVelocity(const DirectX::XMVECTOR& vel);
	void setVelocity(const DirectX::XMFLOAT3& vel);

	const DirectX::XMVECTOR& getPosition() const { return m_position; }
	const DirectX::XMVECTOR& getVelocity() const { return m_velocity; }

	const DirectX::XMMATRIX& getWorldMatrix() const { return m_worldMatrix; }

	void applyImpulse(const DirectX::XMFLOAT3& impulse);

	CommonMesh* const getCommonMesh() { return m_pCommonMesh; }
	ColliderBase* const getColliderBase() { return m_pBaseCollider; }
	void setActivityFlag(bool bIsActive) { m_bIsActive = bIsActive; }

	bool isActive() const { return m_bIsActive; }

	float getMass() const { return m_mass; }

	float getInverseMass() const { return m_invMass; }

	void setMass(float mass);

	void checkHeightMapCollision();
private:


	DirectX::XMVECTOR m_velocity;
	DirectX::XMVECTOR m_position;

	DirectX::XMMATRIX m_worldMatrix;

	CommonMesh* m_pCommonMesh;
	HeightMap* m_pHeightMap;
	ColliderBase* m_pBaseCollider;

	bool m_bIsActive;

	float m_mass = 1.0f;
	float m_invMass = 1.0f / m_mass;
};

#endif // !DYNAMIC_BODY_H
