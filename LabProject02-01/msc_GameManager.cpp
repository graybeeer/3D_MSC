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
	cout << "게임 매니저 시작" << endl;

	pCubeObject = new msc_GameObject(string("TestCube"));
	cout << "큐브 객체 생성됨" << endl;
	pCubeObject->GetTransform()->SetLocalPosition(XMFLOAT3(0.0f, 10.0f, 10.0f));
	pCubeObject->GetTransform()->SetLocalScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	// Mesh 추가
	pCubeObject->AddComponent<msc_CubeMesh_simple>();
	cout << "큐브 메시 추가됨" << endl;
	
}
void msc_GameManager::Update()
{
	//cout << "게임 매니저 업데이트" << endl;
	if(CGameFramework::GetInstance().m_pScene->msc_MainCamera)
	{
		CGameFramework::GetInstance().m_pScene->msc_MainCamera->GetTransform()->LookAt(pCubeObject->GetTransform());
		cout<< "카메라 위치: " << CGameFramework::GetInstance().m_pScene->msc_MainCamera->GetTransform()->GetLocalPosition().x << ", "
			 << CGameFramework::GetInstance().m_pScene->msc_MainCamera->GetTransform()->GetLocalPosition().y << ", "
			<< CGameFramework::GetInstance().m_pScene->msc_MainCamera->GetTransform()->GetLocalPosition().z << endl;
		cout << CGameFramework::GetInstance().m_pScene->m_mscGameObjects.size() << "개의 게임 오브젝트 존재" << endl;
	}

}
void msc_GameManager::onDestroy()
{
	//테스트용 로그
	cout << "게임 매니저 onDestroy" << endl;
}