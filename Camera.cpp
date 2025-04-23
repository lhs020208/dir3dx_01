#include "stdafx.h"
#include "Camera.h"
#include "Mesh.h"
#include "Player.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
void CViewport::SetViewport(int nLeft, int nTop, int nWidth, int nHeight)
{
	m_nLeft = nLeft; 
	m_nTop = nTop; 
	m_nWidth = nWidth; 
	m_nHeight = nHeight; 
}

CCamera::CCamera()
{
}

CCamera::~CCamera()
{
}

void CCamera::GenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Right = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	//m_xmf3Up = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Look, m_xmf3Right));
	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);

	m_xmf4x4ViewPerspectiveProject = Matrix4x4::Multiply(m_xmf4x4View, m_xmf4x4PerspectiveProject);
	m_xmf4x4OrthographicProject = Matrix4x4::Multiply(m_xmf4x4View, m_xmf4x4OrthographicProject);

	m_xmf4x4InverseView._11 = m_xmf3Right.x; m_xmf4x4InverseView._12 = m_xmf3Right.y; m_xmf4x4InverseView._13 = m_xmf3Right.z;
	m_xmf4x4InverseView._21 = m_xmf3Up.x; m_xmf4x4InverseView._22 = m_xmf3Up.y; m_xmf4x4InverseView._23 = m_xmf3Up.z;
	m_xmf4x4InverseView._31 = m_xmf3Look.x; m_xmf4x4InverseView._32 = m_xmf3Look.y; m_xmf4x4InverseView._33 = m_xmf3Look.z;
	m_xmf4x4InverseView._41 = m_xmf3Position.x; m_xmf4x4InverseView._42 = m_xmf3Position.y; m_xmf4x4InverseView._43 = m_xmf3Position.z;

	m_xmFrustumView.Transform(m_xmFrustumWorld, XMLoadFloat4x4(&m_xmf4x4InverseView));

	XMVECTOR eye = XMLoadFloat3(&m_xmf3Position);
	XMVECTOR at = XMLoadFloat3(&m_xmf3Look); // 이 값은 SetLookAt에서 설정돼 있어야 함
	XMVECTOR up = XMLoadFloat3(&m_xmf3Up);

	XMStoreFloat4x4(&m_xmf4x4View, XMMatrixLookAtLH(eye, at, up));
}

void CCamera::SetLookAt(XMFLOAT3& xmf3Position, XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._11, m_xmf4x4View._21, m_xmf4x4View._31));
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._13, m_xmf4x4View._23, m_xmf4x4View._33));
}

void CCamera::SetLookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	//m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	//m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CCamera::SetViewport(int nLeft, int nTop, int nWidth, int nHeight)
{
	m_Viewport.SetViewport(nLeft, nTop, nWidth, nHeight);
	m_fAspectRatio = float(m_Viewport.m_nWidth) / float(m_Viewport.m_nHeight);
}

void CCamera::SetFOVAngle(float fFOVAngle)
{
	m_fFOVAngle = fFOVAngle;
	m_fProjectRectDistance = float(1.0f / tan(DegreeToRadian(fFOVAngle * 0.5f)));
}

void CCamera::GeneratePerspectiveProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fFOVAngle)
{
	float fAspectRatio = (float(m_Viewport.m_nWidth) / float(m_Viewport.m_nHeight));
	XMMATRIX xmmtxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&m_xmf4x4PerspectiveProject, xmmtxProjection);

	BoundingFrustum::CreateFromMatrix(m_xmFrustumView, xmmtxProjection);
}

void CCamera::GenerateOrthographicProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fWidth, float hHeight)
{
	XMMATRIX xmmtxProjection = XMMatrixOrthographicLH(fWidth, hHeight, fNearPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&m_xmf4x4OrthographicProject, xmmtxProjection);
}

bool CCamera::IsInFrustum(BoundingOrientedBox& xmBoundingBox)
{
	//return(m_xmFrustumWorld.Intersects(xmBoundingBox));
	return true;
}

void CCamera::Move(XMFLOAT3& xmf3Shift)
{
	m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
}

void CCamera::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z));
}

void CCamera::Rotate(float fPitch, float fYaw, float fRoll)
{

	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
}

void CCamera::Update(CPlayer* pPlayer, XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{

	if (pPlayer->overview == false) {
		// 1. Up, Look 로드
		XMVECTOR up = XMVector3Normalize(XMLoadFloat3(&pPlayer->m_xmf3Up));
		XMVECTOR look = XMVector3Normalize(XMLoadFloat3(&pPlayer->m_xmf3Look));

		// 2. Right = Up x Look
		XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, look));

		// 3. Look 재정렬 = Right x Up
		look = XMVector3Normalize(XMVector3Cross(right, up)); // 정직교 보정

		// 4. 회전 행렬 구성
		XMMATRIX mtxRotate;
		mtxRotate.r[0] = right;
		mtxRotate.r[1] = up;
		mtxRotate.r[2] = look;
		mtxRotate.r[3] = XMVectorSet(0, 0, 0, 1);

		// 5. 카메라 위치 계산
		XMFLOAT3 xmf3Offset = Vector3::TransformCoord(pPlayer->m_xmf3CameraOffset, mtxRotate);
		XMFLOAT3 xmf3Position = Vector3::Add(pPlayer->m_xmf3Position, xmf3Offset);

		// 6. 위치 적용
		m_xmf3Position = xmf3Position;

		// 7. LookAt 적용 (위치를 바라보게)
		SetLookAt(pPlayer->m_xmf3Position, XMFLOAT3(0.0f, 1.0f, 0.0f));
		// 8. 뷰 행렬 갱신
		GenerateViewMatrix();
	}
	else {
		m_xmf3Position = XMFLOAT3(-30.0f, 10.0f, 30.0f);

		XMFLOAT3 target = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

		SetLookAt(m_xmf3Position, target, up);

		GenerateViewMatrix();
		//m_xmf3Up = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._12, m_xmf4x4View._22, m_xmf4x4View._32));
	}
}


void CCamera::SetOrbitCenter(const XMFLOAT3& center)
{
	m_xmf3OrbitCenter = center;
}

void CCamera::SetOrbitDistance(float distance)
{
	m_fOrbitDistance = distance;
}

void CCamera::RotateOrbit(float deltaYaw, float deltaPitch)
{
	m_fOrbitYaw += deltaYaw;
	m_fOrbitPitch += deltaPitch;

	// pitch 제한
	if (m_fOrbitPitch > XM_PIDIV2 - 0.01f) m_fOrbitPitch = XM_PIDIV2 - 0.01f;
	if (m_fOrbitPitch < -XM_PIDIV2 + 0.01f) m_fOrbitPitch = -XM_PIDIV2 + 0.01f;
}
