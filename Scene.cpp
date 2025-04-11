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
			CLevel_1Mesh * pCubeMesh = new CLevel_1Mesh("Level-1.obj");
			m_pCubeObjects[i]->SetMesh(pCubeMesh);
			break;
		}
		case 2:
		{
			CLevel_2Mesh * pCubeMesh = new CLevel_2Mesh("Level-2.obj");
			m_pCubeObjects[i]->SetMesh(pCubeMesh);
			break;
		}
		case 1:
		{
			CStartMesh * pCubeMesh = new CStartMesh("Start.obj");
			m_pCubeObjects[i]->SetMesh(pCubeMesh);
			break;
		}
		case 0:
		{
			CEndMesh * pCubeMesh = new CEndMesh("End.obj");
			m_pCubeObjects[i]->SetMesh(pCubeMesh);
			break;
		}
		}
		m_pCubeObjects[i]->SetColor(RGB(0, 0, 0));
		m_pCubeObjects[i]->SetPosition(-0.8f, -0.58f + 0.35f * i, 1.0f);
		m_pCubeObjects[i]->UpdateBoundingBox();
	}
}
void CMenuScene::ReleaseObjects()
{
	for (int i = 0; i < m_nCubeObjects; i++) {
		if (m_pCubeObjects[i]) delete m_pCubeObjects[i];
	}
}
void CMenuScene::Render(HDC hDCFrameBuffer, CCamera* pCamera) {
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

		//CGraphicsPipeline::SetViewOrthographicProjectTransform(&pCamera->m_xmf4x4ViewOrthographicProject);
		CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);
		for (int i = 0; i < m_nCubeObjects; i++) {
			m_pCubeObjects[i]->Render(hDCFrameBuffer, pCamera);
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

		if (pPickedObject) {
			g_pFramework->ChangeScene(2);
			OutputDebugString(L"디버그 메시지 출력\n");
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
void CRollerCoasterScene::BuildObjects() {
	
}
void CRollerCoasterScene::ReleaseObjects()
{
	
}
void CRollerCoasterScene::Render(HDC hDCFrameBuffer, CCamera* pCamera) {
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);
	if (m_pPlayer) m_pPlayer->Render(hDCFrameBuffer, pCamera);
}