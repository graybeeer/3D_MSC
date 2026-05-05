#pragma once
#include "stdafx.h"
#include "tool_Quaternion.h"

XMVECTOR tool_Quaternion::Euler(float pitch, float yaw, float roll)
{
	return XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(pitch),
		XMConvertToRadians(yaw),
		XMConvertToRadians(roll)
	);
}	