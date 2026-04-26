#include "stdafx.h"
#include "msc_GameObject.h"
#include "msc_Transform.h"
#include "msc_Component.h"
#include "GameFramework.h"

msc_GameObject::msc_GameObject(string& strName)
{
	m_strName = strName;

	m_pTransform = new msc_Transform(this);
	AddComponent(m_pTransform);
	//AddComponent<msc_Transform>(); // 템플릿 버전(일단은 여기선 사용 안함)

	CGameFramework::GetInstance().m_pScene->m_mscGameObjects.push_back(this); //씬의 게임 오브젝트 리스트에 자신 추가

	Start();
}

msc_GameObject::~msc_GameObject()
{
	onDestroy();

	for (auto& pComponent : m_Components)
	{
		if (pComponent) delete pComponent;
	}
	m_Components.clear();

	for (auto& pChild : m_ChildGameObjects)
	{
		if (pChild) delete pChild;
	}
	m_ChildGameObjects.clear();
}

void msc_GameObject::Update()
{
	// Transform 먼저 업데이트
	if (m_pTransform)
	{
		m_pTransform->Update();
	}

	// 나머지 컴포넌트 업데이트
	for (auto& pComponent : m_Components)
	{
		if (pComponent && pComponent != m_pTransform)
		{
			pComponent->Update();
		}
	}

	// 자식 게임 오브젝트 업데이트
	for (auto& pChild : m_ChildGameObjects)
	{
		if (pChild)
		{
			pChild->Update();  // 자식의 Update는 다시 이 과정을 반복
		}
	}
}

void msc_GameObject::Start()
{
	// Transform 먼저 초기화
	if (m_pTransform)
	{
		m_pTransform->Start();
	}
	// 나머지 컴포넌트
	for (auto& pComponent : m_Components)
	{
		if (pComponent && pComponent != m_pTransform)
		{
			pComponent->Start();
		}
	}

	// 자식 초기화
	for (auto& pChild : m_ChildGameObjects)
	{
		if (pChild)
		{
			pChild->Start();
		}
	}
}

void msc_GameObject::onDestroy()
{
	for (auto& pComponent : m_Components)
	{
		if (pComponent) pComponent->onDestroy();
	}

	for (auto& pChild : m_ChildGameObjects)
	{
		if (pChild) pChild->onDestroy();
	}
}

void msc_GameObject::AddComponent(msc_Component* pComponent)
{
	//새로운 컴포넌트 추가 시 - 템플릿 AddComponent 사용 권장
	if (pComponent)
	{
		m_Components.push_back(pComponent);
		pComponent->m_pTransform = m_pTransform;
		pComponent->Start(); // Start 호출
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
		// Transform dirty flag 설정하여 다음 업데이트에서 월드 변환 재계산
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

msc_Component* msc_GameObject::GetComponent(string& strComponentType)
{
	for (auto& pComponent : m_Components)
	{
		if (typeid(*pComponent).name() == strComponentType)
		{
			return pComponent;
		}
	}
	return nullptr;
}