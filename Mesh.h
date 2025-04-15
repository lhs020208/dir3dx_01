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