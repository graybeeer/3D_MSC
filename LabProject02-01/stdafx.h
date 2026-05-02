// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C의 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>

#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <list>
#include <string>
#include <typeinfo>
#include <iostream>

#define LegacyMode false // LegacyMode가 true이면 기존 방식, false이면 새로 추가한 것들로 구동

using namespace DirectX;
using namespace DirectX::PackedVector;


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DXGIDebug.h>


#define FRAMEBUFFER_WIDTH		640
#define FRAMEBUFFER_HEIGHT		480

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#define EXPLOSION_DEBRISES		240

#define RANDOM_COLOR			(0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX))

#define DegreeToRadian(x)		float((x)*3.141592654f/180.0f)

#define EPSILON					1.0e-6f


inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }

namespace Vector3
{
	inline DirectX::XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{
		DirectX::XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, xmvVector);
		return(xmf3Result);
	}

	inline DirectX::XMFLOAT3 ScalarProduct(DirectX::XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		DirectX::XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
		return(xmf3Result);
	}

	inline DirectX::XMFLOAT3 Add(const DirectX::XMFLOAT3& xmf3Vector1, const DirectX::XMFLOAT3& xmf3Vector2)
	{
		DirectX::XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}

	inline DirectX::XMFLOAT3 Add(DirectX::XMFLOAT3& xmf3Vector1, DirectX::XMFLOAT3& xmf3Vector2, float fScalar)
	{
		DirectX::XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
		return(xmf3Result);
	}

	inline DirectX::XMFLOAT3 Subtract(DirectX::XMFLOAT3& xmf3Vector1, DirectX::XMFLOAT3& xmf3Vector2)
	{
		DirectX::XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}

	inline float DotProduct(DirectX::XMFLOAT3& xmf3Vector1, DirectX::XMFLOAT3& xmf3Vector2)
	{
		DirectX::XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}

	inline DirectX::XMFLOAT3 CrossProduct(DirectX::XMFLOAT3& xmf3Vector1, DirectX::XMFLOAT3& xmf3Vector2, bool bNormalize = true)
	{
		DirectX::XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMVector3Normalize(XMLoadFloat3(&xmf3Vector1)), XMVector3Normalize(XMLoadFloat3(&xmf3Vector2)))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMVector3Normalize(XMLoadFloat3(&xmf3Vector1)), XMVector3Normalize(XMLoadFloat3(&xmf3Vector2))));
		return(xmf3Result);
	}

	inline DirectX::XMFLOAT3 Normalize(DirectX::XMFLOAT3& xmf3Vector)
	{
		DirectX::XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}

	inline float Length(DirectX::XMFLOAT3& xmf3Vector)
	{
		DirectX::XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(xmf3Result.x);
	}

	inline float Distance(DirectX::XMFLOAT3& xmf3Vector1, DirectX::XMFLOAT3& xmf3Vector2)
	{
		DirectX::XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMVectorSubtract(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
		return(xmf3Result.x);
	}

	inline float Angle(DirectX::XMVECTOR& xmvVector1, DirectX::XMVECTOR& xmvVector2)
	{
		DirectX::XMVECTOR xmvAngle = DirectX::XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
		return(DirectX::XMVectorGetX(DirectX::XMVectorACos(xmvAngle)));
	}

	inline float Angle(DirectX::XMFLOAT3& xmf3Vector1, DirectX::XMFLOAT3& xmf3Vector2)
	{
		return(Angle(DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&xmf3Vector1)), DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&xmf3Vector2))));
	}

	inline DirectX::XMFLOAT3 TransformNormal(DirectX::XMFLOAT3& xmf3Vector, DirectX::XMMATRIX& xmxm4x4Transform)
	{
		DirectX::XMFLOAT3 xmf3Result;
		DirectX::XMStoreFloat3(&xmf3Result, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&xmf3Vector), xmxm4x4Transform));
		return(xmf3Result);
	}

	inline DirectX::XMFLOAT3 TransformNormal(DirectX::XMFLOAT3& xmf3Vector, DirectX::XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformNormal(xmf3Vector, DirectX::XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}

	inline DirectX::XMFLOAT3 TransformCoord(DirectX::XMFLOAT3& xmf3Vector, DirectX::XMMATRIX& xmxm4x4Transform)
	{
		DirectX::XMFLOAT3 xmf3Result;
		DirectX::XMStoreFloat3(&xmf3Result, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&xmf3Vector), xmxm4x4Transform));
		return(xmf3Result);
	}

	inline DirectX::XMFLOAT3 TransformCoord(DirectX::XMFLOAT3& xmf3Vector, DirectX::XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformCoord(xmf3Vector, DirectX::XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}
}

namespace Vector4
{
	inline DirectX::XMFLOAT4 Add(DirectX::XMFLOAT4& xmf4Vector1, DirectX::XMFLOAT4& xmf4Vector2)
	{
		DirectX::XMFLOAT4 xmf4Result;
		DirectX::XMStoreFloat4(&xmf4Result, DirectX::XMLoadFloat4(&xmf4Vector1) + DirectX::XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}
}

namespace Matrix4x4
{
	inline DirectX::XMFLOAT4X4 Identity()
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMMatrixIdentity());
		return(xmmtx4x4Result);
	}

	inline DirectX::XMFLOAT4X4 Translate(float x, float y, float z)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMMatrixTranslation(x, y, z));
		return(xmmtx4x4Result);
	}

	inline DirectX::XMFLOAT4X4 Multiply(DirectX::XMFLOAT4X4& xmmtx4x4Matrix1, DirectX::XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMLoadFloat4x4(&xmmtx4x4Matrix1) * DirectX::XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline DirectX::XMFLOAT4X4 Multiply(DirectX::XMFLOAT4X4& xmmtx4x4Matrix1, DirectX::XMMATRIX& xmmtxMatrix2)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}

	inline DirectX::XMFLOAT4X4 Multiply(DirectX::XMMATRIX& xmmtxMatrix1, DirectX::XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * DirectX::XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline DirectX::XMFLOAT4X4 Multiply(DirectX::XMMATRIX& xmmtxMatrix1, DirectX::XMMATRIX& xmmtxMatrix2)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}

	inline DirectX::XMFLOAT4X4 RotationYawPitchRoll(float fPitch, float fYaw, float fRoll)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(fPitch), DirectX::XMConvertToRadians(fYaw), DirectX::XMConvertToRadians(fRoll)));
		return(xmmtx4x4Result);
	}

	inline DirectX::XMFLOAT4X4 RotationAxis(DirectX::XMFLOAT3& xmf3Axis, float fAngle)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&xmf3Axis), DirectX::XMConvertToRadians(fAngle)));
		return(xmmtx4x4Result);
	}

	inline DirectX::XMFLOAT4X4 Inverse(DirectX::XMFLOAT4X4& xmmtx4x4Matrix)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline DirectX::XMFLOAT4X4 Transpose(DirectX::XMFLOAT4X4& xmmtx4x4Matrix)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline DirectX::XMFLOAT4X4 PerspectiveFovLH(float fFovAngleY, float fAspectRatio, float fNearZ, float fFarZ)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fFovAngleY), fAspectRatio, fNearZ, fFarZ));
		return(xmmtx4x4Result);
	}

	inline DirectX::XMFLOAT4X4 LookAtLH(DirectX::XMFLOAT3& xmf3EyePosition, DirectX::XMFLOAT3& xmf3LookAtPosition, DirectX::XMFLOAT3& xmf3UpDirection)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&xmf3EyePosition), DirectX::XMLoadFloat3(&xmf3LookAtPosition), DirectX::XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}

	inline DirectX::XMFLOAT4X4 LookToLH(DirectX::XMFLOAT3& xmf3EyePosition, DirectX::XMFLOAT3& xmf3LookTo, DirectX::XMFLOAT3& xmf3UpDirection)
	{
		DirectX::XMFLOAT4X4 xmmtx4x4Result;
		DirectX::XMStoreFloat4x4(&xmmtx4x4Result, DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&xmf3EyePosition), DirectX::XMLoadFloat3(&xmf3LookTo), DirectX::XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}
}

namespace Triangle
{
	inline bool Intersect(DirectX::XMFLOAT3& xmf3RayPosition, DirectX::XMFLOAT3& xmf3RayDirection, DirectX::XMFLOAT3& v0, DirectX::XMFLOAT3& v1, DirectX::XMFLOAT3& v2, float& fHitDistance)
	{
		return(TriangleTests::Intersects(DirectX::XMLoadFloat3(&xmf3RayPosition), DirectX::XMLoadFloat3(&xmf3RayDirection), DirectX::XMLoadFloat3(&v0), DirectX::XMLoadFloat3(&v1), DirectX::XMLoadFloat3(&v2), fHitDistance));
	}
}

namespace Plane
{
	inline DirectX::XMFLOAT4 Normalize(DirectX::XMFLOAT4& xmf4Plane)
	{
		DirectX::XMFLOAT4 xmf4Result;
		DirectX::XMStoreFloat4(&xmf4Result, DirectX::XMPlaneNormalize(DirectX::XMLoadFloat4(&xmf4Plane)));
		return(xmf4Result);
	}
}
