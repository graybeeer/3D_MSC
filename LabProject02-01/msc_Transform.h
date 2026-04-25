#pragma once
#include "msc_Component.h"
#include "stdafx.h"

class msc_Transform : public msc_Component
{
public:
    msc_Transform(msc_GameObject* pParentObject);
    virtual ~msc_Transform();

    // 추상 클래스 구현
    void Update() override;
    void Start() override;
    void onDestroy() override;

    // 로컬 변환 (부모 기준)
    XMFLOAT3 m_xmf3LocalPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3 m_xmf3LocalRotation = XMFLOAT3(0.0f, 0.0f, 0.0f); // Euler angles (Pitch, Yaw, Roll)
    XMFLOAT3 m_xmf3LocalScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

    // 월드 변환 행렬
    XMFLOAT4X4 m_xmf4x4World = Matrix4x4::Identity();
    
    // 로컬 변환 행렬
    XMFLOAT4X4 m_xmf4x4Local = Matrix4x4::Identity();

    // Getter/Setter
    XMFLOAT3 GetLocalPosition() const { return m_xmf3LocalPosition; }
    XMFLOAT3 GetLocalRotation() const { return m_xmf3LocalRotation; }
    XMFLOAT3 GetLocalScale() const { return m_xmf3LocalScale; }

    XMFLOAT4X4 GetWorldMatrix() const { return m_xmf4x4World; }
    XMFLOAT4X4 GetLocalMatrix() const { return m_xmf4x4Local; }

    void SetLocalPosition(const XMFLOAT3& position) { m_xmf3LocalPosition = position; m_bDirty = true; }
    void SetLocalRotation(const XMFLOAT3& rotation) { m_xmf3LocalRotation = rotation; m_bDirty = true; }
    void SetLocalScale(const XMFLOAT3& scale) { m_xmf3LocalScale = scale; m_bDirty = true; }

    // 월드 좌표 getter
    XMFLOAT3 GetWorldPosition() const;
    XMFLOAT3 GetWorldRotation() const;
    XMFLOAT3 GetWorldScale() const;

    // 변환 행렬 업데이트
    void UpdateWorldTransform();
    bool m_bDirty = true; // 변환이 변경되었는지 여부
private:
    
    
    // 부모의 Transform 가져오기
    msc_Transform* GetParentTransform() const;
};