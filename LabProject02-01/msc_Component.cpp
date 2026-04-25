#include"stdafx.h"
#include "msc_Component.h"
#include "msc_GameObject.h"
msc_Component::msc_Component(msc_GameObject* pParentObject)
{
	//addComponent 템플릿에서 이미 부모 오브젝트와 Transform이 설정되어 있기 때문에, 여기서는 잉여.
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