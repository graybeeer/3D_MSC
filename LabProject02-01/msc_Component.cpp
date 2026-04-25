#include"stdafx.h"
#include "msc_Component.h"
#include "msc_GameObject.h"
msc_Component::msc_Component(msc_GameObject* pParentObject)
{
	m_pParentObject = pParentObject;
	m_pTransform = pParentObject->GetTransform();
}
msc_Component::~msc_Component()
{
}
void msc_Component::onDestroy()
{
}
void msc_Component::Start()
{
}
void msc_Component::Update()
{
}