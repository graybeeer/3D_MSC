#pragma once
#include "stdafx.h"
class msc_Transform;
class msc_Component;
class msc_GameObject
{
public:
	msc_GameObject(string& strName);
	~msc_GameObject();

	void Update();
	void Start();
	void onDestroy();
	string m_strName; // 게임 오브젝트 이름
	
	msc_Transform* GetTransform() { return m_pTransform; }
	msc_Component* GetComponent(string& strComponentType);
	void AddComponent(msc_Component* pComponent);
private:
	msc_Transform* m_pTransform = nullptr; // Transform component
	list<msc_Component*> m_Components; // 컴포넌트 리스트
	//자식 게임 오브젝트 리스트
	list<msc_GameObject*> m_ChildGameObjects;
	msc_GameObject* m_pParentGameObject = nullptr; // 부모 게임 오브젝트에 대한 포인터
};