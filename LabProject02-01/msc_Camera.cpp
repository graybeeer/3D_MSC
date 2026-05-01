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
	cout << "Camera Start" << endl;
	if(CGameFramework::GetInstance().m_pScene->msc_MainCamera == nullptr)
	{
		CGameFramework::GetInstance().m_pScene->msc_MainCamera = this;
	}
	//기본 설정
	GetTransform()->SetLocalPosition(XMFLOAT3(0.0f, 5.0f, -15.0f));
	SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	InitializePerspectiveProjection(0.1f, 1000.0f);
	SetFOVAngle(60.0f);

	// 기본 뷰포트 설정
	m_Viewport.SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	m_fAspectRatio = float(FRAMEBUFFER_WIDTH) / float(FRAMEBUFFER_HEIGHT);
	
	// 기본 원근 투영 초기화
	InitializePerspectiveProjection(0.1f, 1000.0f);

	
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
	GeneratePerspectiveProjectionMatrix(0.1f, 1000.0f);
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