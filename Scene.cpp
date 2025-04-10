#include "stdafx.h"
#include "Scene.h"
#include "GraphicsPipeline.h"

CScene::CScene(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

CScene::~CScene()
{
}

void CScene::BuildObjects()
{

	CCubeMesh* pCubeMesh = new CCubeMesh(1.0f, 0.4f, 1.0f);

	m_pTitleObjects = new CCubeObject();
	m_pTitleObjects->SetMesh(pCubeMesh);
	m_pTitleObjects->SetColor(RGB(255, 0, 0));
	m_pTitleObjects->SetPosition(0.0f, 0.0f, 0.0f);
	m_pTitleObjects->UpdateBoundingBox();

	pCubeMesh = new CCubeMesh(0.5f, 0.2f, 1.0f);

	for (int i = 0; i < m_nCubeObjects; i++)
	{
		m_pCubeObjects[i] = new CCubeObject();
		m_pCubeObjects[i]->SetMesh(pCubeMesh);
		m_pCubeObjects[i]->SetColor(RGB(255, 0, 0));
		m_pCubeObjects[i]->SetPosition(-0.5f, -0.5f + 0.3f * i, 0.0f);
		m_pCubeObjects[i]->UpdateBoundingBox();
	}

#ifdef _WITH_DRAW_AXIS
	m_pWorldAxis = new CGameObject();
	CAxisMesh* pAxisMesh = new CAxisMesh(0.5f, 0.5f, 0.5f);
	m_pWorldAxis->SetMesh(pAxisMesh);
#endif
}

void CScene::ReleaseObjects()
{
	if (m_pTitleObjects) delete m_pTitleObjects;
	for (int i = 0; i < m_nCubeObjects; i++) {
		if (m_pCubeObjects[i]) delete m_pCubeObjects[i];
	}

#ifdef _WITH_DRAW_AXIS
	if (m_pWorldAxis) delete m_pWorldAxis;
#endif
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		CCamera* pCamera = m_pPlayer->GetCamera();  //여기서 확보
		CGameObject* pPickedObject = PickObjectPointedByCursorOrthographic(x, y, pCamera);

		if (pPickedObject) {
			Scene_number = 2;  //클릭된 오브젝트가 있으면 씬 전환
		}
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

CGameObject* CScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
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
CGameObject* CScene::PickObjectPointedByCursorOrthographic(int xClient, int yClient, CCamera* pCamera)
{
	float viewWidth = (float)pCamera->m_Viewport.m_nWidth;
	float viewHeight = (float)pCamera->m_Viewport.m_nHeight;

	float px = ((2.0f * xClient) / viewWidth - 1.0f);
	float py = (1.0f - (2.0f * yClient) / viewHeight);

	XMFLOAT3 xmf3RayOrigin = XMFLOAT3(px * viewWidth * 0.5f, py * viewHeight * 0.5f, 0.0f);
	XMFLOAT3 xmf3RayDirection = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMVECTOR xmvRayOrigin = XMVector3TransformCoord(XMLoadFloat3(&xmf3RayOrigin), XMLoadFloat4x4(&pCamera->m_xmf4x4View));
	XMVECTOR xmvRayDirection = XMVector3TransformNormal(XMLoadFloat3(&xmf3RayDirection), XMLoadFloat4x4(&pCamera->m_xmf4x4View));
	xmvRayDirection = XMVector3Normalize(xmvRayDirection);

	float fNearestHitDistance = FLT_MAX;
	CGameObject* pNearestObject = NULL;

	for (int i = 0; i < m_nCubeObjects; i++)
	{
		int hit = m_pCubeObjects[i]->PickObjectByRayIntersection(xmvRayOrigin, XMMatrixIdentity(), &fNearestHitDistance);
		if (hit > 0) {
			pNearestObject = m_pCubeObjects[i];
		}
	}

	return pNearestObject;
}




void CScene::Animate(float fElapsedTime)
{
}

void CScene::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{

	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

	switch (Scene_number)
	{
	case 0:
		CGraphicsPipeline::SetViewOrthographicProjectTransform(&pCamera->m_xmf4x4ViewOrthographicProject);
		m_pTitleObjects->Render(hDCFrameBuffer, pCamera);
		TextOut(hDCFrameBuffer, 220, 230, _T("3D 게임프로그래밍 1, 이현석"), _tcslen(_T("3D 게임프로그래밍 1, 이현석")));
		break;
	case 1:
		CGraphicsPipeline::SetViewOrthographicProjectTransform(&pCamera->m_xmf4x4ViewOrthographicProject);

		for (int i = 0; i < m_nCubeObjects; i++) {
			m_pCubeObjects[i]->Render(hDCFrameBuffer, pCamera);
		}
		TextOut(hDCFrameBuffer, 135, 60, _T("Tutorial"), _tcslen(_T("Tutorial")));
		TextOut(hDCFrameBuffer, 135, 135, _T("Level-1"), _tcslen(_T("Level-1")));
		TextOut(hDCFrameBuffer, 135, 210, _T("Level-2"), _tcslen(_T("Level-2")));
		TextOut(hDCFrameBuffer, 140, 280, _T("Start"), _tcslen(_T("Start")));
		TextOut(hDCFrameBuffer, 140, 350, _T("End"), _tcslen(_T("End")));
		break;
	default:
		CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);
		if (m_pPlayer) m_pPlayer->Render(hDCFrameBuffer, pCamera);
		break;
	}

//UI
#ifdef _WITH_DRAW_AXIS
	CGraphicsPipeline::SetViewOrthographicProjectTransform(&pCamera->m_xmf4x4ViewOrthographicProject);
	m_pWorldAxis->SetRotationTransform(&m_pPlayer->m_xmf4x4World);
	m_pWorldAxis->Render(hDCFrameBuffer, pCamera);
#endif
}
