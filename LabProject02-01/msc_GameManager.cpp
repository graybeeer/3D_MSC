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

	msc_GameObject* pCubeObject = new msc_GameObject(string("TestCube"));
	pCubeObject->GetTransform()->SetLocalPosition(XMFLOAT3(0.0f, 0.0f, 10.0f));
	//pCubeObject->GetTransform()->SetLocalScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	// Mesh 추가
	pCubeObject->AddComponent<msc_CubeMesh_simple>();

}
void msc_GameManager::Update()
{
	//cout << "게임 매니저 업데이트" << endl;

}
void msc_GameManager::onDestroy()
{
	//테스트용 로그
	cout << "게임 매니저 onDestroy" << endl;
}