#ifndef PHYSICS_WORLD
#define PHYSICS_WORLD

#include "Application.h"
#include <stack>

struct DX_ALIGNED CollisionPOD
{
	OP_NEW;
	OP_DEL;

	DynamicBody* pBodyA = nullptr;
	DynamicBody* pBodyB = nullptr;
	float penetration = 0.0f;
	XMVECTOR normal;
};

class DX_ALIGNED PhysicsWorld
{
public:

	PhysicsWorld(DynamicBody*  pDynamicBodies[SPHERE_COUNT]);
	~PhysicsWorld();

	void tick();

	OP_NEW;
	OP_DEL;

private:

	void generateCollisionPairs();
	void clearCollisionStack();

	bool checkIntersection(CollisionPOD& collPod);
	void resolveImpulse(CollisionPOD& collPod);

	void resolveHeightmapCollision(const CollisionPOD& collPod);
	void positionalCorrectionHeightmap(const CollisionPOD& collPod);

	void correctPosition(CollisionPOD& collPod);

	DynamicBody* m_pDynamicBodies[SPHERE_COUNT];
	std::stack<CollisionPOD> m_collisionPODs;
};

#endif 
