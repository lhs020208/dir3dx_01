#include "stdafx.h"
#include "Scene.h"
#include "GraphicsPipeline.h"
#include "GameFramework.h"

CScene::CScene(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}
CScene::~CScene()
{

}

void CScene::BuildObjects()
{

}

void CScene::ReleaseObjects()
{

}
void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	}
}
void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			break;
		}
		case 'A':
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
void CScene::Animate(float fElapsedTime)
{
}
void CScene::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
		CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);
		if (m_pPlayer) m_pPlayer->Render(hDCFrameBuffer, pCamera);
}
//타이틀 Scene////////////////////////////////////////////////////////////////////////////////////////////////
CTitleScene::CTitleScene(CPlayer* pPlayer) : CScene(pPlayer) {}
void CTitleScene::BuildObjects() {
	CTitleMesh* cTitleMesh = new CTitleMesh("Title.obj");

	m_pTitleObjects = new CTitleObject();
	m_pTitleObjects->SetMesh(cTitleMesh);
	m_pTitleObjects->SetColor(RGB(255, 0, 0));
	m_pTitleObjects->SetPosition(0.0f, 0.0f, 1.0f);
	m_pTitleObjects->UpdateBoundingBox();
}
void CTitleScene::ReleaseObjects()
{
	if (m_pTitleObjects) delete m_pTitleObjects;
}
void CTitleScene::Render(HDC hDCFrameBuffer, CCamera* pCamera) {
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);
	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);

	if (m_pTitleObjects) m_pTitleObjects->Render(hDCFrameBuffer, pCamera);
}
void CTitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		CCamera* pCamera = m_pPlayer->GetCamera();  //여기서 확보
		CGameObject* pPickedObject = PickObjectPointedByCursor(x, y, pCamera);

		if (pPickedObject) {
			if (!m_pTitleObjects->IsBlowingUp()) {
				m_pTitleObjects->PrepareExplosion();
			}
		}

	}
}
void CTitleScene::Animate(float fElapsedTime)
{
	if (m_pTitleObjects) m_pTitleObjects->Animate(fElapsedTime);
}
CGameObject* CTitleScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{

	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)pCamera->m_Viewport.m_nWidth) - 1) / pCamera->m_xmf4x4PerspectiveProject._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)pCamera->m_Viewport.m_nHeight) - 1) / pCamera->m_xmf4x4PerspectiveProject._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->m_xmf4x4View);

	float fNearestHitDistance = FLT_MAX;
	CGameObject* pNearestObject = NULL;
	if (m_pTitleObjects)
	{
		int hit = m_pTitleObjects->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fNearestHitDistance);
		if (hit > 0)
		{
			pNearestObject = m_pTitleObjects;
		}
	}
	return(pNearestObject);
}
//메뉴 Scene////////////////////////////////////////////////////////////////////////////////////////////////
CMenuScene::CMenuScene(CPlayer* pPlayer) : CScene(pPlayer) {}
void CMenuScene::BuildObjects() {
	for (int i = 0; i < m_nCubeObjects; i++)
	{
		m_pCubeObjects[i] = nullptr;
		try {
			m_pCubeObjects[i] = new CMenuObject();
			switch (i)
			{
			case 4:
			{
				CTutorialMesh* pCubeMesh = new CTutorialMesh("Tutorial.obj");
				m_pCubeObjects[i]->SetMesh(pCubeMesh);
				break;
			}
			case 3:
			{
				CLevel_1Mesh* pCubeMesh = new CLevel_1Mesh("Level-1.obj");
				m_pCubeObjects[i]->SetMesh(pCubeMesh);
				break;
			}
			case 2:
			{
				CLevel_2Mesh* pCubeMesh = new CLevel_2Mesh("Level-2.obj");
				m_pCubeObjects[i]->SetMesh(pCubeMesh);
				break;
			}
			case 1:
			{
				CStartMesh* pCubeMesh = new CStartMesh("Start.obj");
				m_pCubeObjects[i]->SetMesh(pCubeMesh);
				break;
			}
			case 0:
			{
				CEndMesh* pCubeMesh = new CEndMesh("End.obj");
				m_pCubeObjects[i]->SetMesh(pCubeMesh);
				break;
			}
			}
		}
		catch (std::bad_alloc& ba) {
			wchar_t msg[64];
			swprintf_s(msg, 64, L"[ERROR] %s\n", ba.what());
			OutputDebugString(msg);
		}
		m_pCubeObjects[i]->SetColor(RGB(0, 0, 0));
		m_pCubeObjects[i]->SetPosition(-0.8f, -0.58f + 0.35f * i, 1.0f);
		m_pCubeObjects[i]->UpdateBoundingBox();

		XMFLOAT3 cameraPos = m_pPlayer->GetCamera()->GetPosition();
		XMFLOAT3 upVector = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_pCubeObjects[i]->LookAt(cameraPos, upVector);

		XMMATRIX rotationY = XMMatrixRotationY(XMConvertToRadians(180.0f));
		XMMATRIX world = XMLoadFloat4x4(&m_pCubeObjects[i]->m_xmf4x4World);
		world = XMMatrixMultiply(rotationY, world);
		XMStoreFloat4x4(&m_pCubeObjects[i]->m_xmf4x4World, world);
	}

}
void CMenuScene::ReleaseObjects()
{
	for (int i = 0; i < m_nCubeObjects; i++) {
		if (m_pCubeObjects[i]) {
			delete m_pCubeObjects[i];
			m_pCubeObjects[i] = nullptr;
		}
	}
}
void CMenuScene::Render(HDC hDCFrameBuffer, CCamera* pCamera) {
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);
	for (int i = 0; i < m_nCubeObjects; i++) {
		if (m_pCubeObjects[i]) m_pCubeObjects[i]->Render(hDCFrameBuffer, pCamera);
	}
}
void CMenuScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	extern CGameFramework* g_pFramework;
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		CCamera* pCamera = m_pPlayer->GetCamera();  //여기서 확보
		CGameObject* pPickedObject = PickObjectPointedByCursor(x, y, pCamera);

		if (pPickedObject)
		{
			for (int i = 0; i < m_nCubeObjects; i++)
			{
				if (pPickedObject == m_pCubeObjects[i])
				{
					// 인덱스별로 분기
					switch (i)
					{
					case 0:
						OutputDebugString(L"Cube 0 클릭됨\n"); // End
						SendMessage(hWnd, WM_CLOSE, 0, 0);
						break;
					case 1:
						OutputDebugString(L"Cube 1 클릭됨\n"); //Start
						g_pFramework->ChangeScene(2);
						break;
					case 2:
						OutputDebugString(L"Cube 2 클릭됨\n"); //L2
						g_pFramework->ChangeScene(3);
						break;
					case 3:
						OutputDebugString(L"Cube 3 클릭됨\n"); //L1
						g_pFramework->ChangeScene(2);
						break;
					case 4:
						OutputDebugString(L"Cube 4 클릭됨\n"); //Tutorial
						break;
					}
					break;
				}
			}
		}

	}
}
CGameObject* CMenuScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{

	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)pCamera->m_Viewport.m_nWidth) - 1) / pCamera->m_xmf4x4PerspectiveProject._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)pCamera->m_Viewport.m_nHeight) - 1) / pCamera->m_xmf4x4PerspectiveProject._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->m_xmf4x4View);

	float fNearestHitDistance = FLT_MAX;
	CGameObject* pNearestObject = NULL;
	for (int i = 0; i < m_nCubeObjects; i++)
	{
		int hit = m_pCubeObjects[i]->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fNearestHitDistance);
		if (hit > 0) {
			pNearestObject = m_pCubeObjects[i];
		}
	}
	return(pNearestObject);
}
//롤러코스터 Scene////////////////////////////////////////////////////////////////////////////////////////////////
CRollerCoasterScene::CRollerCoasterScene(CPlayer* pPlayer) : CScene(pPlayer) {}
void CRollerCoasterScene::BuildObjects() 
{
	m_ppObjects = new CGameObject * [m_nObjects];
	CTitleMesh* cTitleMesh = new CTitleMesh("Title.obj");

	for (int i = 0; i < m_nObjects; i++) {
		float t = (float)i / 1000;
		t = i * 0.01f;
		CRCCubeMesh* pCubeMesh = new CRCCubeMesh(t);
		m_ppObjects[i] = new CRollerCoasterObject();
		m_ppObjects[i]->SetMesh(pCubeMesh);
		m_ppObjects[i]->SetColor(RGB(0, 0, 255));
		m_ppObjects[i]->SetPosition(0.0f, 0.0f, 0.0f);
		m_ppObjects[i]->UpdateBoundingBox();
	}
}
void CRollerCoasterScene::ReleaseObjects()
{
	for (int i = 0; i < m_nObjects; i++) {
		if (m_ppObjects[i]) delete m_ppObjects[i];
	}
	delete[] m_ppObjects;
	m_ppObjects = NULL;
}
void CRollerCoasterScene::Render(HDC hDCFrameBuffer, CCamera* pCamera) {
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);
	if (m_pPlayer) m_pPlayer->Render(hDCFrameBuffer, pCamera);
	for (int i = 0; i < m_nObjects; i++) {
		m_ppObjects[i]->Render(hDCFrameBuffer, pCamera);
	}
}

void CRollerCoasterScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	extern CGameFramework* g_pFramework;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'o':
		case 'O':
			m_pPlayer->overview = true;
			break;
		case 'l':
		case 'L':
			m_pPlayer->overview = false;
			XMFLOAT3 start_pos = RollerCoasterPos(timer);
			m_pPlayer->reset();
			m_pPlayer->SetPosition(start_pos.x, start_pos.y, start_pos.z);
			m_pPlayer->SetCameraOffset(XMFLOAT3(0.0f, 0.1f, -1.5f));
			break;
		case 'n':
		case 'N':
			g_pFramework->ChangeScene(3);
			break;
		case 'm':
		case 'M':
			XMFLOAT3 playerPos = m_pPlayer->GetPosition();
			XMFLOAT3 objectPos = m_ppObjects[0]->GetPosition();

			wchar_t msg[128];
			swprintf_s(msg, 128, L"[DEBUG] Player Pos: (%.2f, %.2f, %.2f)\n", playerPos.x, playerPos.y, playerPos.z);
			OutputDebugString(msg);

			swprintf_s(msg, 128, L"[DEBUG] Object[0] Pos: (%.2f, %.2f, %.2f)\n", objectPos.x, objectPos.y, objectPos.z);
			OutputDebugString(msg);
			break;
		case VK_ESCAPE:
			g_pFramework->ChangeScene(1);
			break;
		case VK_SPACE:
			move = true;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CRollerCoasterScene::Animate(float fElapsedTime)
{
	extern CGameFramework* g_pFramework;
	if (move) {
		m_pPlayer->SetPosition(RollerCoasterPos(timer).x, RollerCoasterPos(timer).y, RollerCoasterPos(timer).z);
		timer += speed;
		if (timer >= 0.71) {
			speed = 0.01;
		}
		if (timer >= 1.42) {
			speed = 0.005;
		}
	}
	if (timer >= 15.14) {
		move = false;
		timer = 0.0;
		speed = 0.002;
		g_pFramework->ChangeScene(3);
	}
}
//탱크 Scene////////////////////////////////////////////////////////////////////////////////////////////////
CTankScene::CTankScene(CPlayer* pPlayer) : CScene(pPlayer) {}
void CTankScene::BuildObjects()
{
	for (int i = 0; i < m_nTanks; i++)
	{
		m_pTank[i] = nullptr;
		m_pTank[i] = new CTankObject();
		CTankMesh* pTankMesh = new CTankMesh("Tank.obj");
		m_pTank[i]->SetMesh(pTankMesh);
		m_pTank[i]->SetColor(RGB(0, 0, 0));
		m_pTank[i]->SetPosition(-2.0f + 0.5f * i, 0.0f, 1.0f);
		m_pTank[i]->UpdateBoundingBox();
	}
	for (int i = 0; i < m_nCubeObjects; i++) {
		CCubeMesh* pCubeMesh = new CCubeMesh(0.1f, 0.1f, 0.1f);
		m_pCubeObjects[i] = new CCubeObject();
		m_pCubeObjects[i]->SetMesh(pCubeMesh);
		m_pCubeObjects[i]->SetColor(RGB(0, 0, 255));
		m_pCubeObjects[i]->SetPosition(-2.0f + 0.5f * i, 0.0f, 0.0f);
		m_pCubeObjects[i]->UpdateBoundingBox();
	}
}
void CTankScene::ReleaseObjects()
{

}
void CTankScene::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);
	if (m_pPlayer) m_pPlayer->Render(hDCFrameBuffer, pCamera);
	for (int i = 0; i < m_nTanks; i++) {
		m_pTank[i]->Render(hDCFrameBuffer, pCamera);
	}
	for (int i = 0; i < m_nCubeObjects; i++) {
		m_pCubeObjects[i]->Render(hDCFrameBuffer, pCamera);
	}
}

void CTankScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	extern CGameFramework* g_pFramework;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'm':
		case 'M':
			XMFLOAT3 playerPos = m_pPlayer->GetPosition();
			XMFLOAT3 objectPos = m_ppObjects[0]->GetPosition();

			wchar_t msg[128];
			swprintf_s(msg, 128, L"[DEBUG] Player Pos: (%.2f, %.2f, %.2f)\n", playerPos.x, playerPos.y, playerPos.z);
			OutputDebugString(msg);

			swprintf_s(msg, 128, L"[DEBUG] Object[0] Pos: (%.2f, %.2f, %.2f)\n", objectPos.x, objectPos.y, objectPos.z);
			OutputDebugString(msg);
			break;
		case VK_ESCAPE:
			g_pFramework->ChangeScene(1);
			break;
		case VK_SPACE:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CTankScene::Animate(float fElapsedTime)
{
	for (int i = 0; i < 10; i++) {
		m_pTank[i]->Animate(fElapsedTime);
	}
	m_pPlayer->Animate(fElapsedTime);
}