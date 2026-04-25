#pragma once
#include "stdafx.h"
#include <typeinfo>
#include <unordered_map>

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
    
    // ===== GetComponent 메서드 =====
    // 이름으로 컴포넌트 조회 (기존)
    msc_Component* GetComponent(string& strComponentType);
    
    // 템플릿으로 타입별 컴포넌트 조회 (NEW)
    template<derived_from<msc_Component> T>
    T* GetComponent()
    {
        // 컴포넌트 리스트 순회
        for (auto& pComponent : m_Components)
        {
            // typeid로 런타임 타입 정보 비교
            if (typeid(*pComponent) == typeid(T))
            {
                return static_cast<T*>(pComponent);
			}
        }
        return nullptr;
    }

    // ===== AddComponent 메서드 =====
    // 포인터로 직접 추가 
private:
    void AddComponent(msc_Component* pComponent);

public:
    // 템플릿으로 새 컴포넌트 생성 후 추가 (유니티처럼)
    // 템플릿은 component의 하위 클래스만 가능 
	// c++20 이상에서 derived_from 제약 조건 사용    
    template <derived_from<msc_Component> T>
    T* AddComponent()
    {
        // 중복 추가 방지: 같은 타입의 컴포넌트가 이미 있으면 반환
        T* existingComponent = GetComponent<T>();
        if (existingComponent)
        {
            return existingComponent;
        }

        // 새 컴포넌트 생성
        T* newComponent = new T(this);
        
        // 리스트에 추가
        m_Components.push_back(newComponent);
        
        // Transform 포인터 설정
        newComponent->m_pTransform = m_pTransform;
        
        // 즉시 Start 호출
        newComponent->Start();
        
        return newComponent;
    }

    // ===== RemoveComponent 메서드 =====

    template<derived_from<msc_Component> T>
    bool RemoveComponent()
    {
        for (auto it = m_Components.begin(); it != m_Components.end(); ++it)
        {
			if (typeid(**it) == typeid(msc_Transform)) continue; // Transform 컴포넌트는 제거 불가   
            if (typeid(**it) == typeid(T))
            {
                (*it)->onDestroy();
                delete *it;
                m_Components.erase(it);
                return true;
            }
        }
        return false;
    }

    // ===== 부모-자식 관계 메서드 =====
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