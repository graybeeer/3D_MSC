#include "stdafx.h"
#include "msc_GameObject.h"
#include "msc_Transform.h"

msc_Transform::msc_Transform(msc_GameObject* pParentObject) : msc_Component(pParentObject)
{
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
    // 매 프레임 월드 변환 행렬 업데이트
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
	if (!pParent) // 부모가 없으면
        return nullptr;
    return pParent->GetTransform();
    
}

void msc_Transform::UpdateWorldTransform()
{
    // 로컬 변환 행렬 계산 (TRS: Translation * Rotation * Scale)
    XMMATRIX xmmtxTranslation = XMMatrixTranslation(m_xmf3LocalPosition.x, m_xmf3LocalPosition.y, m_xmf3LocalPosition.z);
    
    // Euler 각도를 라디안으로 변환하여 회전 행렬 생성
    XMMATRIX xmmtxRotation = XMMatrixRotationRollPitchYaw(
        XMConvertToRadians(m_xmf3LocalRotation.x), // Pitch
        XMConvertToRadians(m_xmf3LocalRotation.y), // Yaw
        XMConvertToRadians(m_xmf3LocalRotation.z)  // Roll
    );
    
    XMMATRIX xmmtxScale = XMMatrixScaling(m_xmf3LocalScale.x, m_xmf3LocalScale.y, m_xmf3LocalScale.z);
    
    // 로컬 변환 행렬 = Scale * Rotation * Translation
    XMMATRIX xmmtxLocal = xmmtxScale * xmmtxRotation * xmmtxTranslation;
    XMStoreFloat4x4(&m_xmf4x4Local, xmmtxLocal);
    
    // 부모가 있으면 부모의 월드 변환 행렬과 곱함
    msc_Transform* pParentTransform = GetParentTransform();
    if (pParentTransform)
    {
        XMMATRIX xmmtxParentWorld = XMLoadFloat4x4(&pParentTransform->GetWorldMatrix());
        XMMATRIX xmmtxWorld = xmmtxLocal * xmmtxParentWorld;
        XMStoreFloat4x4(&m_xmf4x4World, xmmtxWorld);
    }
    else
    {
        // 부모가 없으면 로컬 = 월드
        m_xmf4x4World = m_xmf4x4Local;
    }
}

XMFLOAT3 msc_Transform::GetWorldPosition() const
{
    return XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
}

XMFLOAT3 msc_Transform::GetWorldRotation() const
{
    // 월드 행렬에서 Euler 각도 추출 (근사치)
    float fPitch = atan2f(m_xmf4x4World._32, m_xmf4x4World._33);
    float fYaw = asinf(-m_xmf4x4World._31);
    float fRoll = atan2f(m_xmf4x4World._21, m_xmf4x4World._11);
    
    return XMFLOAT3(
        XMConvertToDegrees(fPitch),
        XMConvertToDegrees(fYaw),
        XMConvertToDegrees(fRoll)
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