#include "stdafx.h"
#include "msc_GameObject.h"
#include "msc_Transform.h"
#include "msc_Component.h"
#include "msc_Mesh.h"
#include "msc_D3D12RenderingEngine.h"

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
	// 시작할 때 한 번만 열어둠
	//constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedDataBegin)); //
}

void msc_Mesh::Update()
{
}

void msc_Mesh::onDestroy()
{
	// 죽을 때 닫아줌
	/*
	if (m_constantBuffer != nullptr) {
		m_constantBuffer->Unmap(0, nullptr);
	}
	*/
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
	//std::cout << "Draw Line: " << f3Previous.x << ", " << f3Previous.y << " to " << f3Current.x << ", " << f3Current.y << std::endl;
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
void msc_Mesh::Render(HDC hDCFrameBuffer, XMFLOAT4X4* pxmf4x4World) {
	XMFLOAT4X4 xmf4x4World = *pxmf4x4World;
	CGraphicsPipeline::SetWorldTransform(&xmf4x4World);
	HPEN hPen = ::CreatePen(PS_SOLID, 0, RGB(125,125,125));
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	this->Render(hDCFrameBuffer);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
}
void msc_Mesh::Render(HDC hDCFrameBuffer, msc_Camera* pCamera)
{
	if (!pCamera) return;

	//std::cout << "Rendering Mesh: " << m_pParentObject->m_strName << std::endl;

	XMFLOAT4X4 xmf4x4World = m_pTransform->GetWorldMatrix();
	CGraphicsPipeline::SetWorldTransform(&xmf4x4World);
	HPEN hPen = ::CreatePen(PS_SOLID, 0, RGB(125,125,125));
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	this->Render(hDCFrameBuffer);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
}
void msc_Mesh::Render(HDC hDCFrameBuffer)
{
	if (!m_ppPolygons_simple || !m_pTransform) return;
	XMFLOAT3 f3InitialProject, f3PreviousProject;
	bool bPreviousInside = false, bInitialInside = false, bCurrentInside = false;
	/*
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
	*/
	for (int j = 0; j < m_nPolygons_simple; j++)
	{
		int nVertices = m_ppPolygons_simple[j]->m_nVertices;
		msc_Vertex* pVertices = m_ppPolygons_simple[j]->m_pVertices;

		f3PreviousProject = f3InitialProject = CGraphicsPipeline::Project(pVertices[0].m_xmf3Position);
		bPreviousInside = bInitialInside = (-1.0f <= f3InitialProject.x) && (f3InitialProject.x <= 1.0f) && (-1.0f <= f3InitialProject.y) && (f3InitialProject.y <= 1.0f);
		
		for (int i = 1; i < nVertices; i++)
		{
			//std::cout << "tetstest" << std::endl;
			XMFLOAT3 f3CurrentProject = CGraphicsPipeline::Project(pVertices[i].m_xmf3Position);
			bCurrentInside = (-1.0f <= f3CurrentProject.x) && (f3CurrentProject.x <= 1.0f) && (-1.0f <= f3CurrentProject.y) && (f3CurrentProject.y <= 1.0f);
			
			if (((0.0f <= f3CurrentProject.z) && (f3CurrentProject.z <= 1000.0f)) && ((bCurrentInside || bPreviousInside))) {
				::Draw2DLine_msc(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
			}
			
			f3PreviousProject = f3CurrentProject;
			bPreviousInside = bCurrentInside;
		}
		
		if (((0.0f <= f3InitialProject.z) && (f3InitialProject.z <= 1000.0f)) && ((bInitialInside || bPreviousInside))) 
			::Draw2DLine_msc(hDCFrameBuffer, f3PreviousProject, f3InitialProject);
	}
}
void msc_Mesh::UpdateConstantBuffer(const XMMATRIX& worldMatrix) {
	// 프레임마다 Map/Unmap 없이 초고속으로 복사만 수행!
	//memcpy(mappedDataBegin, &worldMatrix, sizeof(XMMATRIX));
}
void msc_Mesh::RenderD3D12(const RenderContext& context)
{
	// D3D12 렌더링 구현은 추후에 추가될 예정입니다.
	// 이 함수는 Direct3D 12를 사용하여 메쉬를 렌더링하는 데 필요한 로직을 포함할 것입니다.
	// 예를 들어, 버텍스 버퍼와 인덱스 버퍼를 설정하고, 셰이더를 바인딩하고, 드로우 콜을 실행하는 등의 작업이 포함될 수 있습니다.
	XMMATRIX xmf4x4World = XMLoadFloat4x4(&(m_pTransform->GetWorldMatrix()));
	//UpdateConstantBuffer(context.cmdList, m_pd3dConstantBuffer.Get(), &xmf4x4World, sizeof(XMMATRIX));
	//context.cmdList->SetGraphicsRootSignature(m_pd3dRootSignature.Get());
	/*
	// 1. 자신의 정점/인덱스 버퍼 뷰를 파이프라인에 바인딩
	// (GPU에게 "이 정점들과 인덱스들을 사용해라"라고 지시)
	context.cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	context.cmdList->IASetIndexBuffer(&m_indexBufferView);

	// 2. 프리미티브 토폴로지 설정 (삼각형 리스트 형태로 해석)
	context.cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 3. 상수 버퍼(월드 행렬) 바인딩
	// Root Signature의 0번 슬롯이 CBV를 기다리고 있다고 가정합니다.
	context.cmdList->SetGraphicsRootConstantBufferView(0, m_constantBuffer->GetGPUVirtualAddress());

	// 4. 그리기 명령 기록 (실제로 그리지는 않고 장부에 적기만 함)
	// 인덱스 개수, 인스턴스 개수, 시작 인덱스, 시작 정점 위치 등을 전달
	context.cmdList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
	*/
}
BOOL msc_Mesh::RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance)
{
	float fHitDistance;
	BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
	if (bIntersected && (fHitDistance < *pfNearHitDistance)) *pfNearHitDistance = fHitDistance;

	return(bIntersected);
}
/*
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
*/

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