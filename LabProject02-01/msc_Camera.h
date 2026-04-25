#pragma once
#include "msc_Component.h"

class msc_Viewport;
class msc_Camera : public msc_Component
{
	public:
	msc_Camera(msc_GameObject* pParentObject);
	virtual ~msc_Camera();
	void Start() override;
	void Update() override;
	void onDestroy() override;

private:
	msc_Viewport* m_Viewport;

};

class msc_Viewport
{
public:
	msc_Viewport() {}
	virtual ~msc_Viewport() {}

	int	m_nLeft = 0;
	int m_nTop = 0;
	int	m_nWidth = 0;
	int	m_nHeight = 0;

	void SetViewport(int nLeft, int nTop, int nWidth, int nHeight);
};
