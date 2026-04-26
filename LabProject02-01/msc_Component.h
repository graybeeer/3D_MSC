#pragma once
#include "msc_GameObject.h"

class msc_GameObject;
class msc_Transform;
class msc_Collider;

class msc_Component
{
public:
	msc_Component(msc_GameObject* pParentObject);
	virtual ~msc_Component();
	
	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void onDestroy() = 0;
	virtual void fixedUpdate() {} // 물리 업데이트용, 필요에 따라 오버라이드
	virtual void lateUpdate() {} // 업데이트 후 처리용, 필요에 따라 오버라이드
	virtual void OnCollisionEnter(msc_Collider* pOther) {} // 충돌 시작 시 호출, 필요에 따라 오버라이드
	virtual void OnCollisionStay(msc_Collider* pOther) {} // 충돌 지속 시 호출, 필요에 따라 오버라이드
	virtual void OnCollisionExit(msc_Collider* pOther) {} // 충돌 종료 시 호출, 필요에 따라 오버라이드

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