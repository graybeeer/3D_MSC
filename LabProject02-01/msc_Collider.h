#pragma once
#include "msc_Component.h"
class msc_Collider : public msc_Component
{
	public:
	msc_Collider(msc_GameObject* pParentObject);
	virtual ~msc_Collider();
	virtual void Start() override;
	virtual void Update() override;
	virtual void onDestroy() override;
	virtual void fixedUpdate() override;
	virtual void lateUpdate() override;	
};
class msc_BoxCollider : public msc_Collider
{
	virtual void Start() override;
	virtual void Update() override;
	virtual void onDestroy() override;
	virtual void fixedUpdate() override;
	virtual void lateUpdate() override;
};
class msc_SphereCollider : public msc_Collider
{
	virtual void Start() override;
	virtual void Update() override;
	virtual void onDestroy() override;
	virtual void fixedUpdate() override;
	virtual void lateUpdate() override;
};