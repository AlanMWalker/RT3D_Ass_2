#ifndef STATIC_OCTTREE
#define STATIC_OCTTREE

#include "Application.h"

#define INVALID_IDX -1
#define ROOT_IDX -1


/* Prefix 'S' for static */

struct STreeObject // linear oct-tree object
{
	STreeObject* pNextObject = nullptr;
	float radius;
	DirectX::XMFLOAT3 centre;
	int faceIdx;
};

struct STreeNode //linear oct-tree node
{
	DirectX::XMFLOAT3 centre;
	float halfBounds;
	STreeObject* pObjList = nullptr; // linked list of objects
	int childrenIdxs[8];
	int nodeIdx;
	~STreeNode()
	{
		STreeObject* pObj = pObjList;
		do
		{
			pObj = pObjList;
			if (pObjList)
				pObjList = pObj->pNextObject;
			SAFE_FREE(pObj);
		} while (pObjList != nullptr);
	}
};

struct STreeArray
{
	STreeNode* treeArr = nullptr;
	STreeNode root;
};

void build_static_tree(STreeArray* tree, const DirectX::XMFLOAT3& centre, float halfBounds, int maxDepth);

void insert_into_tree(STreeArray* tree, int idx, STreeObject* pObject);

void get_query_list(STreeArray* tree, STreeObject* pObjList, const STreeObject & queryObj);

void cleanup_static_tree(STreeArray* tree);

#endif 