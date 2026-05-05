#pragma once

#include "Player.h"
#include "Scene.h"
#include "Timer.h"
#include "msc_D3D12RenderingEngine.h"

class CGameFramework
{
public:
	static CGameFramework& GetInstance()
	{
		static CGameFramework instance;
		return instance;
	}

	// 복사 방지
	CGameFramework(const CGameFramework&) = delete;
	CGameFramework& operator=(const CGameFramework&) = delete;

private:
	CGameFramework() { }

	HINSTANCE					m_hInstance = NULL;
	HWND						m_hWnd = NULL;

	bool						m_bActive = true;

	RECT						m_rcClient;

	std::unique_ptr<msc_D3D12RenderingEngine> m_pRenderingEngine = NULL; // Direct3D 12 렌더링 엔진
	//unique_ptr 스마트포인터 자주쓰기- 소멸자에서 자동으로 해제되므로 메모리 누수 방지에 도움


    HDC							m_hDCFrameBuffer = NULL;
    HBITMAP						m_hBitmapFrameBuffer = NULL;
    HBITMAP						m_hBitmapSelect = NULL;

	CPlayer*					m_pPlayer = NULL;
public:
	CScene*						m_pScene = NULL;
private:
	CGameObject*				m_pLockedObject = NULL;
public:
	CGameTimer					m_GameTimer;
private:
	POINT						m_ptOldCursorPos;

	_TCHAR						m_pszFrameRate[50];

public:
	void OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void SetActive(bool bActive) { m_bActive = bActive; }
};

