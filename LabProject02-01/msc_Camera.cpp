#include "stdafx.h"
#include "msc_GameObject.h"
#include "msc_Transform.h"
#include "msc_Component.h"
#include "msc_Camera.h"
#include "GameFramework.h"
void msc_Viewport::SetViewport(int nLeft, int nTop, int nWidth, int nHeight)
{
	m_nLeft = nLeft;
	m_nTop = nTop;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

msc_Camera::msc_Camera(msc_GameObject* pParentObject) : msc_Component(pParentObject)
{
}

msc_Camera::~msc_Camera()
{
}

void msc_Camera::Start()
{
	std::cout << "Camera Start" << std::endl;
	if(CGameFramework::GetInstance().gf_SceneManager->main_scene->mainCamera == nullptr)
	{
		CGameFramework::GetInstance().gf_SceneManager->main_scene->mainCamera = this;
	}

	//기본 설정
	GetTransform()->SetLocalPosition(XMFLOAT3(0.0f, 5.0f, -15.0f));

	SetFOVAngle(60.0f);
	InitializePerspectiveProjection(m_fNearPlaneDistance, m_fFarPlaneDistance);
	SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

	// 기본 뷰포트 설정
	m_Viewport.SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	m_fAspectRatio = float(FRAMEBUFFER_WIDTH) / float(FRAMEBUFFER_HEIGHT);
	
}

void msc_Camera::Update()
{
	// Transform에서 위치 및 회전 정보를 가져와 뷰 매트릭스 생성
	GenerateViewMatrix();
}

void msc_Camera::onDestroy()
{
}

void msc_Camera::SetViewport(int nLeft, int nTop, int nWidth, int nHeight)
{
	m_Viewport.SetViewport(nLeft, nTop, nWidth, nHeight);
	m_fAspectRatio = float(m_Viewport.m_nWidth) / float(m_Viewport.m_nHeight);
	
	// 애스펙트 비율 변경 시 프로젝션 재설정
	GeneratePerspectiveProjectionMatrix(m_fNearPlaneDistance, m_fFarPlaneDistance);
}
void msc_Camera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}

void msc_Camera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}
void msc_Camera::SetFOVAngle(float fFOVAngle)
{
	m_fFOVAngle = fFOVAngle;
	m_fProjectRectDistance = float(1.0f / tan(DegreeToRadian(fFOVAngle * 0.5f)));
}

void msc_Camera::InitializePerspectiveProjection(float fNearPlaneDistance, float fFarPlaneDistance)
{
	GeneratePerspectiveProjectionMatrix(fNearPlaneDistance, fFarPlaneDistance);
}

void msc_Camera::InitializeOrthographicProjection(float fNearPlaneDistance, float fFarPlaneDistance, float fWidth, float fHeight)
{
	GenerateOrthographicProjectionMatrix(fNearPlaneDistance, fFarPlaneDistance, fWidth, fHeight);
	m_bOrthographic = true;
}

void msc_Camera::GenerateViewMatrix()
{
	if (!m_pTransform) return;

	// Transform에서 카메라의 월드 위치와 방향을 가져옴
	XMFLOAT3 xmf3Position = m_pTransform->GetWorldPosition();
	XMFLOAT3 xmf3Forward = m_pTransform->GetForward();  // Look 방향
	XMFLOAT3 xmf3Up = m_pTransform->GetUp();            // Up 방향

	// Look-At 대상 계산 (현재 위치 + 전방 방향)
	XMFLOAT3 xmf3LookAt = XMFLOAT3(
		xmf3Position.x + xmf3Forward.x,
		xmf3Position.y + xmf3Forward.y,
		xmf3Position.z + xmf3Forward.z
	);

	// 뷰 매트릭스 생성
	m_xmf4x4View = Matrix4x4::LookAtLH(xmf3Position, xmf3LookAt, xmf3Up);

	// Inverse View 매트릭스 생성 (프러스텀 변환용)
	XMFLOAT3 xmf3Right = m_pTransform->GetRight();
	
	m_xmf4x4InverseView._11 = xmf3Right.x;   m_xmf4x4InverseView._12 = xmf3Right.y;   m_xmf4x4InverseView._13 = xmf3Right.z;
	m_xmf4x4InverseView._21 = xmf3Up.x;      m_xmf4x4InverseView._22 = xmf3Up.y;      m_xmf4x4InverseView._23 = xmf3Up.z;
	m_xmf4x4InverseView._31 = xmf3Forward.x; m_xmf4x4InverseView._32 = xmf3Forward.y; m_xmf4x4InverseView._33 = xmf3Forward.z;
	m_xmf4x4InverseView._41 = xmf3Position.x; m_xmf4x4InverseView._42 = xmf3Position.y; m_xmf4x4InverseView._43 = xmf3Position.z;

	// 뷰-투영 조합 매트릭스 생성
	m_xmf4x4ViewPerspectiveProject = Matrix4x4::Multiply(m_xmf4x4View, m_xmf4x4PerspectiveProject);
	m_xmf4x4ViewOrthographicProject = Matrix4x4::Multiply(m_xmf4x4View, m_xmf4x4OrthographicProject);

	// 프러스텀 변환
	m_xmFrustumView.Transform(m_xmFrustumWorld, XMLoadFloat4x4(&m_xmf4x4InverseView));
}
void msc_Camera::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 셰이더에 뷰, 투영 매트릭스와 카메라 위치 전달
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(2, 16, &xmf4x4View, 0);

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4PerspectiveProject)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(2, 16, &xmf4x4Projection, 16);

	pd3dCommandList->SetGraphicsRoot32BitConstants(2, 3, &m_pTransform->GetWorldPosition(), 32);
}
void msc_Camera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}
void msc_Camera::GeneratePerspectiveProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance)
{
	// FOV와 Aspect Ratio만으로 프러스텀 정의
	// Scale은 여기에 포함되지 않음
	XMMATRIX xmmtxProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(m_fFOVAngle),
		m_fAspectRatio,
		fNearPlaneDistance,
		fFarPlaneDistance
	);
	XMStoreFloat4x4(&m_xmf4x4PerspectiveProject, xmmtxProjection);

	// 프러스텀 생성
	BoundingFrustum::CreateFromMatrix(m_xmFrustumView, xmmtxProjection);
}

void msc_Camera::GenerateOrthographicProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fWidth, float fHeight)
{
	XMMATRIX xmmtxProjection = XMMatrixOrthographicLH(fWidth, fHeight, fNearPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&m_xmf4x4OrthographicProject, xmmtxProjection);
}

bool msc_Camera::IsInFrustum(BoundingOrientedBox& xmBoundingBox) const
{
	return m_xmFrustumWorld.Intersects(xmBoundingBox);
}
bool msc_Camera::IsInFrustum(BoundingBox& xmBoundingBox) const
{
	return m_xmFrustumWorld.Intersects(xmBoundingBox);
}

// 뷰포트 안에 Transform이 있는지 확인
bool msc_Camera::IsTransformInViewport(const msc_Transform* pTransform) const
{
	if (!pTransform) return false;

	// Transform의 월드 위치 가져오기
	XMFLOAT3 worldPosition = pTransform->GetWorldPosition();

	// 월드 좌표를 뷰 좌표로 변환
	XMVECTOR vWorldPos = XMLoadFloat3(&worldPosition);
	XMMATRIX mtxView = XMLoadFloat4x4(&m_xmf4x4View);
	XMVECTOR vViewPos = XMVector3TransformCoord(vWorldPos, mtxView);

	XMFLOAT3 viewPosition;
	XMStoreFloat3(&viewPosition, vViewPos);

	// 뷰 좌표의 z값이 near/far plane 범위 안에 있는지 확인
	if (viewPosition.z < 0.1f || viewPosition.z > 1000.0f) return false;

	// 투영 변환 적용
	XMMATRIX mtxProj = XMLoadFloat4x4(&m_xmf4x4PerspectiveProject);
	XMVECTOR vProjPos = XMVector3TransformCoord(vViewPos, mtxProj);

	XMFLOAT3 projPosition;
	XMStoreFloat3(&projPosition, vProjPos);

	// 투영 좌표가 [-1, 1] 범위 안에 있는지 확인
	if (projPosition.x < -1.0f || projPosition.x > 1.0f) return false;
	if (projPosition.y < -1.0f || projPosition.y > 1.0f) return false;
	if (projPosition.z < 0.0f || projPosition.z > 1.0f) return false;

	return true;
}

// 투영 좌표가 뷰포트 범위 안에 있는지 확인
bool msc_Camera::IsProjectedCoordInViewport(const XMFLOAT3& projectedCoord) const
{
	// 투영 좌표 [-1, 1] 범위 확인
	if (projectedCoord.x < -1.0f || projectedCoord.x > 1.0f) return false;
	if (projectedCoord.y < -1.0f || projectedCoord.y > 1.0f) return false;
	if (projectedCoord.z < 0.0f || projectedCoord.z > 1.0f) return false;

	// 뷰포트가 설정되었는지 확인
	if (m_Viewport.m_nWidth <= 0 || m_Viewport.m_nHeight <= 0) return false;

	// 스크린 좌표로 변환
	float fHalfWidth = m_Viewport.m_nWidth * 0.5f;
	float fHalfHeight = m_Viewport.m_nHeight * 0.5f;
	
	float screenX = m_Viewport.m_nLeft + (projectedCoord.x * fHalfWidth) + fHalfWidth;
	float screenY = m_Viewport.m_nTop + (-projectedCoord.y * fHalfHeight) + fHalfHeight;

	// 스크린 좌표가 뷰포트 범위 안에 있는지 확인
	if (screenX < m_Viewport.m_nLeft || screenX > m_Viewport.m_nLeft + m_Viewport.m_nWidth) return false;
	if (screenY < m_Viewport.m_nTop || screenY > m_Viewport.m_nTop + m_Viewport.m_nHeight) return false;

	return true;
}

// 월드 좌표가 카메라 프러스텀 안에 있는지 확인
bool msc_Camera::IsWorldPositionInFrustum(const XMFLOAT3& worldPosition) const
{
	// 월드 좌표를 뷰 좌표로 변환
	XMVECTOR vWorldPos = XMLoadFloat3(&worldPosition);
	XMMATRIX mtxView = XMLoadFloat4x4(&m_xmf4x4View);
	XMVECTOR vViewPos = XMVector3TransformCoord(vWorldPos, mtxView);

	XMFLOAT3 viewPosition;
	XMStoreFloat3(&viewPosition, vViewPos);

	// near/far plane 범위 확인
	if (viewPosition.z < 0.1f || viewPosition.z > 1000.0f) return false;

	// 투영 변환 적용
	XMMATRIX mtxProj = XMLoadFloat4x4(&m_xmf4x4PerspectiveProject);
	XMVECTOR vProjPos = XMVector3TransformCoord(vViewPos, mtxProj);

	XMFLOAT3 projPosition;
	XMStoreFloat3(&projPosition, vProjPos);

	// 투영 좌표가 [-1, 1] 범위 안에 있는지 확인
	return (projPosition.x >= -1.0f && projPosition.x <= 1.0f &&
			projPosition.y >= -1.0f && projPosition.y <= 1.0f &&
			projPosition.z >= 0.0f && projPosition.z <= 1.0f);
}