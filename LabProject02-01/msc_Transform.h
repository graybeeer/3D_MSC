#pragma once
#include "msc_Component.h"
#include "stdafx.h"

class msc_Transform : public msc_Component
{
public:
    msc_Transform(msc_GameObject* pParentObject);
    virtual ~msc_Transform();

    // 추상 클래스가 되지 않도록 순수 가상 함수 구현
    void Update() override;
	void Start() override;
	void onDestroy() override;

    XMFLOAT3 m_xmf3LocalPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3 m_xmf3LocalRotation = XMFLOAT3(0.0f, 0.0f, 0.0f); 
    XMFLOAT3 m_xmf3LocalScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

    XMFLOAT4X4 m_xmf4x4World = Matrix4x4::Identity(); //월드 변환 행렬
};