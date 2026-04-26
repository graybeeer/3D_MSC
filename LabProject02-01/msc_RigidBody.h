#pragma once
#include "msc_Component.h"

class msc_RigidBody : public msc_Component
{
	public:
	msc_RigidBody(msc_GameObject* pParentObject);
	virtual ~msc_RigidBody();
	virtual void Start() override;
	virtual void Update() override;
	virtual void onDestroy() override;
	virtual void fixedUpdate() override;
	virtual void lateUpdate() override;
	XMFLOAT3 m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f); //월드 좌표계에서의 속도
	float m_fMass = 1.0f;
	bool m_bUseGravity = true; // 중력 적용 여부
};