#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Player.h"

class CScene
{
public:
	CScene(CPlayer *pPlayer);
	virtual ~CScene();
protected:
	CPlayer* m_pPlayer = NULL;
public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
};

class CTitleScene : public CScene {
public:
	CTitleScene(CPlayer* pPlayer);
	virtual void BuildObjects() override;
	virtual void ReleaseObjects() override;
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera) override;
	virtual void Animate(float fElapsedTime) override;
	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
private:
	CTitleObject* m_pTitleObjects;
};

class CMenuScene : public CScene {
public:
	CMenuScene(CPlayer* pPlayer);
	virtual void BuildObjects() override;
	virtual void ReleaseObjects() override;
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera) override;
	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
private:
	static const int m_nCubeObjects = 5;
	CCubeObject* m_pCubeObjects[m_nCubeObjects];
};