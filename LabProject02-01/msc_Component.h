#pragma once
#include "msc_GameObject.h"

class msc_GameObject;
class msc_Transform;

class msc_Component
{
public:
	msc_Component(msc_GameObject* pParentObject);
	virtual ~msc_Component();
	
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void onDestroy() = 0;
	virtual msc_Transform* GetTransform() { return m_pTransform; }

	// 부모 오브젝트 접근
	msc_GameObject* GetGameObject() const { return m_pParentObject; }
	
	// 템플릿으로 다른 컴포넌트 접근 
	template<derived_from<msc_Component> T>
	T* GetComponent()
	{
		if (!m_pParentObject) return nullptr;
		return m_pParentObject->GetComponent<T>();
	}

	msc_GameObject* m_pParentObject = nullptr;
	msc_Transform* m_pTransform = nullptr; //부모 오브젝트의 Transform
};