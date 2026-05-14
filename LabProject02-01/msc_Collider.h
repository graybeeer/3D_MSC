#pragma once
#include "msc_Component.h"
class msc_Collider : public msc_Component
{
	public:
	msc_Collider(msc_GameObject* pParentObject);
	virtual ~msc_Collider();
	bool m_bIsTrigger = false; // 트리거 여부를 나타내는 멤버 변수입니다. 트리거는 충돌 감지 시 물리적 상호작용을 발생시키지 않고, 대신 이벤트를 발생시키는 콜라이더입니다. 이 변수를 통해 콜라이더가 트리거로 동작할지 여부를 설정할 수 있습니다. 트리거로 설정된 콜라이더는 다른 콜라이더와 충돌할 때 물리적 반응을 일으키지 않지만, OnCollisionEnter, OnCollisionStay, OnCollisionExit와 같은 이벤트를 발생시킬 수 있습니다.

	virtual void Start() override;
	virtual void Update() override;
	virtual void onDestroy() override;
	virtual void fixedUpdate() override;
	virtual void lateUpdate() override;	
};
class msc_BoxCollider : public msc_Collider
{
	BoundingOrientedBox m_xmBoundingBox; // m_xmBoundingBox는 msc_BoxCollider 클래스의 멤버 변수로, 이 콜라이더가 나타내는 3D 공간에서의 박스 형태의 경계 영역을 나타냅니다. BoundingOrientedBox는 중심점, 반지름, 그리고 회전을 포함하여 정의되며, 충돌 감지 및 물리적 상호작용에 사용됩니다. 이 변수를 통해 msc_BoxCollider는 자신의 위치, 크기, 그리고 방향에 따라 충돌을 감지할 수 있습니다.
	virtual void Start() override;
	virtual void Update() override;
	virtual void onDestroy() override;
	virtual void fixedUpdate() override;
	virtual void lateUpdate() override;
};
class msc_SphereCollider : public msc_Collider
{
	BoundingSphere m_xmBoundingSphere; // m_xmBoundingSphere는 msc_SphereCollider 클래스의 멤버 변수로, 이 콜라이더가 나타내는 3D 공간에서의 구 형태의 경계 영역을 나타냅니다. BoundingSphere는 일반적으로 중심점과 반지름을 포함하여 정의되며, 충돌 감지 및 물리적 상호작용에 사용됩니다. 이 변수를 통해 msc_SphereCollider는 자신의 위치와 크기에 따라 충돌을 감지할 수 있습니다.
	virtual void Start() override;
	virtual void Update() override;
	virtual void onDestroy() override;
	virtual void fixedUpdate() override;
	virtual void lateUpdate() override;
};