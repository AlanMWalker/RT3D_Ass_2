#include "Application.h"
#include "HeightMap.h"
#include "PhysicsWorld.h"
#include <future>

Application* Application::s_pApp = NULL;

const float Application::CollisionThreshold = 0.001f;
const float Application::CollisionPercentage = 0.99f;

const int CAMERA_TOP = 0;
const int CAMERA_ROTATE = 1;
const int CAMERA_MAX = 2;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool Application::HandleStart()
{
	s_pApp = this;

	m_frameCount = 0.0f;

	m_bWireframe = true;
	//m_pHeightMap = new HeightMap("Resources/heightmap.bmp", 2.0f, 0.75f);

	m_heightMapPtrs[0] = new HeightMap("Resources/heightmap_a.bmp", 2.0f, 0.75f);
	m_heightMapPtrs[1] = new HeightMap("Resources/heightmap_b.bmp", 2.0f, 0.75f);
	m_heightMapPtrs[2] = new HeightMap("Resources/heightmap_c.bmp", 2.0f, 0.75f);
	m_heightMapPtrs[3] = new HeightMap("Resources/heightmap_d.bmp", 2.0f, 0.75f);

	m_pCurrentHeightmap = m_heightMapPtrs[1];

	mSpherePos = XMFLOAT3(-14.0, 20.0f, -14.0f);
	mSphereVel = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mGravityAcc = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_cameraZ = 50.0f;
	m_rotationAngle = 0.f;

	m_reload = false;

	ReloadShaders();

	if (!this->CommonApp::HandleStart())
		return false;

	this->SetRasterizerState(false, m_bWireframe);

	m_cameraState = CAMERA_ROTATE;

	mSphereCollided = false;
	for (int i = 0; i < SPHERE_COUNT; ++i)
	{
		//if (i < 2)
		//{
		//	m_dynamicBodyPtrs[i] = new DynamicBody(CommonMesh::NewSphereMesh(this, 1.0f, 16, 16), new RayCollider, m_pHeightMap);
		//}
		//else
		{
			SphereCollider* pSphereCollider = new SphereCollider;
			pSphereCollider->radius = 1.0f;
			m_dynamicBodyPtrs[i] = new DynamicBody(CommonMesh::NewSphereMesh(this, 1.0f, 16, 16), pSphereCollider);
		}
		m_dynamicBodyPtrs[i]->setPosition(mSpherePos);

		if (i > 1)
		{
			m_dynamicBodyPtrs[i]->setActivityFlag(false);
		}
	}
	m_pPhysicsWorld = new PhysicsWorld(m_dynamicBodyPtrs);

	return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::HandleStop()
{
	for (auto& pDynamicBody : m_dynamicBodyPtrs)
	{
		SAFE_FREE(pDynamicBody);
	}

	for (auto& pHeightMap : m_heightMapPtrs)
	{
		SAFE_FREE(pHeightMap);
	}

	this->CommonApp::HandleStop();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::ReloadShaders()
{
	if (m_pCurrentHeightmap->ReloadShader() == false)
		this->SetWindowTitle("Reload Failed - see Visual Studio output window. Press F5 to try again.");
	else
		this->SetWindowTitle("Collision: Zoom / Rotate Q, A / O, P, Camera C, Drop Sphere R, N and T, Wire W");
}

void Application::HandleUpdate()
{
	m_pPhysicsWorld->tick();

	if (m_cameraState == CAMERA_ROTATE)
	{
		if (this->IsKeyPressed('Q') && m_cameraZ > 38.0f)
			m_cameraZ -= 1.0f;

		if (this->IsKeyPressed('A'))
			m_cameraZ += 1.0f;

		if (this->IsKeyPressed('O'))
			m_rotationAngle -= .01f;

		if (this->IsKeyPressed('P'))
			m_rotationAngle += .01f;
	}

	static bool dbC = false;

	if (IsKeyPressed('C'))
	{
		if (!dbC)
		{
			if (++m_cameraState == CAMERA_MAX)
				m_cameraState = CAMERA_TOP;

			dbC = true;
		}
	}
	else
	{
		dbC = false;
	}


	static bool dbW = false;
	if (this->IsKeyPressed('W'))
	{
		if (!dbW)
		{
			m_bWireframe = !m_bWireframe;
			this->SetRasterizerState(false, m_bWireframe);
			dbW = true;
		}
	}
	else
	{
		dbW = false;
	}

	if (this->IsKeyPressed(VK_F5))
	{
		if (!m_reload)
		{
			ReloadShaders();
			m_reload = true;
		}
	}
	else
	{
		m_reload = false;
	}

	static bool dbR = false;
	if (IsKeyPressed('R'))
	{
		if (dbR == false)
		{
			static int dx = 0;
			static int dy = 0;
			mSpherePos = XMFLOAT3((float)((rand() % 14 - 7.0f) - 0.5), 20.0f, (float)((rand() % 14 - 7.0f) - 0.5));
			mSphereVel = XMFLOAT3(0.0f, 0.2f, 0.0f);
			m_dynamicBodyPtrs[0]->setVelocity(mSphereVel);
			m_dynamicBodyPtrs[0]->setPosition(mSpherePos);
			dbR = true;
		}
	}
	else
	{
		dbR = false;
	}

	static bool dbT = false;
	if (IsKeyPressed('T'))
	{
		if (dbT == false)
		{
			static int dx = 0;
			static int dy = 0;
			mSpherePos = XMFLOAT3(mSpherePos.x, 20.0f, mSpherePos.z);
			m_dynamicBodyPtrs[0]->setVelocity(XMFLOAT3(0.0f, 0.2f, 0.0f));
			m_dynamicBodyPtrs[0]->setPosition(mSpherePos);

			dbT = true;
		}
	}
	else
	{
		dbT = false;
	}

	static int dx = 0;
	static int dy = 0;
	static int seg = 0;
	static bool dbN = false;

	if (IsKeyPressed('N'))
	{
		if (dbN == false)
		{
			if (++seg == 2)
			{
				seg = 0;
				if (++dx == 15)
				{
					if (++dy == 15) dy = 0;
					dx = 0;
				}
			}

			if (seg == 0)
				mSpherePos = XMFLOAT3(((dx - 7.0f) * 2) - 0.5f, 20.0f, ((dy - 7.0f) * 2) - 0.5f);
			else
				mSpherePos = XMFLOAT3(((dx - 7.0f) * 2) + 0.5f, 20.0f, ((dy - 7.0f) * 2) + 0.5f);

			mSphereVel = XMFLOAT3(0.0f, 0.2f, 0.0f);
			mGravityAcc = XMFLOAT3(0.0f, G_VALUE, 0.0f);
			mSphereCollided = true;
			m_dynamicBodyPtrs[0]->setPosition(mSpherePos);
			m_dynamicBodyPtrs[0]->setVelocity(XMFLOAT3(0, 0, 0));
			dbN = true;
		}
	}
	else
	{
		dbN = false;
	}

	// Update Sphere
	XMVECTOR vSColPos, vSColNorm;

	if (!mSphereCollided)
	{
		XMVECTOR vSPos = XMLoadFloat3(&mSpherePos);
		XMVECTOR vSVel = XMLoadFloat3(&mSphereVel);
		XMVECTOR vSAcc = XMLoadFloat3(&mGravityAcc);

		vSVel += vSAcc * m_deltaTime; // The new velocity gets passed through to the collision so it can base its predictions on our speed NEXT FRAME
		vSPos += vSVel * m_deltaTime; // Really important that we add LAST FRAME'S velocity as this was how fast the collision is expecting the ball to move


		XMStoreFloat3(&mSphereVel, vSVel);
		XMStoreFloat3(&mSpherePos, vSPos);

		mSphereSpeed = XMVectorGetX(XMVector3Length(vSVel));

		mSphereCollided = m_pCurrentHeightmap->RayCollision(vSPos, vSVel, mSphereSpeed, vSColPos, vSColNorm);

		if (mSphereCollided)
		{
			mSphereVel = XMFLOAT3(0.0f, 0.0f, 0.0f);
			XMStoreFloat3(&mSpherePos, vSColPos);
		}
	}

#pragma region Question 2 Debug Tools

	if (IsKeyPressed(' '))
	{ // slow the simulation down by sleeping when keyboard is pressed 
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		m_deltaTime = m_appBaseDT / 6;
	}
	else
	{
		m_deltaTime = 1.0f / 60.0f;//m_appBaseDT;

	}
	static bool dbU = false, dbI = false, dbD = false;
	static XMFLOAT3 float3Array[FACE_NORM_VERTICES_COUNT];
	static int indexInVecArray = 0;
	static int faceIndex = 0;

	if (IsKeyPressed('U'))
	{
		if (!dbU)
		{
			dbU = true;

			indexInVecArray = 3;
			m_pCurrentHeightmap->GetFaceVerticesByIndex(faceIndex, float3Array);
			mSpherePos = XMFLOAT3(float3Array[indexInVecArray].x, 20.0f, float3Array[indexInVecArray].z);
			mSphereVel = XMFLOAT3(0.0f, 0.2f, 0.0f);
			m_dynamicBodyPtrs[0]->setVelocity(mSphereVel);
			m_dynamicBodyPtrs[0]->setPosition(mSpherePos);

			if (faceIndex++ >= m_pCurrentHeightmap->GetFaceCount())
			{
				faceIndex = 0;
			}
		}
	}
	else
	{
		dbU = false;
	}

	if (IsKeyPressed('I'))
	{
		if (!dbI)
		{
			dbI = true;

			indexInVecArray = 3;
			m_pCurrentHeightmap->GetFaceVerticesByIndex(faceIndex, float3Array);
			mSpherePos = XMFLOAT3(float3Array[indexInVecArray].x, 20.0f, float3Array[indexInVecArray].z);
			mSphereVel = XMFLOAT3(0.0f, 0.2f, 0.0f);
			m_dynamicBodyPtrs[0]->setVelocity(mSphereVel);
			m_dynamicBodyPtrs[0]->setPosition(mSpherePos);

			if (--faceIndex < 0)
			{
				faceIndex = m_pCurrentHeightmap->GetFaceCount() - 1;
			}
		}
	}
	else
	{
		dbI = false;
	}

	if (IsKeyPressed('D'))
	{
		if (!dbD)
		{
			dbD = true;

			mSpherePos = XMFLOAT3(float3Array[indexInVecArray].x, 20.0f, float3Array[indexInVecArray].z);
			mSphereVel = XMFLOAT3(0.0f, 0.2f, 0.0f);
			mGravityAcc = XMFLOAT3(0.0f, G_VALUE, 0.0f);
			mSphereCollided = false;

			if (++indexInVecArray >= FACE_NORM_VERTICES_COUNT)
			{
				indexInVecArray = 0;
			}
		}

	}
	else
	{
		dbD = false;
	}

	if (IsKeyPressed('F'))
	{
		mSphereVel = XMFLOAT3(0.0f, 0.2f, 0.0f);

		m_dynamicBodyPtrs[0]->setVelocity(mSphereVel);
		m_dynamicBodyPtrs[1]->setVelocity(mSphereVel);

		XMFLOAT3 returnedVerts[FACE_NORM_VERTICES_COUNT]{ XMFLOAT3(0.0f,0.0f,0.0f) };

		m_pCurrentHeightmap->GetFaceVerticesByIndex(0, returnedVerts);
		m_dynamicBodyPtrs[0]->setPosition(XMFLOAT3(returnedVerts[3].x, 20.0f, returnedVerts[3].z));

		m_pCurrentHeightmap->GetFaceVerticesByIndex(m_pCurrentHeightmap->GetFaceCount() - 1, returnedVerts);
		m_dynamicBodyPtrs[1]->setPosition(XMFLOAT3(returnedVerts[3].x, 20.0f, returnedVerts[3].z));
		m_dynamicBodyPtrs[1]->setActivityFlag(true);


	}
	static bool dbUp = false;

	if (IsKeyPressed(VK_UP))
	{
		if (!dbUp)
		{
			DynamicBody* pBody = getNextAvailableBody();
			if (pBody != nullptr)
			{
				mSpherePos = XMFLOAT3((float)((rand() % 14 - 7.0f) - 0.5), 20.0f, (float)((rand() % 14 - 7.0f) - 0.5));
				mSphereVel = XMFLOAT3(0.0f, 0.2f, 0.0f);
				pBody->setVelocity(mSphereVel);
				pBody->setPosition(mSpherePos);
				pBody->setActivityFlag(true);
			}
			dbUp = true;
		}
	}
	else
	{
		dbUp = false;
	}

	static bool enableBase = false;
	static bool dbH = false;

	if (IsKeyPressed('H'))
	{
		if (!dbH)
		{
			dbH = true;
			if (enableBase)
			{
				const int hiddenCount = m_pCurrentHeightmap->EnableAll();
				dprintf("Hidden count: %d\n", hiddenCount);
			}
			else
			{
				const int hiddenCount = m_pCurrentHeightmap->DisableBelowLevel(Y_DISABLE_VALUE);
				dprintf("Hidden count: %d\n", hiddenCount);
			}
			enableBase = !enableBase;
		}
	}
	else
	{
		dbH = false;
	}

#pragma endregion


#pragma region Change Heightmap Controls

	static bool bIsTabDown = false;
	static int heightMapIndex = 0; // current heightmap
	if (IsKeyPressed(VK_TAB))
	{
		if (!bIsTabDown)
		{
			heightMapIndex + 1 < MAX_HEIGHTMAPS_COUNT ? ++heightMapIndex : heightMapIndex = 0;
			m_pCurrentHeightmap = m_heightMapPtrs[heightMapIndex];
			bIsTabDown = true;
		}
	}
	else
	{
		bIsTabDown = false;
	}

#pragma endregion

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::HandleRender()
{
	XMVECTOR vCamera, vLookat;
	XMVECTOR vUpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX matProj, matView;

	switch (m_cameraState)
	{
	case CAMERA_TOP:
		vCamera = XMVectorSet(0.0f, 100.0f, 0.1f, 0.0f);
		vLookat = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		matView = XMMatrixLookAtLH(vCamera, vLookat, vUpVector);
		matProj = XMMatrixOrthographicLH(64, 36, 1.5f, 5000.0f);
		break;
	case CAMERA_ROTATE:
		vCamera = XMVectorSet(sin(m_rotationAngle)*m_cameraZ, (m_cameraZ*m_cameraZ) / 50, cos(m_rotationAngle)*m_cameraZ, 0.0f);
		vLookat = XMVectorSet(0.0f, 10.0f, 0.0f, 0.0f);
		matView = XMMatrixLookAtLH(vCamera, vLookat, vUpVector);
		matProj = XMMatrixPerspectiveFovLH(float(D3DX_PI / 7), 2, 1.5f, 5000.0f);
		break;
	}

	this->EnableDirectionalLight(1, XMFLOAT3(-1.f, -1.f, -1.f), XMFLOAT3(0.55f, 0.55f, 0.65f));
	this->EnableDirectionalLight(2, XMFLOAT3(1.f, -1.f, 1.f), XMFLOAT3(0.15f, 0.15f, 0.15f));

	this->SetViewMatrix(matView);
	this->SetProjectionMatrix(matProj);

	this->Clear(XMFLOAT4(0.05f, 0.05f, 0.5f, 1.f));

	this->SetWorldMatrix(m_dynamicBodyPtrs[0]->getWorldMatrix());
	SetDepthStencilState(false, true);
	m_pCurrentHeightmap->Draw(m_frameCount);

#pragma region DynamicBodyTesting

	for (auto& pDynamicBody : m_dynamicBodyPtrs)
	{
		if (!pDynamicBody->isActive())
		{
			continue;
		}

		SetWorldMatrix(pDynamicBody->getWorldMatrix());
		//SetDepthStencilState(false, false);
		if (pDynamicBody->getCommonMesh())
		{
			//pDynamicBody->getCommonMesh()->Draw();
		}

		SetWorldMatrix(pDynamicBody->getWorldMatrix());
		SetDepthStencilState(true, true);
		if (pDynamicBody->getCommonMesh())
		{
			pDynamicBody->getCommonMesh()->Draw();
		}
	}

#pragma endregion

	m_frameCount++;
}

DynamicBody* Application::getNextAvailableBody()
{
	auto findResult = std::find_if(std::begin(m_dynamicBodyPtrs), std::end(m_dynamicBodyPtrs), [=](DynamicBody* pDynamicBody) -> bool
	{
		return !pDynamicBody->isActive();
	});

	if (findResult == std::end(m_dynamicBodyPtrs))
	{
		return nullptr;
	}
	return *findResult;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application application;

	Run(&application);

	return 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
