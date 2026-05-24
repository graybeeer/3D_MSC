#pragma once
#include "msc_GameObject.h"
#include "msc_Camera.h"

#include "d3d12.h"
#include "dxgi1_4.h"
#include "D3Dcompiler.h"
#include "DXGIDebug.h"
#include <wrl.h>
using namespace Microsoft::WRL;

class msc_Scene {
public:
	msc_Scene(std::string& name);
	~msc_Scene();

	//추가된 msc 클래스들은 유니티처럼 게임프레임워크-씬메니저-씬-게임오브젝트-컴포넌트 계층구조로 설계되어 있습니다.
public:
	std::string scene_name; //씬의 이름
	std::list<msc_GameObject*> gameObjects; // 게임 오브젝트 리스트
	msc_Camera* mainCamera = nullptr; //씬의 메인 카메라 역할을 하는 컴포넌트
	msc_GameObject* gameManagerObject = nullptr; //게임 매니저 역할을 하는 게임 오브젝트

public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void Update();
	virtual void Render(HDC hDCFrameBuffer, msc_Camera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, msc_Camera* pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	
	// 루트 시그니처
	ComPtr<ID3D12RootSignature> m_pd3dGraphicsRootSignature;
};