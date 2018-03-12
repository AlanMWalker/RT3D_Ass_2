#ifndef APPLICATION_H
#define APPLICATION_H

#define WIN32_LEAN_AND_MEAN

#include <assert.h>

#include <stdio.h>
#include <windows.h>
#include <d3d11.h>


#include "CommonApp.h"
#include "CommonMesh.h"
#include "Macro.h"
#include "DynamicBody.h"

#include <array>

class HeightMap;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
// An example of creating a shader of your own, that fits in
// relatively neatly with the CommonApp functionality.
//
// Edit the shader as the program runs, then Alt+Tab back to it and
// press F5 to reload the shader. Instant feedback!
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#define NON_SLOWED_DT 0.016f
#define SLOWED_DT 0.001f
#define G_VALUE -50.0f

#define SPHERE_COUNT 100
#define MAX_HEIGHTMAPS_COUNT 4

class PhysicsWorld;

class Application :
	public CommonApp
{
public:
	static Application* s_pApp;

	float m_deltaTime = NON_SLOWED_DT;
	static const float CollisionThreshold;
	static const float CollisionPercentage;

	HeightMap* GetHeightmap() { return m_pCurrentHeightmap; }

protected:

	bool HandleStart();
	void HandleStop();
	void HandleUpdate();
	void HandleRender();

private:

	DynamicBody * getNextAvailableBody();

	float m_frameCount;

	bool m_reload;

	float m_rotationAngle;
	float m_cameraZ;

	bool m_bWireframe;

	int m_cameraState;

	HeightMap* m_heightMapPtrs[MAX_HEIGHTMAPS_COUNT] = { nullptr, nullptr, nullptr, nullptr };
	HeightMap* m_pCurrentHeightmap = nullptr;

	PhysicsWorld* m_pPhysicsWorld;

	DynamicBody* m_dynamicBodyPtrs[SPHERE_COUNT];

	XMFLOAT3 mSpherePos;
	XMFLOAT3 mSphereVel;
	float mSphereSpeed;
	bool mSphereCollided;
	XMFLOAT3 mGravityAcc;

	void ReloadShaders();
};

#endif