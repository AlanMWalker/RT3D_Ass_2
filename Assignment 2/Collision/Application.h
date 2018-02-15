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

class Application :
	public CommonApp
{
public:
	static Application* s_pApp;
protected:
	bool HandleStart();
	void HandleStop();
	void HandleUpdate();
	void HandleRender();
private:

	float m_frameCount;

	bool m_reload;

	float m_rotationAngle;
	float m_cameraZ;
	float m_deltaTime = NON_SLOWED_DT;

	bool m_bWireframe;

	int m_cameraState;

	HeightMap* m_pHeightMap;

	CommonMesh *m_pSphereMesh;

	struct DX_ALIGNED DynamicBody
	{
		OP_NEW;
		OP_DEL;

		CommonMesh* pCommonMesh;

		XMVECTOR velocity;
		XMVECTOR position;
		//XMVECTOR force; 
		XMMATRIX worldMatrix;
		bool collided;
		DynamicBody()
			: pCommonMesh(nullptr), collided(false)
		{
			velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		}

		void tick()
		{
			const float dt = Application::s_pApp->m_appBaseDT;

			//time step integration 
			//using symplectic euler
			if (!collided)
			{
				velocity += XMVectorSet(0.0, G_VALUE, 0.0f, 0.0f) * dt;
				position += velocity * dt;
			}

			XMFLOAT3 float3Pos;
			XMStoreFloat3(&float3Pos, position);

			worldMatrix = XMMatrixTranslation(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));
		}

		void setPosition(const XMFLOAT3& pos)
		{
			position = XMLoadFloat3(&pos);
		}

		void setVelocity(const XMFLOAT3& vel)
		{
			velocity = XMLoadFloat3(&vel);
		}
	};

	DynamicBody m_dynamicBody;

	XMFLOAT3 mSpherePos;
	XMFLOAT3 mSphereVel;
	float mSphereSpeed;
	bool mSphereCollided;
	XMFLOAT3 mGravityAcc;

	void ReloadShaders();
};

#endif