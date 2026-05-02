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
    void fixedUpdate() override;
    void lateUpdate() override;

    // 위치
    XMFLOAT3 GetLocalPosition() const { return m_xmf3LocalPosition; }
    XMFLOAT3 GetWorldPosition() const;
    
    void SetLocalPosition(const XMFLOAT3& position) 
    { 
        m_xmf3LocalPosition = position; 
        SetDirty();  
    }

    // 회전
    void SetLocalRotation(const XMFLOAT4& quaternion)
    {
        m_xmf4LocalRotation = quaternion;
        SetDirty();  
    }

    XMFLOAT4 GetLocalRotation() const 
    { 
        return m_xmf4LocalRotation; 
    }

    void SetLocalRotationEuler(const XMFLOAT3& eulerAngles)
    {
        XMVECTOR quat = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(eulerAngles.x),
            XMConvertToRadians(eulerAngles.y),
            XMConvertToRadians(eulerAngles.z)
        );
        XMStoreFloat4(&m_xmf4LocalRotation, quat);
        SetDirty();  
    }

    XMFLOAT4 GetWorldRotation() const;
    XMFLOAT3 GetWorldRotationEuler() const;
    XMFLOAT3 GetLocalRotationEuler() const;

    // 크기
    XMFLOAT3 GetLocalScale() const { return m_xmf3LocalScale; }
    XMFLOAT3 GetWorldScale() const;

    void SetLocalScale(const XMFLOAT3& scale) 
    { 
        m_xmf3LocalScale = scale; 
        SetDirty();  
    }

    XMFLOAT4X4 GetWorldMatrix() const { return m_xmf4x4World; }
    XMFLOAT4X4 GetLocalMatrix() const { return m_xmf4x4Local; }

    void RotateLocal(const XMFLOAT3& eulerDelta);
    void RotateWorld(const XMFLOAT3& eulerDelta);
    void RotateAroundAxis(const XMFLOAT3& axis, float angleDegrees);
    void LookAt(const msc_Transform* target, const XMFLOAT3& upDirection = XMFLOAT3(0.0f, 1.0f, 0.0f));
	void LookAt(const XMFLOAT3& targetPosition, const XMFLOAT3& upDirection = XMFLOAT3(0.0f, 1.0f, 0.0f));

    XMFLOAT3 GetForward() const;
    XMFLOAT3 GetRight() const;
    XMFLOAT3 GetUp() const;
    XMFLOAT3 TransformPoint(const XMFLOAT3& localPoint) const;
    XMFLOAT3 InverseTransformPoint(const XMFLOAT3& worldPoint) const;
    XMFLOAT3 TransformDirection(const XMFLOAT3& localDirection) const;
    XMFLOAT3 InverseTransformDirection(const XMFLOAT3& worldDirection) const;
    void Translate(const XMFLOAT3& translation, bool relativeToWorld = false);

    msc_Transform* Find(const std::string& childName);
    msc_Transform* FindChild(const std::string& childName, bool recursive = true);

    //더티 플래그
    void UpdateWorldTransform();
    bool m_bDirty = true;
    void SetDirty()
    {
        m_bDirty = true;
        PropagateToChildren();
    }

private:
    XMFLOAT3 m_xmf3LocalPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT4 m_xmf4LocalRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); //쿼터니언으로 초기화(회전x)
    XMFLOAT3 m_xmf3LocalScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

    XMFLOAT4X4 m_xmf4x4World = Matrix4x4::Identity();
    XMFLOAT4X4 m_xmf4x4Local = Matrix4x4::Identity();

    // 임시
    msc_Transform* GetParentTransform() const;
    XMFLOAT3 QuaternionToEuler(const XMFLOAT4& quaternion) const;
    
    // 자식들에게 Dirty Flag 전파
    void PropagateToChildren();
};