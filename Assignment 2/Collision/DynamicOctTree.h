#ifndef DYNAMIC_OCT_TREE_H
#define DYNAMIC_OCT_TREE_H

#include "Application.h"
#include <stack>

//forward declarations 
struct CollisionPOD;
class DynamicBody;

struct DTreeObject
{
	DTreeObject* pNextObj = nullptr;
	DynamicBody* pDynBody = nullptr; //avoid allocation of new type by storing this 
};

struct DTreeNode
{
	~DTreeNode();
	DTreeNode* pChildren[8]{ nullptr };
	DirectX::XMFLOAT3 centre;
	float halfBounds;
	DTreeObject* pObjList = nullptr;
};

void insert_into_dynamic_tree(DTreeNode* pNode, DynamicBody* pDynamicBody, int maxDepth);

void test_all_collisions(DTreeNode* pNode, std::stack<CollisionPOD>& collisionResults);

//delete all nodes
void cleanup_dynamic_tree(DTreeNode* pNode);

//delete all objects stored in linked lists on all nodes
void clear_dynamic_tree_objects(DTreeNode* pNode);

#endif