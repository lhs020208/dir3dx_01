#include "stdafx.h"
#include "Player.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
	if (m_pCamera) delete m_pCamera;
}

void CPlayer::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);

	CGameObject::SetPosition(x, y, z);
}

void CPlayer::SetCameraOffset(XMFLOAT3& xmf3CameraOffset)
{
	m_xmf3CameraOffset = xmf3CameraOffset;
	m_pCamera->SetLookAt(Vector3::Add(m_xmf3Position, m_xmf3CameraOffset), m_xmf3Position, m_xmf3Up);
	m_pCamera->GenerateViewMatrix();
}

void CPlayer::Move(DWORD dwDirection, float fDistance)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(xmf3Shift, true);
	}
}

void CPlayer::Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(xmf3Shift, m_xmf3Position);
		m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z), false);
}

void CPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	//m_pCamera->Rotate(fPitch, fYaw, fRoll);
	m_pCamera->Rotate(0.0f, fYaw, 0.0f);
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);

}

void CPlayer::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CPlayer::Update(float fTimeElapsed)
{
	Move(m_xmf3Velocity, false);

	m_pCamera->Update(this, m_xmf3Position, fTimeElapsed);
	m_pCamera->GenerateViewMatrix();

	XMFLOAT3 xmf3Deceleration = Vector3::Normalize(Vector3::ScalarProduct(m_xmf3Velocity, -1.0f));
	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = m_fFriction * fTimeElapsed;
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Deceleration, fDeceleration);
}

void CPlayer::reset()
{
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3CameraOffset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_fFriction = 125.0f;

	m_fPitch = 0.0f;
	m_fYaw = 0.0f;
	m_fRoll = 0.0f;
}

void CPlayer::Animate(float fElapsedTime)
{
	OnUpdateTransform();
	CGameObject::Animate(fElapsedTime);
}

void CPlayer::OnUpdateTransform()
{
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;
}

void CPlayer::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGameObject::Render(hDCFrameBuffer, pCamera);
}

void CPlayer::SetUp(const XMFLOAT3& xmf3Up)
{
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
}


/////////////////////////////////////////////////////////////////////////////////////////////
CCubePlayer::CCubePlayer()
{
}

CCubePlayer::~CCubePlayer()
{
}

void CCubePlayer::Animate(float fElapsedTime)
{
	CPlayer::Animate(fElapsedTime);
}

void CCubePlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();

	m_xmf4x4World = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), m_xmf4x4World);
}

void CCubePlayer::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CPlayer::Render(hDCFrameBuffer, pCamera);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CTankPlayer::CTankPlayer()
{
}

//CTankPlayer::~CTankPlayer() {}

void CTankPlayer::Animate(float fElapsedTime)
{
	XMFLOAT3 look = GetLook();
	XMFLOAT3 right = GetRight();

	XMFLOAT3 moveVec = { 0.0f, 0.0f, 0.0f };

	// 이동 거리 스케일
	float speed = fElapsedTime * 0.5f;

	// 방향별 적용
	moveVec.x += right.x * move_x * speed;
	moveVec.z += right.z * move_x * speed;

	moveVec.x += look.x * move_z * speed;
	moveVec.z += look.z * move_z * speed;

	// 위치 갱신
	XMFLOAT3 now_pos = GetPosition();
	SetPosition(now_pos.x + moveVec.x, now_pos.y, now_pos.z + moveVec.z);

	m_pShild->SetPosition(now_pos.x + moveVec.x, now_pos.y, now_pos.z + moveVec.z);

	CTankPlayer::OnUpdateTransform();


	if (shot) {
		bullet_timer++;
		if (Toggle && ToggleObject) {
			m_pBullet->LookAt(ToggleObject->GetPosition(), m_pBullet->GetUp());
		}
		m_pBullet->SetPosition(Vector3::Add(m_pBullet->GetPosition(), Vector3::ScalarProduct(m_pBullet->GetLook(), fElapsedTime * 2.0f, false)));
		if (bullet_timer >= 200) {
			bullet_timer = 0;
			SwitchBullet();
		}
	}

	UpdateBoundingBox();
	m_pBullet->UpdateBoundingBox();
	m_pShild->UpdateBoundingBox();
}

void CTankPlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();
}


void CTankPlayer::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CPlayer::Render(hDCFrameBuffer, pCamera);
	if (OnShild && m_pShild) {
		m_pShild->Render(hDCFrameBuffer, pCamera);
	}
	if (shot) {
		m_pBullet->Render(hDCFrameBuffer, pCamera);
	}
}

void CTankPlayer::SetBulletPosition()
{
	m_pBullet->SetPosition(GetPosition().x, GetPosition().y, GetPosition().z);

	XMFLOAT3 right = GetRight();
	XMFLOAT3 up = GetUp();
	XMFLOAT3 look = GetLook();

	XMFLOAT4X4 rotationMatrix;
	rotationMatrix._11 = right.x; rotationMatrix._12 = right.y; rotationMatrix._13 = right.z; rotationMatrix._14 = 0.0f;
	rotationMatrix._21 = up.x;    rotationMatrix._22 = up.y;    rotationMatrix._23 = up.z;    rotationMatrix._24 = 0.0f;
	rotationMatrix._31 = look.x;  rotationMatrix._32 = look.y;  rotationMatrix._33 = look.z;  rotationMatrix._34 = 0.0f;
	rotationMatrix._41 = 0.0f;    rotationMatrix._42 = 0.0f;    rotationMatrix._43 = 0.0f;    rotationMatrix._44 = 1.0f;

	m_pBullet->SetRotationTransform(&rotationMatrix);
}

void CTankPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	CPlayer::Rotate(fPitch, fYaw, fRoll);
	XMFLOAT3 right = GetRight();
	XMFLOAT3 up = GetUp();
	XMFLOAT3 look = GetLook();

	XMFLOAT4X4 rotationMatrix;
	rotationMatrix._11 = right.x; rotationMatrix._12 = right.y; rotationMatrix._13 = right.z; rotationMatrix._14 = 0.0f;
	rotationMatrix._21 = up.x;    rotationMatrix._22 = up.y;    rotationMatrix._23 = up.z;    rotationMatrix._24 = 0.0f;
	rotationMatrix._31 = look.x;  rotationMatrix._32 = look.y;  rotationMatrix._33 = look.z;  rotationMatrix._34 = 0.0f;
	rotationMatrix._41 = 0.0f;    rotationMatrix._42 = 0.0f;    rotationMatrix._43 = 0.0f;    rotationMatrix._44 = 1.0f;

	m_pShild->SetRotationTransform(&rotationMatrix);

	UpdateBoundingBox();
}