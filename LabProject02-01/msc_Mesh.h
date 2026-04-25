#pragma once
#include "msc_Component.h"
class msc_Mesh : public msc_Component
{
	public:
	msc_Mesh(msc_GameObject* pParentObject);
	virtual ~msc_Mesh();
	// 추상 클래스 구현
	void Update() override;
	void Start() override;
	void onDestroy() override;
};