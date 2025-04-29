#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CVertex
{
public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(float x, float y, float z) { m_xmf3Position = XMFLOAT3(x, y, z); }
	~CVertex() { }

	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
};

class CPolygon
{
public:
	CPolygon() { }
	CPolygon(int nVertices);
	~CPolygon();

	int							m_nVertices = 0;
	CVertex						*m_pVertices = NULL;

	void SetVertex(int nIndex, CVertex& vertex);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMesh
{
public:
	CMesh() { }
	CMesh(const char* filename);
	CMesh(int nPolygons);
	virtual ~CMesh();

private:
	int							m_nReferences = 1;

public:
	void AddRef() { m_nReferences++; }
	void Release() { m_nReferences--; if (m_nReferences <= 0) delete this; }

protected:
	int							m_nPolygons = 0;
	CPolygon					**m_ppPolygons = NULL;

public:
	BoundingOrientedBox			m_xmOOBB = BoundingOrientedBox();

public:
	void SetPolygon(int nIndex, CPolygon *pPolygon);

	virtual void Render(HDC hDCFrameBuffer);

	BOOL RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance);
	int CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance);
};

class CCubeMesh : public CMesh
{
public:
	CCubeMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
    CCubeMesh(const XMVECTOR& start, const XMVECTOR& end, float fWidth = 0.01f)
        : CCubeMesh(6) // 육면체 6면
    {
        // 두 점 간 방향
        XMVECTOR forward = XMVector3Normalize(end - start);

        // Up 벡터 기준 설정
        XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        if (fabs(XMVectorGetX(XMVector3Dot(forward, worldUp))) > 0.99f)
            worldUp = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

        // 직교 벡터 구성
        XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, forward));
        XMVECTOR up = XMVector3Normalize(XMVector3Cross(forward, right));

        // 스케일 조정
        right = XMVectorScale(right, fWidth * 0.5f);
        up = XMVectorScale(up, fWidth * 0.5f);

        // 베이스 위치
        XMVECTOR base0 = start;
        XMVECTOR base1 = end;

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
            {0, 1, 2, 3}, // 앞
            {4, 5, 6, 7}, // 뒤
            {3, 2, 6, 7}, // 위
            {0, 1, 5, 4}, // 아래
            {0, 3, 7, 4}, // 좌
            {1, 2, 6, 5}  // 우
        };

        for (int i = 0; i < 6; ++i) {
            CPolygon* poly = new CPolygon(4);
            for (int j = 0; j < 4; ++j) {
                XMFLOAT3 pos = corners[faceIndices[i][j]];
                poly->SetVertex(j, CVertex(pos.x, pos.y, pos.z));
            }
            SetPolygon(i, poly);
        }

        // 임시 OBB 설정 (렌더에 필요)
        XMFLOAT3 center;
        XMStoreFloat3(&center, XMVectorScale(XMVectorAdd(start, end), 0.5f));
        XMFLOAT3 extent = { fWidth, fWidth, XMVectorGetX(XMVector3Length(end - start)) * 0.5f };
        m_xmOOBB = BoundingOrientedBox(center, extent, XMFLOAT4(0, 0, 0, 1));
    }
	virtual ~CCubeMesh() { }
};


class CAxisMesh : public CMesh
{
public:
	CAxisMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CAxisMesh() { }

	virtual void Render(HDC hDCFrameBuffer);
};

class CTitleMesh : public CMesh
{
public:
	CTitleMesh(const char* filename);
	virtual ~CTitleMesh() {}
};

class CTutorialMesh : public CMesh
{
public:
	CTutorialMesh(const char* filename);
	virtual ~CTutorialMesh() {}
};

class CLevel_1Mesh : public CMesh
{
public:
	CLevel_1Mesh(const char* filename);
	virtual ~CLevel_1Mesh() {}
};

class CLevel_2Mesh : public CMesh
{
public:
	CLevel_2Mesh(const char* filename);
	virtual ~CLevel_2Mesh() {}
};

class CStartMesh : public CMesh
{
public:
	CStartMesh(const char* filename);
	virtual ~CStartMesh() {}
};

class CEndMesh : public CMesh
{
public:
	CEndMesh(const char* filename);
	virtual ~CEndMesh() {}
};

class CRCCubeMesh : public CMesh
{
public:
	CRCCubeMesh(float T, float fWidth = 0.01f, float fHeight = 0.01f);
	virtual ~CRCCubeMesh() {}
};

class CTankMesh : public CMesh
{
public:
	CTankMesh(const char* filename);
	virtual ~CTankMesh() {}
};

class CBulletMesh : public CMesh
{
public:
	CBulletMesh(const char* filename);
	virtual ~CBulletMesh() {}
};
