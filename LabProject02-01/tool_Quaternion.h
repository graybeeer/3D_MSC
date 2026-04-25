#pragma once
#include "stdafx.h"
//쿼터니언 함수들

class tool_Quaternion
{
	public:
	static XMVECTOR Euler(float pitch, float yaw, float roll); // Euler 각도를 쿼터니언으로 변환
	static XMVECTOR Euler(const XMFLOAT3& eulerAngles); // 오버로드: XMFLOAT3로 Euler 각도 입력받아 쿼터니언으로 변환
};
