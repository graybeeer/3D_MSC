#pragma once
#include "msc_GameObject.h"
class msc_GameObject;
class msc_Component {
public:
	msc_Component(msc_GameObject* pParentObject);
	virtual ~msc_Component();
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void onDestroy() = 0;
	virtual msc_Transform* GetTransform() { return m_pTransform; }

	msc_GameObject* m_pParentObject = nullptr; // 부모 게임 오브젝트에 대한 포인터
	msc_Transform* m_pTransform = nullptr; // Transform 컴포넌트에 대한 포인터
};