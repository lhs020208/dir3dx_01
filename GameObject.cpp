#include "stdafx.h"
#include "GameObject.h"
#include "GraphicsPipeline.h"
#include "GameFramework.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

XMVECTOR RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}


CGameObject::~CGameObject(void)
{
	if (m_pMesh) m_pMesh->Release();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CGameObject::SetPosition(XMFLOAT3& xmf3Position)
{
	m_xmf4x4World._41 = xmf3Position.x;
	m_xmf4x4World._42 = xmf3Position.y;
	m_xmf4x4World._43 = xmf3Position.z;
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	XMFLOAT3 xmf3LookAt(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	xmf3LookAt = Vector3::Normalize(xmf3LookAt);
	return(xmf3LookAt);
}

XMFLOAT3 CGameObject::GetUp()
{
	XMFLOAT3 xmf3Up(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	xmf3Up = Vector3::Normalize(xmf3Up);
	return(xmf3Up);
}

XMFLOAT3 CGameObject::GetRight()
{
	XMFLOAT3 xmf3Right(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	xmf3Right = Vector3::Normalize(xmf3Right);
	return(xmf3Right);
}

void CGameObject::SetRotationTransform(XMFLOAT4X4* pmxf4x4Transform)
{
	m_xmf4x4World._11 = pmxf4x4Transform->_11; m_xmf4x4World._12 = pmxf4x4Transform->_12; m_xmf4x4World._13 = pmxf4x4Transform->_13;
	m_xmf4x4World._21 = pmxf4x4Transform->_21; m_xmf4x4World._22 = pmxf4x4Transform->_22; m_xmf4x4World._23 = pmxf4x4Transform->_23;
	m_xmf4x4World._31 = pmxf4x4Transform->_31; m_xmf4x4World._32 = pmxf4x4Transform->_32; m_xmf4x4World._33 = pmxf4x4Transform->_33;
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Right, fDistance));

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Up, fDistance));

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3LookAt = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3LookAt, fDistance));

	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Rotate(XMFLOAT3& xmf3RotationAxis, float fAngle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	SetPosition(m_xmf4x4World._41 + vDirection.x * fSpeed, m_xmf4x4World._42 + vDirection.y * fSpeed, m_xmf4x4World._43 + vDirection.z * fSpeed);
}

void CGameObject::LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookToLH(GetPosition(), xmf3LookTo, xmf3Up);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, xmf3Up);
	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void CGameObject::UpdateBoundingBox()
{
	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
	}
}

void CGameObject::Animate(float fElapsedTime)
{
	if (m_fRotationSpeed != 0.0f) Rotate(m_xmf3RotationAxis, m_fRotationSpeed * fElapsedTime);
	if (m_fMovingSpeed != 0.0f) Move(m_xmf3MovingDirection, m_fMovingSpeed * fElapsedTime);

	UpdateBoundingBox();
}

void CGameObject::Render(HDC hDCFrameBuffer, XMFLOAT4X4* pxmf4x4World, CMesh* pMesh)
{
	if (pMesh)
	{
		CGraphicsPipeline::SetWorldTransform(pxmf4x4World);

		HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
		HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
		pMesh->Render(hDCFrameBuffer);
		::SelectObject(hDCFrameBuffer, hOldPen);
		::DeleteObject(hPen);
	}
}

void CGameObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	if (pCamera->IsInFrustum(m_xmOOBB)) CGameObject::Render(hDCFrameBuffer, &m_xmf4x4World, m_pMesh);
}

void CGameObject::GenerateRayForPicking(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection)
{
	XMMATRIX xmmtxToModel = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4World) * xmmtxView);

	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	xmvPickRayOrigin = XMVector3TransformCoord(XMLoadFloat3(&xmf3CameraOrigin), xmmtxToModel);
	xmvPickRayDirection = XMVector3TransformCoord(xmvPickPosition, xmmtxToModel);
	xmvPickRayDirection = XMVector3Normalize(xmvPickRayDirection - xmvPickRayOrigin);
}

int CGameObject::PickObjectByRayIntersection(XMVECTOR& xmvPickPosition, XMMATRIX& xmmtxView, float* pfHitDistance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMVECTOR xmvPickRayOrigin, xmvPickRayDirection;
		GenerateRayForPicking(xmvPickPosition, xmmtxView, xmvPickRayOrigin, xmvPickRayDirection);
		nIntersected = m_pMesh->CheckRayIntersection(xmvPickRayOrigin, xmvPickRayDirection, pfHitDistance);
	}
	return(nIntersected);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

CCubeObject::CCubeObject()
{
}
CCubeObject::~CCubeObject()
{
}
void CCubeObject::Animate(float fElapsedTime)
{
}
void CCubeObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGameObject::Render(hDCFrameBuffer, pCamera);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

CMenuObject::CMenuObject()
{
}
CMenuObject::~CMenuObject()
{
}
void CMenuObject::Animate(float fElapsedTime)
{
}
void CMenuObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGameObject::Render(hDCFrameBuffer, pCamera);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

CRollerCoasterObject::CRollerCoasterObject()
{
}
CRollerCoasterObject::~CRollerCoasterObject()
{
}
void CRollerCoasterObject::Animate(float fElapsedTime)
{
}
void CRollerCoasterObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGameObject::Render(hDCFrameBuffer, pCamera);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CAxisObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGraphicsPipeline::SetWorldTransform(&m_xmf4x4World);

	m_pMesh->Render(hDCFrameBuffer);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
CTitleObject::CTitleObject()
{
}

CTitleObject::~CTitleObject()
{
}

void CTitleObject::Animate(float fElapsedTime)
{
	extern CGameFramework* g_pFramework;
	static bool bSceneChanged = false;
	if (m_bBlowingUp)
	{
		if (!bSceneChanged && m_fElapsedTimes >= (m_fDuration - 0.3f)) {
			bSceneChanged = true;
			if (g_pFramework) g_pFramework->RequestSceneChange(1);
		}

		m_fElapsedTimes += fElapsedTime;
		if (m_fElapsedTimes >= m_fDuration)
		{
			m_bBlowingUp = false;
			return;
		}

		for (int i = 0; i < EXPLOSION_DEBRISES; i++) {
			XMFLOAT3 direction = m_pxmf3SphereVectors[i];
			XMFLOAT3 position = Vector3::Add(GetPosition(), Vector3::ScalarProduct(direction, m_fElapsedTimes * m_fExplosionSpeed));
			XMFLOAT4X4 world = Matrix4x4::RotationAxis(direction, m_fElapsedTimes * XMConvertToRadians(m_fExplosionRotation));
			world._41 = position.x; world._42 = position.y; world._43 = position.z;
			m_pxmf4x4Transforms[i] = world;
		}
	}
	else
	{
		Rotate(0.0f * fElapsedTime, 10.0f * fElapsedTime, 0.0f * fElapsedTime);
		UpdateBoundingBox();
	}
}

void CTitleObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	if (m_bBlowingUp)
	{
		for (int i = 0; i < EXPLOSION_DEBRISES; i++) {
			if (pCamera->IsInFrustum(m_xmOOBB)) {
				CGameObject::Render(hDCFrameBuffer, &m_pxmf4x4Transforms[i], m_pExplosionMesh);
			}
		}
	}
	else
	{
		CGameObject::Render(hDCFrameBuffer, pCamera);
	}
}

void CTitleObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CTitleObject::Rotate(XMFLOAT3& xmf3RotationAxis, float fAngle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

CMesh* CTitleObject::m_pExplosionMesh = NULL;
void CTitleObject::PrepareExplosion()
{
	m_bBlowingUp = true;
	m_fElapsedTimes = 0.0f;

	if (!m_pExplosionMesh) m_pExplosionMesh = new CCubeMesh(0.05f, 0.05f, 0.05f);

	for (int i = 0; i < EXPLOSION_DEBRISES; i++) {
		XMStoreFloat3(&m_pxmf3SphereVectors[i], RandomUnitVectorOnSphere());
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

CTankObject::CTankObject()
{
}
CTankObject::~CTankObject()
{
}
void CTankObject::Animate(float fElapsedTime)
{
	if (IsExist()) {
		if (m_bBlowingUp)
		{
			m_fElapsedTimes += fElapsedTime;
			if (m_fElapsedTimes >= m_fDuration)
			{
				SetExist(false);
				return;
			}

			for (int i = 0; i < EXPLOSION_DEBRISES; i++) {
				XMFLOAT3 direction = m_pxmf3SphereVectors[i];
				XMFLOAT3 position = Vector3::Add(GetPosition(), Vector3::ScalarProduct(direction, m_fElapsedTimes * m_fExplosionSpeed));
				XMFLOAT4X4 world = Matrix4x4::RotationAxis(direction, m_fElapsedTimes * XMConvertToRadians(m_fExplosionRotation));
				world._41 = position.x; world._42 = position.y; world._43 = position.z;
				m_pxmf4x4Transforms[i] = world;
			}
		}
		else
		{
			int forward_Step = 100;
			if (timer < forward_Step)
				SetPosition(Vector3::Add(GetPosition(), Vector3::ScalarProduct(GetLook(), fElapsedTime * 0.5f, false)));
			if (timer >= forward_Step && timer < forward_Step + 90)
				Rotate(0.0f, 2.0f, 0.0f);
			if (timer >= forward_Step + 90 && timer < 2 * forward_Step + 90)
				SetPosition(Vector3::Add(GetPosition(), Vector3::ScalarProduct(GetLook(), fElapsedTime * 0.5f, false)));
			if (timer >= 2 * forward_Step + 90 && timer < 2 * forward_Step + 180)
				Rotate(0.0f, 2.0f, 0.0f);

			timer++;
			if (timer == 2 * forward_Step + 180) timer = 0;

			if (shot) {
				bullet_timer++;
				bullet->SetPosition(Vector3::Add(bullet->GetPosition(), Vector3::ScalarProduct(bullet->GetLook(), fElapsedTime * 2.0f, false)));
				if (bullet_timer >= 100) {
					bullet_timer = 0;
					SwitchShot();
				}
			}
			else {
				using namespace std;
				default_random_engine dre{ random_device{}() };
				uniform_int_distribution<int> uid{ 0,500 };
				if (uid(dre) == 0) {
					bullet->SetPosition(GetPosition().x, GetPosition().y, GetPosition().z);

					XMFLOAT3 right = GetRight();
					XMFLOAT3 up = GetUp();
					XMFLOAT3 look = GetLook();

					XMFLOAT4X4 rotationMatrix;
					rotationMatrix._11 = right.x; rotationMatrix._12 = right.y; rotationMatrix._13 = right.z; rotationMatrix._14 = 0.0f;
					rotationMatrix._21 = up.x;    rotationMatrix._22 = up.y;    rotationMatrix._23 = up.z;    rotationMatrix._24 = 0.0f;
					rotationMatrix._31 = look.x;  rotationMatrix._32 = look.y;  rotationMatrix._33 = look.z;  rotationMatrix._34 = 0.0f;
					rotationMatrix._41 = 0.0f;    rotationMatrix._42 = 0.0f;    rotationMatrix._43 = 0.0f;    rotationMatrix._44 = 1.0f;

					bullet->SetRotationTransform(&rotationMatrix);

					SwitchShot();
				}
			}
		}
	}

	UpdateBoundingBox();
	bullet->UpdateBoundingBox();
}
void CTankObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	if (IsExist()) {
		if (m_bBlowingUp)
		{
			for (int i = 0; i < EXPLOSION_DEBRISES; i++) {
				if (pCamera->IsInFrustum(m_xmOOBB)) {
					if (m_pxmf4x4Transforms[i]._42 > 0.2f) {
						CGameObject::Render(hDCFrameBuffer, &m_pxmf4x4Transforms[i], m_pExplosionMesh);
					}
				}
			}
		}
		else
		{
			CGameObject::Render(hDCFrameBuffer, pCamera);
			if (shot) 
				bullet->Render(hDCFrameBuffer, pCamera);
		}
	}
}

CMesh* CTankObject::m_pExplosionMesh = NULL;
void CTankObject::PrepareExplosion()
{
	m_bBlowingUp = true;
	m_fElapsedTimes = 0.0f;

	if (!m_pExplosionMesh) m_pExplosionMesh = new CCubeMesh(0.05f, 0.05f, 0.05f);

	for (int i = 0; i < EXPLOSION_DEBRISES; i++) {
		XMStoreFloat3(&m_pxmf3SphereVectors[i], RandomUnitVectorOnSphere());
	}
}
CBulletObject::CBulletObject()
{
}
CBulletObject::~CBulletObject()
{
}
void CBulletObject::Animate(float fElapsedTime)
{
}
void CBulletObject::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGameObject::Render(hDCFrameBuffer, pCamera);
}