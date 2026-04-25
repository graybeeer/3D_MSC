#include "stdafx.h"
#include "msc_GameObject.h"
#include "msc_Transform.h"

msc_Transform::msc_Transform(msc_GameObject* pParentObject) 
    : msc_Component(pParentObject)
{
    // 초기 회전: 단위 쿼터니언
    m_xmf4LocalRotation = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

msc_Transform::~msc_Transform()
{
}

void msc_Transform::Start()
{
    m_bDirty = true;
    UpdateWorldTransform();
}

void msc_Transform::Update()
{
    if (m_bDirty)
    {
        UpdateWorldTransform();
        m_bDirty = false;
    }
}

void msc_Transform::onDestroy()
{
}

msc_Transform* msc_Transform::GetParentTransform() const
{
    if (!m_pParentObject) return nullptr;
    
    msc_GameObject* pParent = m_pParentObject->GetParent();
    if (!pParent) return nullptr;
    
    return pParent->GetTransform();
}

void msc_Transform::UpdateWorldTransform()
{
    // 로컬 변환 행렬 계산 (Scale * Rotation * Translation)
    XMMATRIX xmmtxTranslation = XMMatrixTranslation(
        m_xmf3LocalPosition.x, 
        m_xmf3LocalPosition.y, 
        m_xmf3LocalPosition.z
    );
    
    // 쿼터니언을 회전 행렬로 변환
    XMVECTOR quat = XMLoadFloat4(&m_xmf4LocalRotation);
    XMMATRIX xmmtxRotation = XMMatrixRotationQuaternion(quat);
    
    XMMATRIX xmmtxScale = XMMatrixScaling(
        m_xmf3LocalScale.x, 
        m_xmf3LocalScale.y, 
        m_xmf3LocalScale.z
    );
    
    // 로컬 행렬 = Scale * Rotation * Translation
    XMMATRIX xmmtxLocal = xmmtxScale * xmmtxRotation * xmmtxTranslation;
    XMStoreFloat4x4(&m_xmf4x4Local, xmmtxLocal);
    
    // 부모가 있으면 부모의 월드 변환과 곱함
    msc_Transform* pParentTransform = GetParentTransform();
    if (pParentTransform)
    {
        XMMATRIX xmmtxParentWorld = XMLoadFloat4x4(&pParentTransform->GetWorldMatrix());
        XMMATRIX xmmtxWorld = xmmtxLocal * xmmtxParentWorld;
        XMStoreFloat4x4(&m_xmf4x4World, xmmtxWorld);
    }
    else
    {
        m_xmf4x4World = m_xmf4x4Local;
    }
}

XMFLOAT3 msc_Transform::GetWorldPosition() const
{
    return XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
}

XMFLOAT4 msc_Transform::GetWorldRotation() const
{
    // 월드 행렬에서 쿼터니언 추출
    XMMATRIX mtxRotation = XMMatrixSet(
        m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13, 0.0f,
        m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23, 0.0f,
        m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    
    XMVECTOR quat = XMQuaternionRotationMatrix(mtxRotation);
    XMFLOAT4 result;
    XMStoreFloat4(&result, quat);
    return result;
}

XMFLOAT3 msc_Transform::GetWorldRotationEuler() const
{
    XMFLOAT4 worldQuat = GetWorldRotation();
    return QuaternionToEuler(worldQuat);
}

XMFLOAT3 msc_Transform::GetLocalRotationEuler() const
{
    return QuaternionToEuler(m_xmf4LocalRotation);
}

XMFLOAT3 msc_Transform::QuaternionToEuler(const XMFLOAT4& quaternion) const
{
    // 쿼터니언 → Euler 각도 변환
    float x = quaternion.x;
    float y = quaternion.y;
    float z = quaternion.z;
    float w = quaternion.w;

    float roll = atan2f(2 * (w * x + y * z), 1 - 2 * (x * x + y * y));
    float pitch = asinf(2 * (w * y - z * x));
    float yaw = atan2f(2 * (w * z + x * y), 1 - 2 * (y * y + z * z));

    return XMFLOAT3(
        XMConvertToDegrees(pitch),
        XMConvertToDegrees(yaw),
        XMConvertToDegrees(roll)
    );
}

XMFLOAT3 msc_Transform::GetWorldScale() const
{
    XMFLOAT3 xmf3Scale;
    xmf3Scale.x = sqrtf(m_xmf4x4World._11 * m_xmf4x4World._11 + 
                         m_xmf4x4World._12 * m_xmf4x4World._12 + 
                         m_xmf4x4World._13 * m_xmf4x4World._13);
    xmf3Scale.y = sqrtf(m_xmf4x4World._21 * m_xmf4x4World._21 + 
                         m_xmf4x4World._22 * m_xmf4x4World._22 + 
                         m_xmf4x4World._23 * m_xmf4x4World._23);
    xmf3Scale.z = sqrtf(m_xmf4x4World._31 * m_xmf4x4World._31 + 
                         m_xmf4x4World._32 * m_xmf4x4World._32 + 
                         m_xmf4x4World._33 * m_xmf4x4World._33);
    
    return xmf3Scale;
}

void msc_Transform::RotateLocal(const XMFLOAT3& eulerDelta)
{
    // Euler 델타 → 쿼터니언 변환
    XMVECTOR quatDelta = XMQuaternionRotationRollPitchYaw(
        XMConvertToRadians(eulerDelta.x),
        XMConvertToRadians(eulerDelta.y),
        XMConvertToRadians(eulerDelta.z)
    );
    
    // 기존 로컬 회전에 델타 적용
    XMVECTOR quatLocal = XMLoadFloat4(&m_xmf4LocalRotation);
    XMVECTOR quatResult = XMQuaternionMultiply(quatDelta, quatLocal);
    
    XMStoreFloat4(&m_xmf4LocalRotation, quatResult);
    m_bDirty = true;
}

void msc_Transform::RotateWorld(const XMFLOAT3& eulerDelta)
{
    // Euler 델타 → 쿼터니언 변환
    XMVECTOR quatDelta = XMQuaternionRotationRollPitchYaw(
        XMConvertToRadians(eulerDelta.x),
        XMConvertToRadians(eulerDelta.y),
        XMConvertToRadians(eulerDelta.z)
    );
    
    // 기존 로컬 회전에 델타 적용 (순서 변경)
    XMVECTOR quatLocal = XMLoadFloat4(&m_xmf4LocalRotation);
    XMVECTOR quatResult = XMQuaternionMultiply(quatLocal, quatDelta);
    
    XMStoreFloat4(&m_xmf4LocalRotation, quatResult);
    m_bDirty = true;
}

void msc_Transform::RotateAroundAxis(const XMFLOAT3& axis, float angleDegrees)
{
    XMVECTOR quatAxis = XMQuaternionRotationAxis(
        XMLoadFloat3(&axis),
        XMConvertToRadians(angleDegrees)
    );
    
    XMVECTOR quatLocal = XMLoadFloat4(&m_xmf4LocalRotation);
    XMVECTOR quatResult = XMQuaternionMultiply(quatAxis, quatLocal);
    
    XMStoreFloat4(&m_xmf4LocalRotation, quatResult);
    m_bDirty = true;
}

void msc_Transform::LookAt(const msc_Transform* target, const XMFLOAT3& upDirection)
{
    if (!target) return;
    
    XMFLOAT3 targetPosition = target->GetWorldPosition();
    XMFLOAT3 currentPosition = GetWorldPosition();
    
    // LookAt 벡터 계산
    XMVECTOR direction = XMLoadFloat3(&targetPosition) - XMLoadFloat3(&currentPosition);
    direction = XMVector3Normalize(direction);
    
    // 월드 X축 (Right) 계산
    XMVECTOR worldUp = XMLoadFloat3(&upDirection);
    XMVECTOR right = XMVector3Cross(worldUp, direction);
    right = XMVector3Normalize(right);
    
    // 실제 Up 계산
    XMVECTOR up = XMVector3Cross(direction, right);
    up = XMVector3Normalize(up);
    
    // 회전 행렬 구성
    XMMATRIX mtxRotation = XMMatrixSet(
        XMVectorGetX(right), XMVectorGetX(up), XMVectorGetX(direction), 0.0f,
        XMVectorGetY(right), XMVectorGetY(up), XMVectorGetY(direction), 0.0f,
        XMVectorGetZ(right), XMVectorGetZ(up), XMVectorGetZ(direction), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    
    // 회전 행렬 → 쿼터니언 변환
    XMVECTOR quat = XMQuaternionRotationMatrix(mtxRotation);
    XMStoreFloat4(&m_xmf4LocalRotation, quat);
    
    m_bDirty = true;
}
XMFLOAT3 msc_Transform::GetForward() const
{
    // 월드 행렬에서 -Z축 추출
    return XMFLOAT3(-m_xmf4x4World._13, -m_xmf4x4World._23, -m_xmf4x4World._33);
}

XMFLOAT3 msc_Transform::GetRight() const
{
    // 월드 행렬에서 X축 추출
    return XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._21, m_xmf4x4World._31);
}

XMFLOAT3 msc_Transform::GetUp() const
{
    // 월드 행렬에서 Y축 추출
    return XMFLOAT3(m_xmf4x4World._12, m_xmf4x4World._22, m_xmf4x4World._32);
}

// 로컬 좌표 → 월드 좌표
XMFLOAT3 msc_Transform::TransformPoint(const XMFLOAT3& localPoint) const
{
    XMVECTOR point = XMLoadFloat3(&localPoint);
    XMMATRIX world = XMLoadFloat4x4(&m_xmf4x4World);
    XMVECTOR worldPoint = XMVector3TransformCoord(point, world);

    XMFLOAT3 result;
    XMStoreFloat3(&result, worldPoint);
    return result;
}

// 월드 좌표 → 로컬 좌표
XMFLOAT3 msc_Transform::InverseTransformPoint(const XMFLOAT3& worldPoint) const
{
    XMVECTOR point = XMLoadFloat3(&worldPoint);
    XMMATRIX world = XMLoadFloat4x4(&m_xmf4x4World);
    XMMATRIX inverseWorld = XMMatrixInverse(nullptr, world);
    XMVECTOR localPoint = XMVector3TransformCoord(point, inverseWorld);

    XMFLOAT3 result;
    XMStoreFloat3(&result, localPoint);
    return result;
}

// 이동
void msc_Transform::Translate(const XMFLOAT3& translation, bool relativeToWorld)
{
    if (relativeToWorld)
    {
        m_xmf3LocalPosition.x += translation.x;
        m_xmf3LocalPosition.y += translation.y;
        m_xmf3LocalPosition.z += translation.z;
    }
    else
    {
        // 로컬 이동
        XMFLOAT3 localMovement = TransformDirection(translation);
        m_xmf3LocalPosition.x += localMovement.x;
        m_xmf3LocalPosition.y += localMovement.y;
        m_xmf3LocalPosition.z += localMovement.z;
    }
    m_bDirty = true;
}
// 로컬 방향 → 월드 방향
XMFLOAT3 msc_Transform::TransformDirection(const XMFLOAT3& localDirection) const
{
    XMVECTOR direction = XMLoadFloat3(&localDirection);
    XMMATRIX world = XMLoadFloat4x4(&m_xmf4x4World);
    XMVECTOR worldDirection = XMVector3TransformNormal(direction, world);

    XMFLOAT3 result;
    XMStoreFloat3(&result, worldDirection);
    return result;
}

// 월드 방향 → 로컬 방향
XMFLOAT3 msc_Transform::InverseTransformDirection(const XMFLOAT3& worldDirection) const
{
    XMVECTOR direction = XMLoadFloat3(&worldDirection);
    XMMATRIX world = XMLoadFloat4x4(&m_xmf4x4World);
    XMMATRIX inverseWorld = XMMatrixInverse(nullptr, world);
    XMVECTOR localDirection = XMVector3TransformNormal(direction, inverseWorld);

    XMFLOAT3 result;
    XMStoreFloat3(&result, localDirection);
    return result;
}
// 자식 찾기
msc_Transform* msc_Transform::FindChild(const string& childName, bool recursive)
{
    if (!m_pParentObject) return nullptr;

    list<msc_GameObject*>& children = m_pParentObject->GetChildren();

    for (auto& child : children)
    {
        if (child->m_strName == childName)
            return child->GetTransform();

        if (recursive)
        {
            msc_Transform* found = child->GetTransform()->FindChild(childName, true);
            if (found) return found;
        }
    }

    return nullptr;
}