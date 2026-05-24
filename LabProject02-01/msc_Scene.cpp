#include"stdafx.h"
#include "msc_Scene.h"
#include "msc_GameObject.h"
#include "msc_Component.h"
#include "msc_GameManager.h"
#include "msc_Transform.h"
#include "msc_Camera.h"
#include "msc_Mesh.h"
#include "msc_D3D12RenderingEngine.h"
#include "GameFramework.h"
msc_Scene::msc_Scene(std::string& name) {
	scene_name = name;
}
msc_Scene::~msc_Scene() {

}
void msc_Scene::BuildObjects() {
	gameManagerObject = new msc_GameObject(std::string("GameManager")); // 게임 매니저 역할을 하는 게임 오브젝트를 생성
	gameManagerObject->AddComponent<msc_GameManager>();  // 게임 매니저 컴포넌트를 추가
}
void msc_Scene::ReleaseObjects() {

}
void msc_Scene::Update() {
	for (auto& mscGameObject : gameObjects) mscGameObject->Update();
}
void msc_Scene::Render(HDC hDCFrameBuffer, msc_Camera* pCamera) {
	if (!pCamera) return;
	CGraphicsPipeline::SetViewport(&pCamera->GetViewport());
	//cout << "Viewport Set: " << pCamera->GetViewport().m_nWidth << "x" << pCamera->GetViewport().m_nHeight << endl;

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->GetViewPerspectiveProjectionMatrix());

	for (auto& mscGameObject : gameObjects) {
		if (mscGameObject->GetComponent<msc_Mesh>()) {
			mscGameObject->GetComponent<msc_Mesh>()->Render(hDCFrameBuffer, pCamera);
		}
	}
	//새로 추가하는 d3d12
	//RenderContext context=new RenderContext()

	// --- [공통 파이프라인 세팅] ---
	// 모든 메쉬가 공통으로 쓰는 Root Signature와 기본 PSO 세팅
	// context.cmdList->SetGraphicsRootSignature(pMainRootSig);
	// context.cmdList->SetPipelineState(pDefaultPSO);
	// context.cmdList->RSSetViewports(...);
	// context.cmdList->RSSetScissorRects(...);

	// --- [1단계: 전역 상수 버퍼 업데이트 및 세팅] ---
	// 1. CPU에서 GPU로 카메라/시간 데이터를 한 번만 복사합니다.
	//engine.UpdatePassConstantBuffer(mainCamera, t, dt);

	// 2. 루트 시그니처 바인딩 (모든 오브젝트가 이 데이터를 공유함)
	//cmdList->SetGraphicsRootSignature(pMainRootSig);

	// b1 (1번 슬롯)에 전역 데이터 장착!
	//cmdList->SetGraphicsRootConstantBufferView(1, engine.GetPassCBVAddress());
}
void msc_Scene::Render(ID3D12GraphicsCommandList* pd3dCommandList, msc_Camera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());
	pCamera->UpdateShaderVariables(pd3dCommandList);
}
void msc_Scene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void msc_Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			break;
		}
		case 'A':
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

}