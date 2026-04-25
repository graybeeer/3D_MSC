#pragma once
//엔진 시스템과 별개의 게임 매니저 클래스입니다. 게임 로직과 관련된 기능을 담당하는 클래스입니다.
// 게임의 시작, 종료, 레벨 관리, 점수 관리 등을 담당할 수 있습니다.
#include "stdafx.h"
#include "msc_Component.h"
class msc_GameManager : public msc_Component
{

public:
	static msc_GameManager* GetInstance(msc_GameObject* pParentObject) //싱글톤 
	{
		static msc_GameManager instance(pParentObject);
		return &instance;
	}
	msc_GameManager(const msc_GameManager&) = delete; // 복사 생성자 삭제
	msc_GameManager& operator=(const msc_GameManager&) = delete; // 대입 연산자 삭제

public:
	msc_GameManager(msc_GameObject* pParentObject);
	virtual ~msc_GameManager();
	void Start() override;
	void Update() override;
	void onDestroy() override;
};