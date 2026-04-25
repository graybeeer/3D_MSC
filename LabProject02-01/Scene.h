#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Player.h"
#include "msc_GameObject.h"

class CScene
{
public:
	CScene(CPlayer *pPlayer);
	virtual ~CScene();

private:
	int							m_nObjects = 0;
	CGameObject					**m_ppObjects = NULL;

	CWallsObject*				m_pWallsObject = NULL;

	CPlayer*					m_pPlayer = NULL;

	//추가- msc클래스들
	//추가된 msc 클래스들은 유니티처럼 게임프레임워크-씬-게임오브젝트-컴포넌트 계층구조로 설계되어 있습니다.
	list<msc_GameObject*>		m_mscGameObjects; // 게임 오브젝트 리스트

#ifdef _WITH_DRAW_AXIS
	CGameObject*				m_pWorldAxis = NULL;
#endif

public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();

	void CheckObjectByObjectCollisions();
	void CheckObjectByWallCollisions();
	void CheckPlayerByWallCollision();
	void CheckObjectByBulletCollisions();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
};

