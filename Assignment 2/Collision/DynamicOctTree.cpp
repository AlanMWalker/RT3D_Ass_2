#include "DynamicOctTree.h"
#include "XMVectorUtils.h"
#include "PhysicsWorld.h"

//Non-header defined
static DTreeNode* build_node(int nodeIdx, float parentHalfBounds, const XMFLOAT3& parentCentre)
{
	const float halfBounds = parentHalfBounds * 0.5f;
	XMFLOAT3 offset;

	offset.x = (nodeIdx & 1) ? halfBounds : -halfBounds;
	offset.y = (nodeIdx & 2) ? halfBounds : -halfBounds;
	offset.z = (nodeIdx & 4) ? halfBounds : -halfBounds;

	DTreeNode* pNode = new DTreeNode;
	if (pNode)
	{
		pNode->centre = parentCentre + offset;
		pNode->halfBounds = halfBounds;
		pNode->pObjList = nullptr;
		for (int i = 0; i < 8; ++i)
		{
			pNode->pChildren[i] = nullptr;
		}
	}
	return pNode;
}

//Header defined 
void insert_into_dynamic_tree(DTreeNode * pNode, DynamicBody * pDynamicBody, int maxDepth)
{
	int index = 0;
	bool straddle = false;
	static float objPos[3], nodePos[3];

	static_assert(sizeof(objPos) == sizeof(XMFLOAT3), "incorrect data sizes!");
	static_assert(sizeof(nodePos) == sizeof(XMFLOAT3), "incorrect data sizes!");

	if (!pNode)
	{
		return;
	}

	//book uses array format called "Point" which is a float array 
	//to mititgate converting data structures memcpy the struct into a float
	//to allow index based iteration
	memcpy_s(objPos, sizeof(objPos), &pDynamicBody->getPosition(), sizeof(XMFLOAT3));
	memcpy_s(objPos, sizeof(objPos), &pDynamicBody->getPosition(), sizeof(XMFLOAT3));

	const float radius = static_cast<SphereCollider* const>(pDynamicBody->getColliderBase())->radius;

	for (int i = 0; i < 3; ++i)
	{
		const float delta = objPos[i] - nodePos[i];

		if (fabs(delta) <= radius)
		{
			straddle = true;
		}

		if (delta > 0.0f)
		{
			index |= (1 << i);
		}
	}

	if (!straddle && maxDepth >= 0)
	{
		if (!pNode->pChildren[index])
		{
			pNode->pChildren[index] = build_node(index, pNode->halfBounds, pNode->centre);
		}

		insert_into_dynamic_tree(pNode->pChildren[index], pDynamicBody, maxDepth - 1);
	}
	else
	{
		DTreeObject* pObj = new DTreeObject;
		pObj->pDynBody = pDynamicBody;
		pObj->pNextObj = pNode->pObjList;
		pNode->pObjList = pObj;
	}
}

void test_all_collisions(DTreeNode * pNode, std::stack<CollisionPOD>& collisionResults)
{
	const int MAX_DEPTH = 40;
	static DTreeNode* ancestorStack[MAX_DEPTH];
	static int depth = 0;

	ancestorStack[depth++] = pNode;

	for (int n = 0; n < depth; ++n)
	{
		DTreeObject* pObjA = nullptr, *pObjB = nullptr;
		for (pObjA = ancestorStack[n]->pObjList; pObjA; pObjA = pObjA->pNextObj)
		{
			for (pObjB = pNode->pObjList; pObjB; pObjB = pObjB->pNextObj)
			{
				if (pObjA == pObjB)
				{
					break;
				}

				if (!pObjA || !pObjB)
					break;
				CollisionPOD pod;
				pod.pBodyA = pObjA->pDynBody;
				pod.pBodyB = pObjB->pDynBody;
				const bool result = SpherevsSpherePaired(pod);
				if (result)
				{
					collisionResults.push(pod);
				}
			}
		}
	}

	for (int i = 0; i < 8; ++i)
	{
		if (pNode->pChildren[i])
		{
			test_all_collisions(pNode->pChildren[i], collisionResults);
		}
	}
	depth--;
}

void cleanup_dynamic_tree(DTreeNode* pNode)
{
	if (!pNode)
	{
		return;
	}
	for (int i = 0; i < 8; ++i)
	{
		cleanup_dynamic_tree(pNode->pChildren[i]);
	}
	SAFE_FREE(pNode);
	//pNode->~DTreeNode();
}

DTreeNode::~DTreeNode()
{
	DTreeObject* pObj = pObjList;
	while (pObj)
	{
		DTreeObject* temp = pObj->pNextObj;
		SAFE_FREE(pObj);
		pObj = temp;
	}
	pObjList = nullptr;
}
