#include "StaticOctTree.h"
#include "XMVectorUtils.h"
#include "PhysicsWorld.h"

using namespace DirectX;

//Private utilities
static bool contains(const STreeNode& node, const STreeObject& obj)
{
	const bool xAxis = (node.centre.x - node.halfBounds < obj.centre.x) && (node.centre.x + node.halfBounds > obj.centre.x);
	const bool yAxis = (node.centre.y - node.halfBounds < obj.centre.y) && (node.centre.y + node.halfBounds > obj.centre.y);
	const bool zAxis = (node.centre.z - node.halfBounds < obj.centre.z) && (node.centre.z + node.halfBounds > obj.centre.z);
	return  xAxis && yAxis && zAxis;
}

static void build_nodes(STreeArray * tree, const XMFLOAT3& centre, float halfBounds, int maxDepth, int nodeIdx)
{
	if (maxDepth < 0)
	{
		return;
	}
	STreeNode* pNode = &tree->treeArr[nodeIdx];
	pNode->centre = centre;
	pNode->halfBounds = halfBounds;
	pNode->pObjList = nullptr;
	pNode->nodeIdx = nodeIdx;

	XMFLOAT3 offset;

	const float step = halfBounds * 0.5f;

	for (int i = 0; i < 8; ++i)
	{
		offset.x = ((i & 1)) ? step : -step;
		offset.y = ((i & 2)) ? step : -step;
		offset.z = ((i & 4)) ? step : -step;
		const int idx = (nodeIdx * 8) + (i);
		build_nodes(tree, centre + offset, step, maxDepth - 1, idx);
		if (maxDepth - 1 >= 0)
		{
			pNode->childrenIdxs[i] = idx;
		}
		else
		{
			pNode->childrenIdxs[i] = -1;
		}
	}
}

static void get_query_list_nodes(STreeArray * tree, int nodeIdx, STreeObject * pList, const STreeObject & queryObj)
{
	STreeNode* pNode = nullptr;
	if (nodeIdx == INVALID_IDX) //if we've reached gone to max depth return
	{
		return;
	}
	else if (nodeIdx == ROOT_IDX)
	{
		pNode = &tree->root;
	}
	else
	{
		pNode = &tree->treeArr[nodeIdx];
	}

	if (!contains(*pNode, queryObj)) // if not in this octant, return
	{
		return;
	}
	else
	{
		for (int i = 0; i < 8; ++i)
		{
			get_query_list_nodes(tree, pNode->childrenIdxs[i], pList, queryObj);
		}

		STreeObject* pLoop = nullptr;
		STreeObject* pEnd = nullptr;
		for (pLoop = pList; pLoop; pLoop = pLoop->pNextObject)
		{
			pEnd = pLoop;
		}
		pEnd->pNextObject = pNode->pObjList;
	}
}

static void get_query_list_nodes(STreeArray * tree, int nodeIdx, std::stack<int>& resultStack, const STreeObject & queryObj)
{
	STreeNode* pNode = nullptr;
	if (nodeIdx == INVALID_IDX) //if we've reached gone to max depth return
	{
		return;
	}
	else if (nodeIdx == ROOT_IDX)
	{
		pNode = &tree->root;
	}
	else
	{
		pNode = &tree->treeArr[nodeIdx];
	}

	if (!contains(*pNode, queryObj)) // if not in this octant, return
	{
		return;
	}
	else
	{
		for (int i = 0; i < 8; ++i)
		{
			get_query_list_nodes(tree, pNode->childrenIdxs[i], resultStack, queryObj);
		}

		STreeObject* pLoop = nullptr;
		STreeObject* pEnd = nullptr;
		for (pLoop = pNode->pObjList; pLoop != nullptr; pLoop = pLoop->pNextObject)
		{
			//if (SpherevsSphere(pLoop->centre, pLoop->radius, queryObj.centre, queryObj.radius))
			{
				resultStack.push(pLoop->faceIdx);
			}
		}
	}
}
// Header defined

void build_static_tree(STreeArray* tree, const XMFLOAT3& centre, float halfBounds, int maxDepth)
{
	static int NodeCount;
	if (!tree->treeArr)
	{
		NodeCount = (int)(pow(8, maxDepth) - 1); // (d^n - 1) / (d - 1)-> where d = 8 and n is max depth
		tree->treeArr = new STreeNode[NodeCount + 1];

		tree->root.centre = centre;
		tree->root.halfBounds = halfBounds;
		tree->root.nodeIdx = ROOT_IDX;
		XMFLOAT3 offset;
		const float step = halfBounds * 0.5f;

		for (int i = 0; i < 8; ++i)
		{
			offset.x = ((i & 1)) ? step : -step;
			offset.y = ((i & 2)) ? step : -step;
			offset.z = ((i & 4)) ? step : -step;
			//const int idx = ((i);
			build_nodes(tree, centre + offset, step, maxDepth - 1, i);
			if (maxDepth - 1 >= 0)
			{
				tree->root.childrenIdxs[i] = i;
			}
			else
			{
				tree->root.childrenIdxs[i] = -1;
			}
		}
	}
}

void insert_into_tree(STreeArray* tree, int idx, STreeObject* pObject)
{
	float objPos[3], nodePos[3];
	int index = 0;
	bool straddle = 0;

	//assert(root);
	//assert(pObject);

	STreeNode* pNode = nullptr;
	if (idx != ROOT_IDX)
	{
		pNode = &tree->treeArr[idx];
	}
	else
	{
		pNode = &tree->root;
	}

	static_assert(sizeof(objPos) == sizeof(XMFLOAT3), "incorrect data sizes!");
	static_assert(sizeof(nodePos) == sizeof(XMFLOAT3), "incorrect data sizes!");

	memcpy_s(objPos, sizeof(objPos), &pObject->centre, sizeof(XMFLOAT3));
	memcpy_s(nodePos, sizeof(nodePos), &pNode->centre, sizeof(XMFLOAT3));

	for (int i = 0; i < 3; ++i)
	{
		const float delta = objPos[i] - nodePos[i];
		if (fabs(delta) <= pObject->radius)
		{
			straddle = true;
			break;
		}

		if (delta > 0.0f)
		{
			index |= (1 << i);
		}
	}

	if (!straddle && pNode->childrenIdxs[index] != INVALID_IDX)
	{
		insert_into_tree(tree, pNode->childrenIdxs[index], pObject);
	}
	else
	{
		pObject->pNextObject = pNode->pObjList;
		pNode->pObjList = pObject;
	}
}

void get_query_list(STreeArray* tree, STreeObject * pObjList, const STreeObject & queryObj)
{
	if (!tree || !pObjList)
	{
		return;
	}

	get_query_list_nodes(tree, ROOT_IDX, pObjList, queryObj);

	int count = 0;
	for (auto pObj = pObjList; pObj; pObj = pObjList->pNextObject)
	{
		++count;
	}
	dprintf("\nTotal obj count - %d\n", count - 1);
}

void get_query_list(STreeArray* tree, std::stack<int>& results, const STreeObject & queryObj)
{
	if (!tree)
	{
		return;
	}

	get_query_list_nodes(tree, ROOT_IDX, results, queryObj);
}

void cleanup_static_tree(STreeArray * tree)
{
	if (!tree)
	{
		return;
	}

	SAFE_FREE_ARR(tree->treeArr);
}
