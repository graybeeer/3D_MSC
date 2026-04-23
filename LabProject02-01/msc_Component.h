#pragma once
#include "msc_GameObject.h"
class msc_GameObject;
class msc_Component {
	public:
	msc_Component();
	virtual ~msc_Component();
	virtual void Update() = 0;
	msc_Component* m_pParent = nullptr;
};