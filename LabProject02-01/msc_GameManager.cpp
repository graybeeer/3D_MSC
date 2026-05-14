#include "stdafx.h"
#include "msc_GameManager.h"
#include "msc_GameObject.h"	
#include "msc_Transform.h"
#include "msc_Mesh.h"
#include "GameFramework.h"

msc_GameManager::msc_GameManager(msc_GameObject* pParentObject) : msc_Component(pParentObject)
{
}
msc_GameManager::~msc_GameManager()
{
	
}
void msc_GameManager::Start()
{
	//테스트용 로그
	std::cout << "게임 매니저 시작" << endl;
	//메인 카메라 생성
	msc_GameObject* pCameraObject = new msc_GameObject(std::string("TestCamera"));
	pCamera = pCameraObject->AddComponent<msc_Camera>();


	pCubeObject = new msc_GameObject(std::string("TestCube"));
	std::cout << "큐브 객체 생성됨" << std::endl;
	pCubeObject->GetTransform()->SetLocalPosition(XMFLOAT3(0.0f, 0.0f, 15.0f));
	pCubeObject->GetTransform()->SetLocalScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	pCubeObject->AddComponent<msc_CubeMesh_simple>();
	std::cout << "큐브 메시 추가됨" << std::endl;

 	msc_GameObject* pCubeObject2 = new msc_GameObject(std::string("TestCube2"));
	std::cout << "큐브 객체 생성됨" << std::endl;
	pCubeObject2->GetTransform()->SetLocalPosition(XMFLOAT3(5.0f, 5.0f, 5.0f));
	pCubeObject2->GetTransform()->SetLocalScale(XMFLOAT3(4.0f, 4.0f, 4.0f));
	pCubeObject2->AddComponent<msc_CubeMesh_simple>();
	std::cout << "큐브 메시 추가됨" << std::endl;
	
}
void msc_GameManager::Update()
{
	//std::cout << "게임 매니저 업데이트" << std::endl;
	if(pCamera != nullptr)
	{
		pCamera->GetTransform()->LookAt(pCubeObject->GetTransform());
		std::cout << "카메라 위치: " << pCamera->GetTransform()->GetLocalPosition().x << ", "
			 << pCamera->GetTransform()->GetLocalPosition().y << ", "
			<< pCamera->GetTransform()->GetLocalPosition().z << std::endl;
		std::cout << CGameFramework::GetInstance().gf_SceneManager->main_scene->gameObjects.size() << "개의 게임 오브젝트 존재" << std::endl;
	}
	std::cout << CGameFramework::GetInstance().m_GameTimer.GetTimeElapsed() << std::endl; //델타 타임 출력
	pCubeObject->GetTransform()->RotateLocal(XMFLOAT3(0.0f, CGameFramework::GetInstance().m_GameTimer.GetTimeElapsed()*60, 0.0f));
}
void msc_GameManager::onDestroy()
{
	//테스트용 로그
	std::cout << "게임 매니저 onDestroy" << std::endl;
}