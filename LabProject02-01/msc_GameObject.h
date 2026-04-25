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
	void fixedUpdate();
	void lateUpdate();

    string m_strName;

    msc_Transform* GetTransform() { return m_pTransform; }
    msc_Component* GetComponent(string& strComponentType);
    void AddComponent(msc_Component* pComponent);

    // 부모-자식 관계 메서드
    void SetParent(msc_GameObject* pParent);
    msc_GameObject* GetParent() const { return m_pParentGameObject; }
    void AddChild(msc_GameObject* pChild);
    list<msc_GameObject*>& GetChildren() { return m_ChildGameObjects; }

private:
    msc_Transform* m_pTransform = nullptr;
    list<msc_Component*> m_Components;
    list<msc_GameObject*> m_ChildGameObjects;
    msc_GameObject* m_pParentGameObject = nullptr;
};