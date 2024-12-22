#include "Client_Functions.h"

#include <codecvt>
#include <locale>

#include "Bone.h"

#include <iostream>
#include <sstream>
#include <iomanip>

namespace Client
{
	wstring Convert_StrToWStr(const string& str)
	{
		wstring_convert<codecvt_utf8<_tchar>> converter;
		return converter.from_bytes(str);
	}

	string Convert_WStrToStr(const wstring& wstr)
	{
		wstring_convert<codecvt_utf8<_tchar>> converter;
		return converter.to_bytes(wstr);
	}

	wstring to_wstring_with_precision(float value, int precision) {
		std::wstringstream wss;
		wss << std::fixed << std::setprecision(precision) << value;
		return wss.str();
	}

	wstring Get_Path(const wstring& wstrFileFullPath)
	{
		size_t iPathLength = wstrFileFullPath.rfind(L'\\') + 1;
		return wstrFileFullPath.substr(0, iPathLength);
	}

	POINT Get_ScreenCursorPos(HWND hWnd)
	{
		POINT ptMouse = {};
		GetCursorPos(&ptMouse);
		ScreenToClient(hWnd, &ptMouse);

		return ptMouse;
	}

	void Set_CursorToCenter(HWND hWnd, _uint iWinCX, _uint iWinCY)
	{
		POINT ptCenter = { (_long)iWinCX >> 1, (_long)iWinCY >> 1 };
		ClientToScreen(hWnd, &ptCenter);
		SetCursorPos(ptCenter.x, ptCenter.y);
	}
}

namespace JoMath
{
	_matrix LookTo(_fvector vTargetLook)
	{
		_fvector		vLook = vTargetLook;
		_fvector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		_fvector		vUp = XMVector3Cross(vLook, vRight);

		return _matrix(
			vRight,
			vUp,
			vLook,
			XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	_matrix LookAt(_fvector vSrc, _fvector vDst)
	{
		_fvector		vLook = XMVector3Normalize(vDst - vSrc);
		_fvector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
		_fvector		vUp = XMVector3Cross(vLook, vRight);

		return _matrix(
			vRight,
			vUp,
			vLook,
			XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}

	_matrix OrientToDir(_fvector vDir)
	{
		_fvector		vRight = XMVector3Normalize(vDir);
		_fvector		vLook = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vRight);
		_fvector		vUp = XMVector3Cross(vLook, vRight);

		return _matrix(
			vRight,
			vUp,
			vLook,
			XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}


	_vector ToEulerAngle(_fvector vQuat, _bool bToDegree)
	{
		_float w = vQuat.m128_f32[3];
		_float x = vQuat.m128_f32[0];
		_float y = vQuat.m128_f32[1];
		_float z = vQuat.m128_f32[2];

		_float sqw = w * w;
		_float sqx = x * x;
		_float sqy = y * y;
		_float sqz = z * z;

		// rotation about x-axis
		_float pitch = asinf(2.0f * (w * x - y * z));
		// rotation about y-axis
		_float yaw = atan2f(2.0f * (x * z + w * y), (-sqx - sqy + sqz + sqw));
		// rotation about z-axis
		_float roll = atan2f(2.0f * (x * y + w * z), (-sqx + sqy - sqz + sqw));

		if (bToDegree)
		{
			pitch = XMConvertToDegrees(pitch);
			yaw = XMConvertToDegrees(yaw);
			roll = XMConvertToDegrees(roll);
		}

		return XMVectorSet(pitch, yaw, roll, 1.f);
	}
	_vector Slerp_TargetLook(_fvector vNowLook, _fvector vTargetLook, _float fRotRate)
	{
		_vector StartQuat = XMQuaternionRotationMatrix(JoMath::LookTo(vNowLook));
		_vector EndQuat = XMQuaternionRotationMatrix(JoMath::LookTo(vTargetLook));

		return XMQuaternionSlerp(StartQuat, EndQuat, fRotRate);
	}
	_vector Calc_GroundLook(_fvector vDst, _fvector vSrc)
	{
		return XMVector3Normalize(XMVectorSetY(vDst - vSrc, 0.f));
	}

	_vector Get_BoneWorldPos(Engine::CBone* pBone, _fmatrix ParentMatrix)
	{
		_matrix SocketMatrix = pBone->Get_CombinedTransformation();
		SocketMatrix *= ParentMatrix;
		return SocketMatrix.r[3];
	}

	_float Calc_AngleToTarget(_fvector vSrcPos, _fvector vDestPos, _fvector vSrcLook)
	{
		_vector vTargetLook = XMVector3Normalize(XMVectorSetY(vDestPos - vSrcPos, 0.f));
		_vector vNormalizedSrcLook = XMVector3Normalize(XMVectorSetY(vSrcLook, 0.f));

		_float fAngle = acosf(XMVector3Dot(vNormalizedSrcLook, vTargetLook).m128_f32[0]);

		_vector vCross = XMVector3Cross(vNormalizedSrcLook, vTargetLook);

		if (vCross.m128_f32[1] < 0.f)
			fAngle = -fAngle;

		return fAngle;
	}

	_float Lerp(_float fA, _float fB, _float fRatio)
	{
		fRatio = clamp(fRatio, 0.f, 1.f);
		return fA * (1.f - fRatio) + fB * fRatio;
	}
}

namespace JoRandom
{
	_int Random_Int(_int _iMin, _int _iMax)
	{
		std::random_device RD;
		std::mt19937 gen(RD());

		std::uniform_int_distribution<_int> dis(_iMin, _iMax);

		return dis(gen);
	}

	_float Random_Float(_float _fMin, _float _fMax)
	{
		std::random_device RD;
		std::mt19937 gen(RD());

		if (_fMin > _fMax)
			swap(_fMin, _fMax);

		std::uniform_real_distribution<_float> dis(_fMin, _fMax);

		return dis(gen);
	}

	_float2 Random_Float2(const _float2& vMin, const _float2& vMax)
	{
		return _float2(Random_Float(vMin.x, vMax.x), Random_Float(vMin.y, vMax.y));
	}

	_float3 Random_Float3(const _float3& vMin, const _float3& vMax)
	{
		return _float3(Random_Float(vMin.x, vMax.x), Random_Float(vMin.y, vMax.y), Random_Float(vMin.z, vMax.z));
	}

	_float4 Random_Float4(const _float4& vMin, const _float4& vMax)
	{
		return _float4(Random_Float(vMin.x, vMax.x), Random_Float(vMin.y, vMax.y), Random_Float(vMin.z, vMax.z), Random_Float(vMin.w, vMax.w));
	}
}

