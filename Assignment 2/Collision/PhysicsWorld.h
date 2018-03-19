#ifndef PHYSICS_WORLD
#define PHYSICS_WORLD

#include "Application.h"
#include <stack>

//forward declarations
struct DTreeNode;

struct DX_ALIGNED CollisionPOD
{
	OP_NEW;
	OP_DEL;

	DynamicBody* pBodyA = nullptr;
	DynamicBody* pBodyB = nullptr;
	float penetration = 0.0f;
	XMVECTOR normal;
};

//Quick lightweight test (used for static tree collision detection with heightmap)
bool SpherevsSphere(const XMFLOAT3& centreA, float radiusA, const XMFLOAT3& centreB, float radiusB);

//More intense detection which calculates normal and collision detection results
bool SpherevsSpherePaired(CollisionPOD& collPod);

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

	void resolveImpulse(CollisionPOD& collPod);

	void resolveHeightmapCollision(const CollisionPOD& collPod);
	void positionalCorrectionHeightmap(const CollisionPOD& collPod);

	void correctPosition(CollisionPOD& collPod);

	DynamicBody* m_pDynamicBodies[SPHERE_COUNT];
	std::stack<CollisionPOD> m_collisionPODs;

	DTreeNode* m_pRootNode = nullptr;
};

#endif 
