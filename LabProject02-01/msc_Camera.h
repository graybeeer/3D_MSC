#pragma once
#include "msc_Component.h"

class msc_Viewport
{
public:
	msc_Viewport() {}
	virtual ~msc_Viewport() {}

	int	m_nLeft = 0;
	int m_nTop = 0;
	int	m_nWidth = 0;
	int	m_nHeight = 0;

	void SetViewport(int nLeft, int nTop, int nWidth, int nHeight);
};

class msc_Camera : public msc_Component
{
public:
	msc_Camera(msc_GameObject* pParentObject);
	virtual ~msc_Camera();
	
	void Start() override;
	void Update() override;
	void onDestroy() override;
	

	// ===== Viewport & Projection Settings =====
	void SetViewport(int nLeft, int nTop, int nWidth, int nHeight);
	void SetFOVAngle(float fFOVAngle);
	void SetOrthographic(bool bOrthographic) { m_bOrthographic = bOrthographic; }
	
	// ===== Getters =====
	XMFLOAT4X4 GetViewMatrix() const { return m_xmf4x4View; } // GetViewMatrix()는 카메라의 위치와 방향을 나타내는 뷰 매트릭스를 반환합니다. 이 매트릭스는 월드 공간에서 카메라가 어떻게 배치되어 있는지를 나타냅니다. 일반적으로, 뷰 매트릭스는 카메라의 위치, 타겟(카메라가 바라보는 지점), 그리고 업 벡터(카메라의 위쪽 방향)를 기반으로 생성됩니다. 이 매트릭스는 3D 그래픽스에서 객체를 카메라 시점으로 변환하는 데 사용됩니다.
	XMFLOAT4X4 GetPerspectiveProjectionMatrix() const { return m_xmf4x4PerspectiveProject; } // GetPerspectiveProjectionMatrix()는 원근 투영 매트릭스를 반환합니다. 이 매트릭스는 3D 장면을 2D 화면에 투영하는 방식을 정의합니다. 원근 투영은 멀리 있는 객체가 더 작게 보이는 효과를 만들어냅니다. 이 매트릭스는 카메라의 시야각(FOV), 애스펙트 비율, 그리고 근평면과 원평면의 거리를 기반으로 생성됩니다.
	XMFLOAT4X4 GetOrthographicProjectionMatrix() const { return m_xmf4x4OrthographicProject; } // GetOrthographicProjectionMatrix()는 직교 투영 매트릭스를 반환합니다. 이 매트릭스는 3D 장면을 2D 화면에 투영하는 방식을 정의합니다. 직교 투영은 객체의 크기가 거리에 관계없이 일정하게 유지되는 효과를 만들어냅니다. 이 매트릭스는 카메라의 뷰 폭과 높이, 그리고 근평면과 원평면의 거리를 기반으로 생성됩니다.
	
	XMFLOAT4X4 GetViewPerspectiveProjectionMatrix() const { return m_xmf4x4ViewPerspectiveProject; }// GetViewPerspectiveProjectionMatrix()는 뷰 매트릭스와 원근 투영 매트릭스를 곱한 결과를 반환합니다. 이 매트릭스는 월드 공간에서 객체를 카메라 시점으로 변환한 후, 원근 투영을 적용하여 최종적으로 클립 공간으로 변환하는 데 사용됩니다. 이 매트릭스를 사용하면 월드 공간의 좌표를 한 번에 클립 공간으로 변환할 수 있어 렌더링 과정에서 효율적입니다.
	XMFLOAT4X4 GetViewOrthographicProjectionMatrix() const { return m_xmf4x4ViewOrthographicProject; } // GetViewOrthographicProjectionMatrix()는 뷰 매트릭스와 직교 투영 매트릭스를 곱한 결과를 반환합니다. 이 매트릭스는 월드 공간에서 객체를 카메라 시점으로 변환한 후, 직교 투영을 적용하여 최종적으로 클립 공간으로 변환하는 데 사용됩니다. 이 매트릭스를 사용하면 월드 공간의 좌표를 한 번에 클립 공간으로 변환할 수 있어 렌더링 과정에서 효율적입니다.
	
	bool IsInFrustum(BoundingOrientedBox& xmBoundingBox) const; // IsInFrustum()는 주어진 방향성 경계 상자(BoundingOrientedBox)가 카메라의 뷰 프러스텀 안에 있는지를 검사하는 메서드입니다. 이 메서드는 주로 시야 절두체 컬링(Frustum Culling)에서 사용됩니다. 시야 절두체 컬링은 렌더링 성능을 향상시키기 위해 카메라의 시야에 보이지 않는 객체를 렌더링하지 않는 기술입니다. 이 메서드는 객체가 프러스텀 안에 완전히 포함되어 있는지, 부분적으로 포함되어 있는지, 또는 완전히 포함되어 있지 않은지를 판단하여 반환합니다.
	msc_Viewport& GetViewport() { return m_Viewport; }

	// ===== Initialize Projection =====
	void InitializePerspectiveProjection(float fNearPlaneDistance, float fFarPlaneDistance);
	void InitializeOrthographicProjection(float fNearPlaneDistance, float fFarPlaneDistance, float fWidth, float fHeight);

private:
	// ===== Matrix =====
	XMFLOAT4X4					m_xmf4x4View = Matrix4x4::Identity();
	XMFLOAT4X4					m_xmf4x4PerspectiveProject = Matrix4x4::Identity();
	XMFLOAT4X4					m_xmf4x4OrthographicProject = Matrix4x4::Identity();
	
	XMFLOAT4X4					m_xmf4x4ViewPerspectiveProject = Matrix4x4::Identity();
	XMFLOAT4X4					m_xmf4x4ViewOrthographicProject = Matrix4x4::Identity();
	
	// ===== Frustum =====
	BoundingFrustum				m_xmFrustumView = BoundingFrustum();
	BoundingFrustum				m_xmFrustumWorld = BoundingFrustum();
	XMFLOAT4X4					m_xmf4x4InverseView = Matrix4x4::Identity();

	// ===== Camera Settings =====
	msc_Viewport				m_Viewport;
	float						m_fFOVAngle = 90.0f;
	float						m_fProjectRectDistance = 1.0f;
	float						m_fAspectRatio = float(FRAMEBUFFER_WIDTH) / float(FRAMEBUFFER_HEIGHT);
	bool						m_bOrthographic = false;

	// ===== Internal Helper Methods =====
	void GenerateViewMatrix();
	void GeneratePerspectiveProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance);
	void GenerateOrthographicProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fWidth, float fHeight);
};
