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

    // ===== Position (Public) =====
    XMFLOAT3 GetLocalPosition() const { return m_xmf3LocalPosition; }
    XMFLOAT3 GetWorldPosition() const;
    
    void SetLocalPosition(const XMFLOAT3& position) 
    { 
        m_xmf3LocalPosition = position; 
        m_bDirty = true; 
    }

    // ===== Rotation (Quaternion Public Interface) =====
    // 쿼터니언으로 로컬 회전 설정
    void SetLocalRotation(const XMFLOAT4& quaternion)
    {
        m_xmf4LocalRotation = quaternion;
        m_bDirty = true;
    }

    // 쿼터니언으로 로컬 회전 조회
    XMFLOAT4 GetLocalRotation() const 
    { 
        return m_xmf4LocalRotation; 
    }

    // Euler 각도로 로컬 회전 설정 (내부적으로 쿼터니언 변환)
    void SetLocalRotationEuler(const XMFLOAT3& eulerAngles)
    {
        XMVECTOR quat = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(eulerAngles.x),
            XMConvertToRadians(eulerAngles.y),
            XMConvertToRadians(eulerAngles.z)
        );
        XMStoreFloat4(&m_xmf4LocalRotation, quat);
        m_bDirty = true;
    }

    // 월드 회전을 쿼터니언으로 조회
    XMFLOAT4 GetWorldRotation() const;

    // 월드 회전을 Euler 각도로 조회 (내부 변환)
    XMFLOAT3 GetWorldRotationEuler() const;

    // 로컬 Euler 각도 조회 (내부 변환용, Getter만 제공)
    XMFLOAT3 GetLocalRotationEuler() const;

    // ===== Scale (Public) =====
    XMFLOAT3 GetLocalScale() const { return m_xmf3LocalScale; }
    XMFLOAT3 GetWorldScale() const;

    void SetLocalScale(const XMFLOAT3& scale) 
    { 
        m_xmf3LocalScale = scale; 
        m_bDirty = true; 
    }

    // ===== Matrix Getter =====
    XMFLOAT4X4 GetWorldMatrix() const { return m_xmf4x4World; }
    XMFLOAT4X4 GetLocalMatrix() const { return m_xmf4x4Local; }

    // ===== Rotation 메서드 (Quaternion 기반) =====
    // 로컬 축 기준 회전 추가
    void RotateLocal(const XMFLOAT3& eulerDelta);
    
    // 월드 축 기준 회전 추가
    void RotateWorld(const XMFLOAT3& eulerDelta);
    
    // 임의 축 기준 회전 추가
    void RotateAroundAxis(const XMFLOAT3& axis, float angleDegrees);

    // 특정 대상을 바라보도록 회전 설정
    void LookAt(const msc_Transform* target, const XMFLOAT3& upDirection = XMFLOAT3(0.0f, 1.0f, 0.0f));

    // 변환 행렬 업데이트
    void UpdateWorldTransform();

    // Dirty flag
    bool m_bDirty = true;

    // ===== Euler Angles Property (Unity 스타일) =====
    XMFLOAT3 GetEulerAngles() const { return GetLocalRotationEuler(); }
    void SetEulerAngles(const XMFLOAT3& euler) { SetLocalRotationEuler(euler); }

    // ===== 방향 벡터 (Forward, Right, Up) =====
    XMFLOAT3 GetForward() const;  // -Z 방향
    XMFLOAT3 GetRight() const;    // X 방향
    XMFLOAT3 GetUp() const;       // Y 방향

    // ===== 변환 메서드 (좌표계 변환) =====
    // 로컬 좌표 → 월드 좌표
    XMFLOAT3 TransformPoint(const XMFLOAT3& localPoint) const;

    // 월드 좌표 → 로컬 좌표
    XMFLOAT3 InverseTransformPoint(const XMFLOAT3& worldPoint) const;

    // 로컬 방향 → 월드 방향
    XMFLOAT3 TransformDirection(const XMFLOAT3& localDirection) const;

    // 월드 방향 → 로컬 방향
    XMFLOAT3 InverseTransformDirection(const XMFLOAT3& worldDirection) const;

    // ===== 이동 메서드 =====
    // 방향과 거리로 이동
    void Translate(const XMFLOAT3& translation, bool relativToWorld = false);

    // ===== 한번에 설정 (성능 최적화) =====
    void SetLocalPositionAndRotation(const XMFLOAT3& position, const XMFLOAT4& rotation)
    {
        m_xmf3LocalPosition = position;
        m_xmf4LocalRotation = rotation;
        m_bDirty = true;
    }

    void GetLocalPositionAndRotation(XMFLOAT3& outPosition, XMFLOAT4& outRotation) const
    {
        outPosition = m_xmf3LocalPosition;
        outRotation = m_xmf4LocalRotation;
    }

    // ===== 자식 Find 메서드 =====
    msc_Transform* Find(const string& childName);
    msc_Transform* FindChild(const string& childName, bool recursive = true);

private:
    // ===== 로컬 변환 (Private) =====
    XMFLOAT3 m_xmf3LocalPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
    
    // 쿼터니언 (x, y, z, w) - 외부 수정 불가
    XMFLOAT4 m_xmf4LocalRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Euler 각도는 계산 목적으로만 사용 (캐시)
    XMFLOAT3 m_xmf3LocalRotationEuler = XMFLOAT3(0.0f, 0.0f, 0.0f);
    
    XMFLOAT3 m_xmf3LocalScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

    // ===== 월드 변환 행렬 =====
    XMFLOAT4X4 m_xmf4x4World = Matrix4x4::Identity();
    XMFLOAT4X4 m_xmf4x4Local = Matrix4x4::Identity();

    // ===== 부모 Transform 조회 =====
    msc_Transform* GetParentTransform() const;

    // ===== Helper 메서드 =====
    // 쿼터니언에서 Euler 각도로 변환
    XMFLOAT3 QuaternionToEuler(const XMFLOAT4& quaternion) const;
};