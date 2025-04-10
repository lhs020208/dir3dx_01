#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Player.h"

class CScene
{
public:
	CScene(CPlayer *pPlayer);
	virtual ~CScene();
	int GetSceneNumber() const { return Scene_number; }
private:
	int							Scene_number = 0;

	CCubeObject*				m_pTitleObjects;

	static const int			m_nCubeObjects = 5;
	CCubeObject*				m_pCubeObjects[m_nCubeObjects];

	CPlayer*					m_pPlayer = NULL;

#ifdef _WITH_DRAW_AXIS
	CGameObject*				m_pWorldAxis = NULL;
#endif

public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
	CGameObject* CScene::PickObjectPointedByCursorOrthographic(int xClient, int yClient, CCamera* pCamera);
};

