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
}
void msc_Transform::Update() 
{
}
void msc_Transform::onDestroy()
{
}