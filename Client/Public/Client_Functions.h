#pragma once

#include "Engine_Defines.h"

namespace Engine
{
	class CBone;
}

namespace Client
{
   wstring Convert_StrToWStr(const string& str);
   string Convert_WStrToStr(const wstring& wstr);
   wstring to_wstring_with_precision(float value, int precision);
   wstring Get_Path(const wstring& wstrFileFullPath);
   POINT Get_ScreenCursorPos(HWND hWnd);
   void  Set_CursorToCenter(HWND hWnd, _uint iWinCX, _uint iWinCY);
};

namespace JoMath
{
	_matrix LookTo(_fvector vTargetLook);
	_matrix LookAt(_fvector vSrc, _fvector vDst);
	_matrix OrientToDir(_fvector vDir);

	_vector ToEulerAngle(_fvector vQuat, _bool bToDegree = true);

	_vector Slerp_TargetLook(_fvector vNowLook, _fvector vTargetLook, _float fRotRate);
	_vector Calc_GroundLook(_fvector vSrc, _fvector vDst);

	_vector Get_BoneWorldPos(Engine::CBone* pBone, _fmatrix ParentMatrix);

	_float Calc_AngleToTarget(_fvector vSrcPos, _fvector vDestPos, _fvector vSrcLook);

	_float Lerp(_float fA, _float fB, _float fRatio);
}

namespace JoRandom
{
	_int Random_Int(_int _iMin, _int _iMax);
	_float Random_Float(_float _fMin, _float _fMax);
	_float2 Random_Float2(const _float2& vMin, const _float2& vMax);
	_float3 Random_Float3(const _float3& vMin, const _float3& vMax);
	_float4 Random_Float4(const _float4& vMin, const _float4& vMax);
}