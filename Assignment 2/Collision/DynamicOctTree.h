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

//Insert dynamic bodies into tree
void insert_into_dynamic_tree(DTreeNode* pNode, DynamicBody* pDynamicBody, int maxDepth);

//test all collisions and produce a stack of collision pairs
void test_all_collisions(DTreeNode* pNode, std::stack<CollisionPOD>& collisionResults);

//clean up dynami tree nodes
void cleanup_dynamic_tree(DTreeNode* pNode);

#endif