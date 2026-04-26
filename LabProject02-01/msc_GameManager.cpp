#include "stdafx.h"
#include "msc_GameManager.h"
#include "msc_GameObject.h"	
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