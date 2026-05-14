#pragma once
#include<list>
#include <memory>
#include "msc_Scene.h"
class msc_SceneManager {
public:
	msc_SceneManager();
	~msc_SceneManager();

	msc_Scene* main_scene;
	std::list<msc_Scene*> scenes; //

	void NewScene(std::string& name);
	void AddScene(msc_Scene* new_scene);
	//void SetMainScene();

	void BuildObjects();
	void ReleaseObjects();

	void Update();
	void Render(HDC hDCFrameBuffer, msc_Camera* pCamera);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};