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
	msc_GameObject* pCameraObject = new msc_GameObject(std::string("TestCamera"));
	pCameraObject->AddComponent<msc_Camera>();

	pCubeObject = new msc_GameObject(std::string("TestCube"));
	std::cout << "큐브 객체 생성됨" << std::endl;
	pCubeObject->GetTransform()->SetLocalPosition(XMFLOAT3(0.0f, 5.0f, 10.0f));
	pCubeObject->GetTransform()->SetLocalScale(XMFLOAT3(0.1f, 0.1f, 0.1f));
	pCubeObject->AddComponent<msc_CubeMesh_simple>();
	std::cout << "큐브 메시 추가됨" << std::endl;

 	msc_GameObject* pCubeObject2 = new msc_GameObject(std::string("TestCube2"));
	std::cout << "큐브 객체 생성됨" << std::endl;
	pCubeObject2->GetTransform()->SetLocalPosition(XMFLOAT3(0.0f, 5.0f, 5.0f));
	pCubeObject2->GetTransform()->SetLocalScale(XMFLOAT3(2.0f, 2.0f, 2.0f));
	pCubeObject2->AddComponent<msc_CubeMesh_simple>();
	std::cout << "큐브 메시 추가됨" << std::endl;
	
}
void msc_GameManager::Update()
{
	//std::cout << "게임 매니저 업데이트" << std::endl;
	if(CGameFramework::GetInstance().m_pScene->msc_MainCamera)
	{
		CGameFramework::GetInstance().m_pScene->msc_MainCamera->GetTransform()->LookAt(pCubeObject->GetTransform());
		std::cout << "카메라 위치: " << CGameFramework::GetInstance().m_pScene->msc_MainCamera->GetTransform()->GetLocalPosition().x << ", "
			 << CGameFramework::GetInstance().m_pScene->msc_MainCamera->GetTransform()->GetLocalPosition().y << ", "
			<< CGameFramework::GetInstance().m_pScene->msc_MainCamera->GetTransform()->GetLocalPosition().z << std::endl;
		std::cout << CGameFramework::GetInstance().m_pScene->m_mscGameObjects.size() << "개의 게임 오브젝트 존재" << std::endl;
	}

}
void msc_GameManager::onDestroy()
{
	//테스트용 로그
	std::cout << "게임 매니저 onDestroy" << std::endl;
}