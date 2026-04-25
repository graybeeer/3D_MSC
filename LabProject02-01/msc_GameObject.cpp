#include "stdafx.h"
#include "msc_GameObject.h"
#include "msc_Transform.h"
#include "msc_Component.h"

msc_GameObject::msc_GameObject(string& strName)
{
	m_strName = strName;
	m_pTransform = new msc_Transform(this);
	AddComponent(m_pTransform);
}
msc_GameObject::~msc_GameObject()
{
	onDestroy(); 

	if (m_pTransform) delete m_pTransform;
	for (auto& pComponent : m_Components)
	{
		if (pComponent) delete pComponent;
	}
	m_Components.clear();

	// 자식 오브젝트도 정리
	for (auto& pChild : m_ChildGameObjects)
	{
		if (pChild) delete pChild;
	}
	m_ChildGameObjects.clear();
}
void msc_GameObject::Update()
{
	for (auto& pComponent : m_Components)
	{
		if (pComponent) pComponent->Update();
	}

	// 자식 오브젝트 업데이트
	for (auto& pChild : m_ChildGameObjects)
	{
		if (pChild) pChild->Update();
	}
}
void msc_GameObject::Start()
{
	for (auto& pComponent : m_Components)
	{
		if (pComponent) pComponent->Start();
	}

	// 자식 오브젝트 시작
	for (auto& pChild : m_ChildGameObjects)
	{
		if (pChild) pChild->Start();
	}
}
void msc_GameObject::onDestroy()
{
	for (auto& pComponent : m_Components)
	{
		if (pComponent) pComponent->onDestroy();
	}

	// 자식 오브젝트 제거
	for (auto& pChild : m_ChildGameObjects)
	{
		if (pChild) pChild->onDestroy();
	}
}
void msc_GameObject::AddComponent(msc_Component* pComponent)
{
	if (pComponent)
	{
		m_Components.push_back(pComponent);
	}
}
void msc_GameObject::SetParent(msc_GameObject* pParent)
{
	if (m_pParentGameObject == pParent) return;

	// 기존 부모에서 제거
	if (m_pParentGameObject)
	{
		auto it = find(m_pParentGameObject->m_ChildGameObjects.begin(),
					   m_pParentGameObject->m_ChildGameObjects.end(),
					   this);
		if (it != m_pParentGameObject->m_ChildGameObjects.end())
		{
			m_pParentGameObject->m_ChildGameObjects.erase(it);
		}
	}

	m_pParentGameObject = pParent;

	// 새로운 부모에 추가
	if (m_pParentGameObject)
	{
		m_pParentGameObject->AddChild(this);
		// Transform 업데이트
		if (m_pTransform)
		{
			m_pTransform->m_bDirty = true;
		}
	}
}
void msc_GameObject::AddChild(msc_GameObject* pChild)
{
	if (pChild && find(m_ChildGameObjects.begin(), m_ChildGameObjects.end(), pChild) == m_ChildGameObjects.end())
	{
		m_ChildGameObjects.push_back(pChild);
	}
}