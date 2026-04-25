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

	msc_Viewport m_Viewport;

private:
	float						m_fFOVAngle = 90.0f;
	float						m_fProjectRectDistance = 1.0f;

	//카메라 오브젝트의 위치와 회전만 카메라의 뷰 행렬에 영향. scale은 무시
	BoundingFrustum				m_xmFrustumView = BoundingFrustum();
	BoundingFrustum				m_xmFrustumWorld = BoundingFrustum();
	XMFLOAT4X4					m_xmf4x4InverseView = Matrix4x4::Identity();

};
