#include"stdafx.h"
#include "msc_SceneManager.h"
#include "msc_Scene.h"
msc_SceneManager::msc_SceneManager() {

}
msc_SceneManager::~msc_SceneManager() {

}
void msc_SceneManager::BuildObjects() {
	main_scene = new msc_Scene(std::string("test scene"));
	scenes.push_back(main_scene);

	main_scene->BuildObjects();
}
void msc_SceneManager::ReleaseObjects() {
	main_scene->ReleaseObjects();
}
void msc_SceneManager::NewScene(std::string& name) {

}
void msc_SceneManager::AddScene(msc_Scene* new_scene) {

}
void msc_SceneManager::Update() {
	main_scene->Update();
}
void msc_SceneManager::Render(HDC hDCFrameBuffer, msc_Camera* pCamera) {
	main_scene->Render(hDCFrameBuffer, pCamera);
}
void msc_SceneManager::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	main_scene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
}
void msc_SceneManager::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	main_scene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
}