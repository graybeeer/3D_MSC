#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Player.h"
#include "msc_GameObject.h"
#include "msc_Camera.h"

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
public:
	list<msc_GameObject*>	m_mscGameObjects; // 게임 오브젝트 리스트
	msc_Camera* msc_MainCamera = nullptr; //추가예정- 씬의 메인 카메라 역할을 하는 컴포넌트
	msc_GameObject* msc_MainCameraObject = nullptr;
	msc_GameObject* msc_GameManagerObject = nullptr; //추가예정- 게임 매니저 역할을 하는 게임 오브젝트

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
	//msc 전용 update, render, input 처리 함수
	virtual void msc_BuildObjects();
	virtual void msc_ReleaseObjects();
	virtual void msc_Update();
	virtual void msc_Render(HDC hDCFrameBuffer, msc_Camera* pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);
};

