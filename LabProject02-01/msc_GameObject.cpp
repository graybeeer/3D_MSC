#include "stdafx.h"
#include "msc_GameObject.h"
#include "msc_Transform.h"
msc_GameObject::msc_GameObject(string& strName)
{
	m_strName = strName;
	m_pTransform = new msc_Transform(this);
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
}
void msc_GameObject::Update()
{
	for (auto& pComponent : m_Components)
	{
		if (pComponent) pComponent->Update();
	}
}
void msc_GameObject::Start()
{
	for (auto& pComponent : m_Components)
	{
		if (pComponent) pComponent->Start();
	}
}
void msc_GameObject::onDestroy()
{
	for (auto& pComponent : m_Components)
	{
		if (pComponent) pComponent->onDestroy();
	}
}
void msc_GameObject::AddComponent(msc_Component* pComponent)
{
	if (pComponent)
	{
		m_Components.push_back(pComponent);
	}
}