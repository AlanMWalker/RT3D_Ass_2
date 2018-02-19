#include "PhysicsWorld.h"

PhysicsWorld::PhysicsWorld(DynamicBody * pDynamicBodies[SPHERE_COUNT])
{
	for (int i = 0; i < SPHERE_COUNT; ++i)
	{
		m_pDynamicBodies[i] = pDynamicBodies[i];
	}
}

PhysicsWorld::~PhysicsWorld()
{

}

void PhysicsWorld::tick()
{
	float dt = Application::s_pApp->m_deltaTime;
	for (auto& pDynBody : m_pDynamicBodies)
		pDynBody->updateDynamicBody(dt);

	generateCollisionPairs();
	clearCollisionStack();
}

void PhysicsWorld::generateCollisionPairs()
{
	const auto beginIT = std::begin(m_pDynamicBodies);
	const auto endIT = std::end(m_pDynamicBodies);
	CollisionPOD colPOD;

	for (auto itA = beginIT; itA != endIT; ++itA)
	{
		for (auto itB = beginIT + 1; itB != endIT; ++itB)
		{
			if (itB == itA)
			{
				continue;
			}

			if (!(*itA)->isActive() || !(*itB)->isActive())
			{
				continue;
			}

			colPOD.pBodyA = *itA;
			colPOD.pBodyB = *itB;
			m_collisionPODs.push(colPOD);
		}
	}
}

void PhysicsWorld::clearCollisionStack()
{
	while (!m_collisionPODs.empty())
	{
		CollisionPOD collPOD = m_collisionPODs.top();
		bool result = checkIntersection(collPOD);
		if (result)
		{
			resolveImpulse(collPOD);
			correctPosition(collPOD);
		}

		m_collisionPODs.pop();
	}
}

bool PhysicsWorld::checkIntersection(CollisionPOD & collPod)
{
	// ideal -> Simplify to lookup table
	if (collPod.pBodyA->getColliderBase()->colliderType == Sphere && collPod.pBodyB->getColliderBase()->colliderType == Sphere)
	{
		const XMVECTOR &posA = collPod.pBodyA->getPosition(), &posB = collPod.pBodyB->getPosition();
		const float radiusA = static_cast<SphereCollider*>(collPod.pBodyA->getColliderBase())->radius;
		const float radiusB = static_cast<SphereCollider*>(collPod.pBodyB->getColliderBase())->radius;

		float distSquared = XMVectorGetX(XMVector3LengthSq(posA - posB));
		if (distSquared > (radiusA + radiusB) * (radiusA + radiusB))
		{
			return false;
		}

		float distance = sqrtf(distSquared);

		if (distance == 0.0f)
		{
			collPod.normal = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		}
		else
		{
			collPod.normal = (posB - posA) / distance;
		}
		collPod.penetration = (radiusA + radiusB) - distance;
		return true;
	}

	return false;
}

void PhysicsWorld::resolveImpulse(CollisionPOD & collPOD)
{
	//DynamicBody
	float e = 0.15f;
	//setPosition(colPos);

	XMVECTOR relativeVel = collPOD.pBodyB->getVelocity() - collPOD.pBodyA->getVelocity();
	const float velAlongNormal = XMVectorGetX(XMVector3Dot(relativeVel, collPOD.normal));
	const float j = -(1 + e) * velAlongNormal;
	XMVECTOR impulse = j * collPOD.normal;

	collPOD.pBodyA->setVelocity(collPOD.pBodyA->getVelocity() - impulse);
	collPOD.pBodyB->setVelocity(collPOD.pBodyB->getVelocity() + impulse);
}

void PhysicsWorld::correctPosition(CollisionPOD & collPod)
{
	////float penetration = radius - XMVectorGetX(XMVector3Length(colPos - m_position));
	const static float correctionThreshold = 0.01f;
	const static float correctPercentage = 0.6f;
	XMVECTOR correction = (max(collPod.penetration - correctionThreshold, 0.0f)) * correctPercentage * collPod.normal;

	collPod.pBodyA->setPosition(collPod.pBodyA->getPosition() - correction);
	collPod.pBodyB->setPosition(collPod.pBodyB->getPosition() + correction);
}
