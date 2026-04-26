#pragma once
#include "msc_Component.h"
#include "GraphicsPipeline.h"

class msc_Vertex
{
public:
	msc_Vertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	msc_Vertex(float x, float y, float z) { m_xmf3Position = XMFLOAT3(x, y, z); }
	~msc_Vertex() { }

	XMFLOAT3 m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
};

class msc_Polygon_simple
{
public:
	msc_Polygon_simple() { }
	msc_Polygon_simple(int nVertices);
	~msc_Polygon_simple();

	int m_nVertices = 0;
	msc_Vertex* m_pVertices = NULL;

	void SetVertex(int nIndex, msc_Vertex& vertex);
};

class msc_Mesh : public msc_Component
{
public:
	msc_Mesh(msc_GameObject* pParentObject);
	virtual ~msc_Mesh();

	void Start() override;
	void Update() override;
	void onDestroy() override;
	void fixedUpdate() override;
	void lateUpdate() override;

	virtual void Render(HDC hDCFrameBuffer);
	
	// ===== Simple Polygon Mesh Methods =====
	int CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance);
	
	BoundingOrientedBox GetBoundingBox() const { return m_xmOOBB_simple; }
	void SetBoundingBox(const BoundingOrientedBox& box) { m_xmOOBB_simple = box; }

protected:
	// ===== Simple Polygon Structure =====
	int m_nPolygons_simple = 0;
	msc_Polygon_simple** m_ppPolygons_simple = NULL;
	
	BoundingOrientedBox m_xmOOBB_simple = BoundingOrientedBox();

	// ===== Helper Methods =====
	void SetPolygon_simple(int nIndex, msc_Polygon_simple* pPolygon);
	BOOL RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance);
};

// ===== Simple Mesh Implementations =====
class msc_CubeMesh_simple : public msc_Mesh
{
public:
	msc_CubeMesh_simple(msc_GameObject* pParentObject, float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~msc_CubeMesh_simple() { }
	void SetSize(float fWidth, float fHeight, float fDepth);
	void MakeMesh(msc_GameObject* pParentObject,float fWidth, float fHeight, float fDepth);
};

class msc_AirplaneMesh_simple : public msc_Mesh
{
public:
	msc_AirplaneMesh_simple(msc_GameObject* pParentObject, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f);
	virtual ~msc_AirplaneMesh_simple() { }
	void SetSize(float fWidth, float fHeight, float fDepth);
};