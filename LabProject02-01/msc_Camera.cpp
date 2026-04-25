#include "stdafx.h"
#include "msc_GameObject.h"
#include "msc_Transform.h"
#include "msc_Component.h"
#include "msc_Camera.h"
msc_Camera::msc_Camera(msc_GameObject* pParentObject) : msc_Component(pParentObject)
{
}

msc_Camera::~msc_Camera()
{
}

void msc_Camera::Start()
{
}

void msc_Camera::Update()
{
}

void msc_Camera::onDestroy()
{
}