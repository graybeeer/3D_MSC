#include "stdafx.h"
#include "msc_GameObject.h"
#include "msc_Transform.h"
#include "msc_Component.h"
#include "msc_Mesh.h"


msc_Polygon_simple::msc_Polygon_simple(int nVertices)
{
	m_nVertices = nVertices;
	m_pVertices = new msc_Vertex[nVertices];
}

msc_Polygon_simple::~msc_Polygon_simple()
{
	if (m_pVertices) delete[] m_pVertices;
}

void msc_Polygon_simple::SetVertex(int nIndex, msc_Vertex& vertex)
{
	if ((0 <= nIndex) && (nIndex < m_nVertices) && m_pVertices)
	{
		m_pVertices[nIndex] = vertex;
	}
}


msc_Mesh::msc_Mesh(msc_GameObject* pParentObject) : msc_Component(pParentObject)
{
}

msc_Mesh::~msc_Mesh()
{
	if (m_ppPolygons_simple)
	{
		for (int i = 0; i < m_nPolygons_simple; i++) 
			if (m_ppPolygons_simple[i]) delete m_ppPolygons_simple[i];
		delete[] m_ppPolygons_simple;
	}
}

void msc_Mesh::Start()
{
}

void msc_Mesh::Update()
{
}

void msc_Mesh::onDestroy()
{
}

void msc_Mesh::fixedUpdate()
{
}

void msc_Mesh::lateUpdate()
{
}

void msc_Mesh::SetPolygon_simple(int nIndex, msc_Polygon_simple* pPolygon)
{
	if ((0 <= nIndex) && (nIndex < m_nPolygons_simple)) 
		m_ppPolygons_simple[nIndex] = pPolygon;
}

void Draw2DLine_msc(HDC hDCFrameBuffer, XMFLOAT3& f3PreviousProject, XMFLOAT3& f3CurrentProject)
{
	XMFLOAT3 f3Previous = CGraphicsPipeline::ScreenTransform(f3PreviousProject);
	XMFLOAT3 f3Current = CGraphicsPipeline::ScreenTransform(f3CurrentProject);
	::MoveToEx(hDCFrameBuffer, (long)f3Previous.x, (long)f3Previous.y, NULL);
	::LineTo(hDCFrameBuffer, (long)f3Current.x, (long)f3Current.y);
}

void FillPolygon2D_msc(HDC hDCFrameBuffer, XMFLOAT3* pProjectedVertices, int nVertices, DWORD dwColor)
{
	if (nVertices < 3) return;

	HBRUSH hBrush = CreateSolidBrush(dwColor);
	HPEN hPen = CreatePen(PS_SOLID, 1, dwColor);
	
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDCFrameBuffer, hBrush);
	HPEN hOldPen = (HPEN)SelectObject(hDCFrameBuffer, hPen);

	POINT* pPoints = new POINT[nVertices];
	for (int i = 0; i < nVertices; i++)
	{
		XMFLOAT3 f3Screen = CGraphicsPipeline::ScreenTransform(pProjectedVertices[i]);
		pPoints[i].x = (long)f3Screen.x;
		pPoints[i].y = (long)f3Screen.y;
	}

	::Polygon(hDCFrameBuffer, pPoints, nVertices);

	SelectObject(hDCFrameBuffer, hOldBrush);
	SelectObject(hDCFrameBuffer, hOldPen);

	DeleteObject(hBrush);
	DeleteObject(hPen);
	delete[] pPoints;
}

void msc_Mesh::Render(HDC hDCFrameBuffer)
{
	if (!m_ppPolygons_simple || !m_pTransform) return;

	//Transform의 월드 매트릭스 얻기
	XMFLOAT4X4 xmf4x4World = m_pTransform->GetWorldMatrix();
	
	// GraphicsPipeline에 이 메시의 월드 변환 설정
	CGraphicsPipeline::SetWorldTransform(&xmf4x4World);

	XMFLOAT3 f3InitialProject, f3PreviousProject;
	bool bPreviousInside = false, bInitialInside = false, bCurrentInside = false;

	for (int j = 0; j < m_nPolygons_simple; j++)
	{
		int nVertices = m_ppPolygons_simple[j]->m_nVertices;
		msc_Vertex* pVertices = m_ppPolygons_simple[j]->m_pVertices;

		// 모든 정점을 투영 좌표로 변환 (Transform 적용됨)
		XMFLOAT3* pProjectedVertices = new XMFLOAT3[nVertices];
		bool bAllInFrustum = true;

		for (int i = 0; i < nVertices; i++)
		{
			pProjectedVertices[i] = CGraphicsPipeline::Project(pVertices[i].m_xmf3Position);
			if ((pProjectedVertices[i].z < 0.0f) || (pProjectedVertices[i].z > 1.0f))
				bAllInFrustum = false;
		}

		// 프러스텀 안에 있는 면만 채우기
		if (bAllInFrustum)
		{
			FillPolygon2D_msc(hDCFrameBuffer, pProjectedVertices, nVertices, RGB(192, 192, 192));
		}

		delete[] pProjectedVertices;
	}

	for (int j = 0; j < m_nPolygons_simple; j++)
	{
		int nVertices = m_ppPolygons_simple[j]->m_nVertices;
		msc_Vertex* pVertices = m_ppPolygons_simple[j]->m_pVertices;

		f3PreviousProject = f3InitialProject = CGraphicsPipeline::Project(pVertices[0].m_xmf3Position);
		bPreviousInside = bInitialInside = (-1.0f <= f3InitialProject.x) && (f3InitialProject.x <= 1.0f) && (-1.0f <= f3InitialProject.y) && (f3InitialProject.y <= 1.0f);
		
		for (int i = 1; i < nVertices; i++)
		{
			XMFLOAT3 f3CurrentProject = CGraphicsPipeline::Project(pVertices[i].m_xmf3Position);
			bCurrentInside = (-1.0f <= f3CurrentProject.x) && (f3CurrentProject.x <= 1.0f) && (-1.0f <= f3CurrentProject.y) && (f3CurrentProject.y <= 1.0f);
			
			if (((0.0f <= f3CurrentProject.z) && (f3CurrentProject.z <= 1.0f)) && ((bCurrentInside || bPreviousInside))) 
				::Draw2DLine_msc(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
			
			f3PreviousProject = f3CurrentProject;
			bPreviousInside = bCurrentInside;
		}
		
		if (((0.0f <= f3InitialProject.z) && (f3InitialProject.z <= 1.0f)) && ((bInitialInside || bPreviousInside))) 
			::Draw2DLine_msc(hDCFrameBuffer, f3PreviousProject, f3InitialProject);
	}
}

BOOL msc_Mesh::RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance)
{
	float fHitDistance;
	BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
	if (bIntersected && (fHitDistance < *pfNearHitDistance)) *pfNearHitDistance = fHitDistance;

	return(bIntersected);
}

int msc_Mesh::CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance)
{
	int nIntersections = 0;
	bool bIntersected = m_xmOOBB_simple.Intersects(xmvPickRayOrigin, xmvPickRayDirection, *pfNearHitDistance);
	
	if (bIntersected)
	{
		for (int i = 0; i < m_nPolygons_simple; i++)
		{
			switch (m_ppPolygons_simple[i]->m_nVertices)
			{
			case 3:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons_simple[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons_simple[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons_simple[i]->m_pVertices[2].m_xmf3Position));
				BOOL bHit = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bHit) nIntersections++;
				break;
			}
			case 4:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons_simple[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons_simple[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons_simple[i]->m_pVertices[2].m_xmf3Position));
				BOOL bHit = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bHit) nIntersections++;
				
				v0 = XMLoadFloat3(&(m_ppPolygons_simple[i]->m_pVertices[0].m_xmf3Position));
				v1 = XMLoadFloat3(&(m_ppPolygons_simple[i]->m_pVertices[2].m_xmf3Position));
				v2 = XMLoadFloat3(&(m_ppPolygons_simple[i]->m_pVertices[3].m_xmf3Position));
				bHit = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bHit) nIntersections++;
				break;
			}
			}
		}
	}
	return(nIntersections);
}


msc_CubeMesh_simple::msc_CubeMesh_simple(msc_GameObject* pParentObject, float fWidth, float fHeight, float fDepth) 
	: msc_Mesh(pParentObject)
{
	MakeMesh(pParentObject, fWidth, fHeight, fDepth);
}

void msc_CubeMesh_simple::SetSize(float fWidth, float fHeight, float fDepth)
{
	MakeMesh(m_pParentObject, fWidth, fHeight, fDepth);
}

void msc_CubeMesh_simple::MakeMesh(msc_GameObject* pParentObject, float fWidth, float fHeight, float fDepth)
{
	m_nPolygons_simple = 6;
	m_ppPolygons_simple = new msc_Polygon_simple * [6];

	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	// Front Face
	msc_Polygon_simple* pFrontFace = new msc_Polygon_simple(4);
	pFrontFace->SetVertex(0, msc_Vertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, msc_Vertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, msc_Vertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, msc_Vertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon_simple(0, pFrontFace);

	// Top Face
	msc_Polygon_simple* pTopFace = new msc_Polygon_simple(4);
	pTopFace->SetVertex(0, msc_Vertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(1, msc_Vertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(2, msc_Vertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pTopFace->SetVertex(3, msc_Vertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	SetPolygon_simple(1, pTopFace);

	// Back Face
	msc_Polygon_simple* pBackFace = new msc_Polygon_simple(4);
	pBackFace->SetVertex(0, msc_Vertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, msc_Vertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, msc_Vertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, msc_Vertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon_simple(2, pBackFace);

	// Bottom Face
	msc_Polygon_simple* pBottomFace = new msc_Polygon_simple(4);
	pBottomFace->SetVertex(0, msc_Vertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(1, msc_Vertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(2, msc_Vertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(3, msc_Vertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon_simple(3, pBottomFace);

	// Left Face
	msc_Polygon_simple* pLeftFace = new msc_Polygon_simple(4);
	pLeftFace->SetVertex(0, msc_Vertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pLeftFace->SetVertex(1, msc_Vertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(2, msc_Vertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(3, msc_Vertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon_simple(4, pLeftFace);

	// Right Face
	msc_Polygon_simple* pRightFace = new msc_Polygon_simple(4);
	pRightFace->SetVertex(0, msc_Vertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pRightFace->SetVertex(1, msc_Vertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(2, msc_Vertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(3, msc_Vertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon_simple(5, pRightFace);

	// Bounding Box 설정
	m_xmOOBB_simple = BoundingOrientedBox(
		XMFLOAT3(0.0f, 0.0f, 0.0f), 
		XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), 
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}


msc_AirplaneMesh_simple::msc_AirplaneMesh_simple(msc_GameObject* pParentObject, float fWidth, float fHeight, float fDepth)
	: msc_Mesh(pParentObject)
{
	m_nPolygons_simple = 14;
	m_ppPolygons_simple = new msc_Polygon_simple*[14];

	float fx = fWidth * 0.5f;
	float fy = fHeight * 0.5f;
	float fz = fDepth * 0.5f;

	float x1 = fx * 0.3f;
	float y1 = fy * 0.5f;
	float x2 = fx * 0.7f;
	float y2 = fy * 0.2f;
	float x3 = fx;
	float y3 = fy * 0.9f;

	int i = 0;

	// Upper Plane
	msc_Polygon_simple* pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, msc_Vertex(+x1, +y1, -fz));
	pFace->SetVertex(2, msc_Vertex(0.0f, 0.0f, -fz));
	SetPolygon_simple(i++, pFace);

	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, msc_Vertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(2, msc_Vertex(-x1, +y1, -fz));
	SetPolygon_simple(i++, pFace);

	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(+x2, +y2, -fz));
	pFace->SetVertex(1, msc_Vertex(+x1, +y1, -fz));
	pFace->SetVertex(2, msc_Vertex(+x3, -y3, -fz));
	SetPolygon_simple(i++, pFace);

	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(-x2, +y2, -fz));
	pFace->SetVertex(1, msc_Vertex(-x3, -y3, -fz));
	pFace->SetVertex(2, msc_Vertex(-x1, +y1, -fz));
	SetPolygon_simple(i++, pFace);

	// Lower Plane
	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, msc_Vertex(0.0f, 0.0f, +fz));
	pFace->SetVertex(2, msc_Vertex(+x1, +y1, +fz));
	SetPolygon_simple(i++, pFace);

	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, msc_Vertex(-x1, +y1, +fz));
	pFace->SetVertex(2, msc_Vertex(0.0f, 0.0f, +fz));
	SetPolygon_simple(i++, pFace);

	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(+x2, +y2, +fz));
	pFace->SetVertex(1, msc_Vertex(+x3, -y3, +fz));
	pFace->SetVertex(2, msc_Vertex(+x1, +y1, +fz));
	SetPolygon_simple(i++, pFace);

	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(-x2, +y2, +fz));
	pFace->SetVertex(1, msc_Vertex(-x1, +y1, +fz));
	pFace->SetVertex(2, msc_Vertex(-x3, -y3, +fz));
	SetPolygon_simple(i++, pFace);

	// Right Plane
	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, msc_Vertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(2, msc_Vertex(+x2, +y2, -fz));
	SetPolygon_simple(i++, pFace);

	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(+x2, +y2, -fz));
	pFace->SetVertex(1, msc_Vertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(2, msc_Vertex(+x2, +y2, +fz));
	SetPolygon_simple(i++, pFace);

	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(+x2, +y2, -fz));
	pFace->SetVertex(1, msc_Vertex(+x2, +y2, +fz));
	pFace->SetVertex(2, msc_Vertex(+x3, -y3, -fz));
	SetPolygon_simple(i++, pFace);

	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(+x3, -y3, -fz));
	pFace->SetVertex(1, msc_Vertex(+x2, +y2, +fz));
	pFace->SetVertex(2, msc_Vertex(+x3, -y3, +fz));
	SetPolygon_simple(i++, pFace);

	// Left Plane
	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, msc_Vertex(-x2, +y2, -fz));
	pFace->SetVertex(2, msc_Vertex(0.0f, +(fy + y3), +fz));
	SetPolygon_simple(i++, pFace);

	pFace = new msc_Polygon_simple(3);
	pFace->SetVertex(0, msc_Vertex(-x2, +y2, -fz));
	pFace->SetVertex(1, msc_Vertex(-x2, +y2, +fz));
	pFace->SetVertex(2, msc_Vertex(0.0f, +(fy + y3), +fz));
	SetPolygon_simple(i++, pFace);

	// Bounding Box 설정
	m_xmOOBB_simple = BoundingOrientedBox(
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(fx, fy, fz),
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)
	);
}