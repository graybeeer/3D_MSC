#pragma once
#include "msc_Component.h"
#include "GraphicsPipeline.h"
#include "DirectXCollision.h"
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_6.h>
using Microsoft::WRL::ComPtr;
struct RenderContext;
class msc_Material;
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
	virtual void RenderD3D12(const RenderContext& context); 
public:
	// --- D3D12 리소스 (소유권자: ComPtr) ---
	ComPtr<ID3D12Resource> m_vertexBuffer = nullptr; // D3D12에서 버텍스 버퍼를 나타내는 리소스입니다. 이 버퍼는 GPU에 업로드되어 렌더링에 사용됩니다. 버텍스 버퍼는 일반적으로 정점 데이터(위치, 색상, 텍스처 좌표 등)를 포함하며, GPU가 이 데이터를 사용하여 3D 모델을 렌더링할 수 있도록 합니다. ComPtr은 스마트 포인터로, 리소스 관리를 자동으로 처리하여 메모리 누수를 방지합니다.
	ComPtr<ID3D12Resource> m_indexBuffer = nullptr; // D3D12에서 인덱스 버퍼를 나타내는 리소스입니다. 이 버퍼는 GPU에 업로드되어 렌더링에 사용됩니다. 인덱스 버퍼는 일반적으로 정점 버퍼의 인덱스를 포함하며, GPU가 이 인덱스를 사용하여 정점 데이터를 참조하여 3D 모델을 렌더링할 수 있도록 합니다. ComPtr은 스마트 포인터로, 리소스 관리를 자동으로 처리하여 메모리 누수를 방지합니다.
	ComPtr<ID3D12Resource> m_constantBuffer = nullptr;// D3D12에서 상수 버퍼를 나타내는 리소스입니다. 이 버퍼는 GPU에 업로드되어 렌더링에 사용됩니다. 상수 버퍼는 일반적으로 셰이더에서 사용되는 상수 데이터를 포함하며, GPU가 이 데이터를 사용하여 3D 모델을 렌더링할 때 필요한 정보를 제공할 수 있도록 합니다. ComPtr은 스마트 포인터로, 리소스 관리를 자동으로 처리하여 메모리 누수를 방지합니다.
	
	// --- 뷰 (설명서) ---
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW  m_indexBufferView;

	// --- 데이터 정보 ---
	UINT m_indexCount = 0;
	void* m_mappedData = nullptr; // 영구 매핑된 상수 버퍼 주소
	
	UINT8* mappedDataBegin; // 뚜껑을 열어둔 주소를 계속 보관
	void UpdateConstantBuffer(const XMMATRIX& worldMatrix); //

	//int CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance);
	
	BoundingOrientedBox GetBoundingBox() const { return m_xmOOBB_simple; }
	void SetBoundingBox(const BoundingOrientedBox& box) { m_xmOOBB_simple = box; }
	std::list<std::shared_ptr<msc_Material>> m_Materials; // msc_Material은 메쉬의 재질을 나타내는 클래스입니다. 이 리스트는 메쉬가 사용할 수 있는 여러 재질을 저장할 수 있도록 합니다. 각 재질은 메쉬의 렌더링에 영향을 미치는 다양한 속성(예: 색상, 반사율, 텍스처 등)을 포함할 수 있습니다. 메쉬가 렌더링될 때, 이 리스트에서 적절한 재질이 선택되어 사용될 수 있습니다. std::shared_ptr를 사용하여 메모리 관리를 자동으로 처리하고, 여러 메쉬가 동일한 재질을 공유할 수 있도록 합니다.

protected:
	int m_nPolygons_simple = 0;
	msc_Polygon_simple** m_ppPolygons_simple = NULL;

	BoundingOrientedBox m_xmOOBB_simple = BoundingOrientedBox();

	void SetPolygon_simple(int nIndex, msc_Polygon_simple* pPolygon);
	BOOL RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance);
};
class msc_CubeMesh : public msc_Mesh
{
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