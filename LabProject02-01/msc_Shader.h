#pragma once
#include "msc_Component.h"
class msc_Shader : public msc_Component {
	public:
	msc_Shader(msc_GameObject* pParentObject);
	virtual ~msc_Shader();
	virtual void Start() override;
	virtual void Update() override;
	virtual void onDestroy() override;
	virtual void fixedUpdate() override;
	virtual void lateUpdate() override;
};