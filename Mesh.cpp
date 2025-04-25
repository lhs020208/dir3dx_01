#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "stdafx.h"
#include "Mesh.h"
#include "GraphicsPipeline.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPolygon::CPolygon(int nVertices)
{
	m_nVertices = nVertices;
	m_pVertices = new CVertex[nVertices];
}

CPolygon::~CPolygon()
{
	if (m_pVertices) delete[] m_pVertices;
}

void CPolygon::SetVertex(int nIndex, CVertex& vertex)
{
	if ((0 <= nIndex) && (nIndex < m_nVertices) && m_pVertices)
	{
		m_pVertices[nIndex] = vertex;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
CMesh::CMesh(int nPolygons)
{
	m_nPolygons = nPolygons;
	m_ppPolygons = new CPolygon * [nPolygons];
}

CMesh::~CMesh()
{
	if (m_ppPolygons)
	{
		for (int i = 0; i < m_nPolygons; i++) if (m_ppPolygons[i]) delete m_ppPolygons[i];
		delete[] m_ppPolygons;
	}
}

void CMesh::SetPolygon(int nIndex, CPolygon* pPolygon)
{
	if ((0 <= nIndex) && (nIndex < m_nPolygons)) m_ppPolygons[nIndex] = pPolygon;
}

void Draw2DLine(HDC hDCFrameBuffer, XMFLOAT3& f3PreviousProject, XMFLOAT3& f3CurrentProject)
{
	XMFLOAT3 f3Previous = CGraphicsPipeline::ScreenTransform(f3PreviousProject);
	XMFLOAT3 f3Current = CGraphicsPipeline::ScreenTransform(f3CurrentProject);
	::MoveToEx(hDCFrameBuffer, (long)f3Previous.x, (long)f3Previous.y, NULL);
	::LineTo(hDCFrameBuffer, (long)f3Current.x, (long)f3Current.y);
}

void CMesh::Render(HDC hDCFrameBuffer)
{
	XMFLOAT3 f3InitialProject, f3PreviousProject;
	bool bPreviousInside = false, bInitialInside = false, bCurrentInside = false, bIntersectInside = false;

	for (int j = 0; j < m_nPolygons; j++)
	{
		if (m_ppPolygons[j]->m_nVertices && m_ppPolygons[j]->m_pVertices) {
			int nVertices = m_ppPolygons[j]->m_nVertices;
			CVertex* pVertices = m_ppPolygons[j]->m_pVertices;

			f3PreviousProject = f3InitialProject = CGraphicsPipeline::Project(pVertices[0].m_xmf3Position);
			bPreviousInside = bInitialInside = (-1.0f <= f3InitialProject.x) && (f3InitialProject.x <= 1.0f) && (-1.0f <= f3InitialProject.y) && (f3InitialProject.y <= 1.0f);
			for (int i = 1; i < nVertices; i++)
			{
				XMFLOAT3 f3CurrentProject = CGraphicsPipeline::Project(pVertices[i].m_xmf3Position);
				bCurrentInside = (-1.0f <= f3CurrentProject.x) && (f3CurrentProject.x <= 1.0f) && (-1.0f <= f3CurrentProject.y) && (f3CurrentProject.y <= 1.0f);
				if (((0.0f <= f3CurrentProject.z) && (f3CurrentProject.z <= 1.0f)) && ((bCurrentInside || bPreviousInside))) ::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
				f3PreviousProject = f3CurrentProject;
				bPreviousInside = bCurrentInside;
			}
			if (((0.0f <= f3InitialProject.z) && (f3InitialProject.z <= 1.0f)) && ((bInitialInside || bPreviousInside))) ::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3InitialProject);
		}
	}
}

BOOL CMesh::RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance)
{
	float fHitDistance;
	BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
	if (bIntersected && (fHitDistance < *pfNearHitDistance)) *pfNearHitDistance = fHitDistance;

	return(bIntersected);
}

int CMesh::CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance)
{
	int nIntersections = 0;
	bool bIntersected = m_xmOOBB.Intersects(xmvPickRayOrigin, xmvPickRayDirection, *pfNearHitDistance);
	if (bIntersected)
	{
		for (int i = 0; i < m_nPolygons; i++)
		{
			switch (m_ppPolygons[i]->m_nVertices)
			{
			case 3:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				break;
			}
			case 4:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[3].m_xmf3Position));
				bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				break;
			}
			}
		}
	}
	return(nIntersections);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMesh::CCubeMesh(float fWidth, float fHeight, float fDepth) : CMesh(6)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	CPolygon* pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(0, pFrontFace);

	CPolygon* pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pTopFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	SetPolygon(1, pTopFace);

	CPolygon* pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(2, pBackFace);

	CPolygon* pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(3, pBottomFace);

	CPolygon* pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pLeftFace->SetVertex(1, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(4, pLeftFace);

	CPolygon* pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pRightFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(5, pRightFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAxisMesh::CAxisMesh(float fWidth, float fHeight, float fDepth) : CMesh(3)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	CPolygon* pxAxis = new CPolygon(2);
	pxAxis->SetVertex(0, CVertex(-fHalfWidth, 0.0f, 0.0f));
	pxAxis->SetVertex(1, CVertex(+fHalfWidth, 0.0f, 0.0f));
	SetPolygon(0, pxAxis);

	CPolygon* pyAxis = new CPolygon(2);
	pyAxis->SetVertex(0, CVertex(0.0f, -fHalfWidth, 0.0f));
	pyAxis->SetVertex(1, CVertex(0.0f, +fHalfWidth, 0.0f));
	SetPolygon(1, pyAxis);

	CPolygon* pzAxis = new CPolygon(2);
	pzAxis->SetVertex(0, CVertex(0.0f, 0.0f, -fHalfWidth));
	pzAxis->SetVertex(1, CVertex(0.0f, 0.0f, +fHalfWidth));
	SetPolygon(2, pzAxis);
}

void CAxisMesh::Render(HDC hDCFrameBuffer)
{
	XMFLOAT3 f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[0]->m_pVertices[0].m_xmf3Position);
	XMFLOAT3 f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[0]->m_pVertices[1].m_xmf3Position);
	HPEN hPen = ::CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[1]->m_pVertices[0].m_xmf3Position);
	f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[1]->m_pVertices[1].m_xmf3Position);
	hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[2]->m_pVertices[0].m_xmf3Position);
	f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[2]->m_pVertices[1].m_xmf3Position);
	hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 255, 0));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
}

struct Face3 {
	int i[3];
};

CMesh::CMesh(const char* filename)
{
	std::ifstream file(filename);
	if (!file.is_open()) return;

	std::vector<XMFLOAT3> vertices;
	std::vector<Face3> faces;

	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string prefix;
		iss >> prefix;

		if (prefix == "v") {
			float x, y, z;
			iss >> x >> y >> z;
			vertices.emplace_back(x, y, z);
		}
		else if (prefix == "f") {
			Face3 face;
			for (int i = 0; i < 3; ++i) {
				std::string token;
				iss >> token;
				std::istringstream tokenStream(token);
				std::string vIdx;
				std::getline(tokenStream, vIdx, '/');
				face.i[i] = std::stoi(vIdx) - 1;
			}
			faces.push_back(face);
		}
	}

	file.close();

	m_nPolygons = static_cast<int>(faces.size());
	m_ppPolygons = new CPolygon * [m_nPolygons];

	for (int i = 0; i < m_nPolygons; ++i) {

		if (faces[i].i[0] < 0 || faces[i].i[1] < 0 || faces[i].i[2] < 0) {
			m_ppPolygons[i] = nullptr;
			continue;
		}

		CPolygon* poly = new CPolygon(3);
		for (int j = 0; j < 3; ++j) {
			int vi = faces[i].i[j];
			const XMFLOAT3& pos = vertices[vi];
			poly->SetVertex(j, CVertex(pos.x, pos.y, pos.z));
		}
		SetPolygon(i, poly);
	}

	XMFLOAT3 min = vertices[0], max = vertices[0];
	for (const auto& v : vertices) {
		if (v.x < min.x) min.x = v.x;
		if (v.y < min.y) min.y = v.y;
		if (v.z < min.z) min.z = v.z;

		if (v.x > max.x) max.x = v.x;
		if (v.y > max.y) max.y = v.y;
		if (v.z > max.z) max.z = v.z;
	}

	// 중심과 Extents 계산
	XMFLOAT3 center = XMFLOAT3(
		(min.x + max.x) / 2.0f,
		(min.y + max.y) / 2.0f,
		(min.z + max.z) / 2.0f
	);
	XMFLOAT3 extents = XMFLOAT3(
		(max.x - min.x) / 2.0f,
		(max.y - min.y) / 2.0f,
		(max.z - min.z) / 2.0f
	);

	// OOBB 설정 (기본 orientation)
	m_xmOOBB = BoundingOrientedBox(center, extents, XMFLOAT4(0, 0, 0, 1));
}

CTitleMesh::CTitleMesh(const char* filename) : CMesh(filename) {}
CTutorialMesh::CTutorialMesh(const char* filename) : CMesh(filename) {}
CLevel_1Mesh::CLevel_1Mesh(const char* filename) : CMesh(filename) {}
CLevel_2Mesh::CLevel_2Mesh(const char* filename) : CMesh(filename) {}
CStartMesh::CStartMesh(const char* filename) : CMesh(filename) {}
CEndMesh::CEndMesh(const char* filename) : CMesh(filename) {}

CRCCubeMesh::CRCCubeMesh(float T, float fWidth, float fHeight) : CMesh(6)
{
	XMFLOAT3 p0 = RollerCoasterPos(T);
	XMFLOAT3 p1 = RollerCoasterPos(T + 0.01f);

	XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&p1) - XMLoadFloat3(&p0));

	XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	if (fabs(XMVectorGetX(XMVector3Dot(forward, worldUp))) > 0.99f) {
		worldUp = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	}

	XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, forward));
	XMVECTOR up = XMVector3Normalize(XMVector3Cross(forward, right));

	right = XMVectorScale(right, fWidth * 0.5f);
	up = XMVectorScale(up, fHeight * 0.5f);

	XMVECTOR base0 = XMLoadFloat3(&p0);
	XMVECTOR base1 = XMLoadFloat3(&p1);

	XMVECTOR p0_v[4] = {
		base0 - right - up,
		base0 + right - up,
		base0 + right + up,
		base0 - right + up
	};

	XMVECTOR p1_v[4] = {
		base1 - right - up,
		base1 + right - up,
		base1 + right + up,
		base1 - right + up
	};

	XMFLOAT3 corners[8];
	for (int i = 0; i < 4; ++i) XMStoreFloat3(&corners[i], p0_v[i]);
	for (int i = 0; i < 4; ++i) XMStoreFloat3(&corners[i + 4], p1_v[i]);

	const int faceIndices[6][4] = {
		{0, 1, 2, 3},
		{4, 5, 6, 7},
		{3, 2, 6, 7},
		{0, 1, 5, 4},
		{0, 3, 7, 4},
		{1, 2, 6, 5}
	};

	for (int i = 0; i < 6; ++i) {
		CPolygon* poly = new CPolygon(4);
		for (int j = 0; j < 4; ++j) {
			XMFLOAT3 pos = corners[faceIndices[i][j]];
			poly->SetVertex(j, CVertex(pos.x, pos.y, pos.z));
		}
		SetPolygon(i, poly);
	}

	m_xmOOBB = BoundingOrientedBox(p0, XMFLOAT3(0.01f, fHeight, fWidth), XMFLOAT4(0, 0, 0, 1));
}

CTankMesh::CTankMesh(const char* filename) : CMesh(filename) {}
CBulletMesh::CBulletMesh(const char* filename) : CMesh(filename) {}