#pragma once
#include "msc_Component.h"
#include "GraphicsPipeline.h"
#include "DirectXCollision.h"

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
	virtual void Render(HDC hDCFrameBuffer, msc_Camera* pCamera);
	virtual void Render(HDC hDCFrameBuffer, XMFLOAT4X4* pxmf4x4World);
	//virtual void RenderD3D12(msc_Camera* pCamera);
public:
	//ComPtr<ID3D12Resource> m_pd3dVertexBuffer = nullptr; // D3D12에서 버텍스 버퍼를 나타내는 리소스입니다. 이 버퍼는 GPU에 업로드되어 렌더링에 사용됩니다. 버텍스 버퍼는 일반적으로 정점 데이터(위치, 색상, 텍스처 좌표 등)를 포함하며, GPU가 이 데이터를 사용하여 3D 모델을 렌더링할 수 있도록 합니다. ComPtr은 스마트 포인터로, 리소스 관리를 자동으로 처리하여 메모리 누수를 방지합니다.
	//ComPtr<ID3D12Resource> m_pd3dIndexBuffer = nullptr; // D3D12에서 인덱스 버퍼를 나타내는 리소스입니다. 이 버퍼는 GPU에 업로드되어 렌더링에 사용됩니다. 인덱스 버퍼는 일반적으로 정점 버퍼의 인덱스를 포함하며, GPU가 이 인덱스를 사용하여 정점 데이터를 참조하여 3D 모델을 렌더링할 수 있도록 합니다. ComPtr은 스마트 포인터로, 리소스 관리를 자동으로 처리하여 메모리 누수를 방지합니다.
	//ComPtr<ID3D12Resource> m_pd3dConstantBuffer = nullptr;// D3D12에서 상수 버퍼를 나타내는 리소스입니다. 이 버퍼는 GPU에 업로드되어 렌더링에 사용됩니다. 상수 버퍼는 일반적으로 셰이더에서 사용되는 상수 데이터를 포함하며, GPU가 이 데이터를 사용하여 3D 모델을 렌더링할 때 필요한 정보를 제공할 수 있도록 합니다. ComPtr은 스마트 포인터로, 리소스 관리를 자동으로 처리하여 메모리 누수를 방지합니다.
	
	int CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance);
	
	BoundingOrientedBox GetBoundingBox() const { return m_xmOOBB_simple; }
	void SetBoundingBox(const BoundingOrientedBox& box) { m_xmOOBB_simple = box; }

protected:
	int m_nPolygons_simple = 0;
	msc_Polygon_simple** m_ppPolygons_simple = NULL;

	BoundingOrientedBox m_xmOOBB_simple = BoundingOrientedBox();

	void SetPolygon_simple(int nIndex, msc_Polygon_simple* pPolygon);
	BOOL RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance);
};

class msc_CubeMesh_simple : public msc_Mesh
{
public:
	msc_CubeMesh_simple(msc_GameObject* pParentObject, float fWidth = 1.0f, float fHeight = 1.0f, float fDepth = 1.0f);
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