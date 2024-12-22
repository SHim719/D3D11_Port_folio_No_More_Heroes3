#pragma once

#include "Engine_Defines.h"

namespace Tool
{
}

namespace Engine
{
	class CBone;
	class CTransform;
	class CGameObject;
}


typedef struct tagToolCinematicDescs
{
	typedef map<_float, pair<_float3, _bool>> CINE_FRAMES;
	typedef map<_float, CINE_EVENT> CINE_EVENTS;

	typedef CINE_FRAMES::iterator CINE_FRAMES_ITER;
	typedef CINE_EVENTS::iterator CINE_EVENTS_ITER;

	string				strActorTag = "";
	CINE_FRAMES			Positions; // 키 값: 시간초, 밸류 값: 위치값, 보간 여부(선형보간)
	CINE_FRAMES			Looks;
	CINE_FRAMES			Scales;
	CINE_EVENTS			Events;
	size_t				EventPlayed;
} TOOL_CINEMATIC_DESCS;


namespace HR
{
	using ATTACH_DESC = struct tagAttach_desc
	{
		Engine::CBone* pBone = { nullptr };
		Engine::CTransform* pParentTransform = { nullptr };

		char		szBoneName[MAX_PATH] = { "" };
		XMFLOAT3	vOffSet = { 0.f,0.f,0.f };
		bool		bOnlyPosition = { true };
		bool		bWorldOffset = { false };

		Engine::CBone* pTargetBone = { nullptr };
		char		   szTargetBoneName[MAX_PATH] = { "" };
	};

	using Tick_Buffer = struct tagTick_Buffer
	{
		float		fTimeDelta = { 0.f };
		XMFLOAT3	vTargetPoint = { 0.f,0.f,0.f };

		BOOL		bEndStop = { false };
		BOOL		bStoped = { false };
		BOOL		bChangeDir = { false };
		int			iDispatchX = { 0 };

		XMFLOAT4X4  AttachMatrix = {};
	};

	// 2 :: For Update SaveFile

	using EFFECT_COMMON_DESC = struct tagEffect_Common_Desc
	{
		unsigned int	iTransformType = { 0 };
		unsigned int	iRenderGroup = { 1 };
		int				iPassIdx = { 0 };

		bool			bFllowOnlyPos = { true };
		bool			bEffectLoop = { false };
		bool			bLerpSize = { false };

		XMFLOAT3		vStartPosMin = { 0.f, 0.f ,0.f };
		XMFLOAT3		vStartPosMax = { 0.f, 0.f ,0.f };

		XMFLOAT2		vSpeedMinMax = { 0.f, 1.f };
		float			fSpeedOverLifeTime = { 0.f };

		unsigned int    iSpeedLerpRatioNum = { 0 };
		XMFLOAT3		vSpeedDirMin = { -1.f, -1.f, -1.f };
		XMFLOAT3		vSpeedDirMax = { 1.f, 1.f, 1.f };

		bool			bEnableColorOverLife = { false };
		XMFLOAT4		vStartColorMin = { 0.f,0.f,0.f,0.f };
		XMFLOAT4		vStartColorMax = { 0.f,0.f,0.f,0.f };
		XMFLOAT4		vColorOverLife = { 0.f,0.f,0.f,0.f };
		XMFLOAT4		vClipColor = { 0.f,0.f,0.f,0.f };

		XMFLOAT4		vMultipleColor = { 1.f,1.f,1.f,1.f };
		XMFLOAT4		vAddColor = { 0.f,0.f,0.f,0.f };

		XMFLOAT3		vStartRotationMin = { 0.f, 0.f, 0.f };
		XMFLOAT3		vStartRotationMax = { 0.f, 0.f, 0.f };
		XMFLOAT3		vRotationOverLife = { 0.f, 0.f, 0.f };
		bool			bRotationLocal = { false };

		XMFLOAT3		vPowerDirMin = { -1.f, -1.f, -1.f };
		XMFLOAT3		vPowerDirMax = { 1.f, 1.f, 1.f };

		float			fLifeTimeMin = { 1.f };
		float			fLifeTimeMax = { 1.f };
		float			fTotalPlayTime = { 1.f };

		float			fStartDelayMin = { 0.f };
		float			fStartDelayMax = { 0.f };

		bool			bUseDistortion = { false };
		float			fDistortionPower = { false };

		bool			bDissolveReverse = false;
		float			fDissolveSpeed = 0.f;
		float			fDissolveStartTime = 0.f;

		bool			bUseGreaterThan_U = { false };
		bool			bUseGreaterThan_V = { false };
		bool			bUseLessThan_U = { false };
		bool			bUseLessThan_V = { false };

		float			fGreaterCmpVal_U = { 1.f };
		float			fGreaterCmpVal_V = { 1.f };
		float			fLessCmpVal_U = { 0.f };
		float			fLessCmpVal_V = { 0.f };

		float			fClipFactor = { 0.f };

		bool			bBaseSprite = { false };
		int				iBaseSpriteMaxRow = { 0 };
		int				iBaseSpriteMaxCol = { 0 };
		bool			ChangeBaseSprite = { false };
		float			fBaseSpriteChangeTime = { 0.f };

		bool			bMaskSprite = { false };
		int				iMaskSpriteMaxRow = { 0 };
		int				iMaskSpriteMaxCol = { 0 };
		bool			ChangeMaskSprite = { false };
		float			fMaskSpriteChangeTime = { 0.f };

		bool			bProportionalAlphaToSpeed = { false };
		float			fProportionalAlphaMaxSpeed = { 1.f };

		bool			bNoiseMultiplyAlpha = { true };

		int				iDissolveFlag = { 0 };
		XMFLOAT4		vEdgeColor = { 0.f,0.f ,0.f ,0.f };
		float			fStopDissolveAmount = { 0.f };

		float			fDissolveLoopMin = { 0.f };
		float			fDissolveLoopMax = { 1.f };

		int				iDirDissolveFlag = { 0 };
		float			fDissolveEdgeWidth = { 0.05f };
	};

	using COMMON_EFFECT_BUFFER = struct tagCommon_Buffer
	{
		unsigned int	iTransformType = { 0 };
		unsigned int	iTextureFlag = { 0 };
		BOOL			bEffectLoop = { false };
		BOOL			bLerpSize = { false };

		float			fSpeedOverLifeTime = { 0.f };
		BOOL			bEnableColorOverLife = { false };
		XMFLOAT2		vPadding0 = { 0.f,0.f };

		XMFLOAT3		vStartPosMin = { 0.f, 0.f ,0.f };
		float			fPadding1 = { 0.f };

		XMFLOAT3		vStartPosMax = { 0.f, 0.f ,0.f };
		float			fPadding2 = { 0.f };

		XMFLOAT4		vColorOverLife = { 0.f, 0.f, 0.f, 0.f };

		XMFLOAT3		vRotationOverLife = { 0.f, 0.f, 0.f };
		BOOL			bRotationLocal = { false };


		float			fTotalPlayTime = { 1.f };
		BOOL			bDissolveReverse = false;
		float			fDissolveSpeed = 0.f;
		float			fDissolveStartTime = 0.f;

		BOOL			bBaseSprite = { false };
		int				iBaseSpriteMaxRow = { 0 };
		int				iBaseSpriteMaxCol = { 0 };
		BOOL			ChangeBaseSprite = { false };

		float			fBaseSpriteChangeTime = { 0.f };
		BOOL			bMaskSprite = { false };
		int				iMaskSpriteMaxRow = { 0 };
		int				iMaskSpriteMaxCol = { 0 };

		BOOL			ChangeMaskSprite = { false };
		float			fMaskSpriteChangeTime = { 0.f };
		XMFLOAT2		vPadding3 = { 0.f,0.f };
	};

	using EFFECT_COMMON_UV_DESC = struct tagEffect_Common_Uv_Desc
	{
		XMFLOAT2		vStartBaseUVOffset = { 0.f, 0.f };
		XMFLOAT2		vBaseUVSpeed = { 0.f, 0.f };
		XMFLOAT2		vMinBaseUVOffset = { 0.f, 0.f };
		XMFLOAT2		vMaxBaseUVOffset = { 1.f, 1.f };
		int				iBaseSampler = { 0 };

		XMFLOAT2		vStartAlphaMaskUVOffset = { 0.f, 0.f };
		XMFLOAT2		vAlphaMaskUVSpeed = { 0.f, 0.f };
		XMFLOAT2		vMinAlphaMaskUVOffset = { 0.f, 0.f };
		XMFLOAT2		vMaxAlphaMaskUVOffset = { 1.f, 1.f };
		int				iAlphaMaskSampler = { 0 };

		XMFLOAT2		vStartColorScaleUVOffset = { 0.f, 0.f };
		XMFLOAT2		vColorScaleUVSpeed = { 0.f, 0.f };
		XMFLOAT2		vMinColorScaleUVOffset = { 0.f, 0.f };
		XMFLOAT2		vMaxColorScaleUVOffset = { 1.f, 1.f };
		int				iColorScaleUVSampler = { 0 };

		XMFLOAT2		vStartNoiseUVOffset = { 0.f, 0.f };
		XMFLOAT2		vNoiseUVSpeed = { 0.f, 0.f };
		XMFLOAT2		vMinNoiseUVOffset = { 0.f, 0.f };
		XMFLOAT2		vMaxNoiseUVOffset = { 1.f, 1.f };
		int				iNoiseSampler = { 0 };

		XMFLOAT2		vStartDissolveUVOffset = { 0.f, 0.f };
		XMFLOAT2		vDissolveUVSpeed = { 0.f, 0.f };
		XMFLOAT2		vMinDissolveUVOffset = { 0.f, 0.f };
		XMFLOAT2		vMaxDissolveUVOffset = { 1.f, 1.f };
		int				iDissolveSampler = { 0 };

		XMFLOAT2		vStartBaseNoiseUVOffset = { 0.f, 0.f };
		XMFLOAT2		vBaseNoiseUVSpeed = { 0.f, 0.f };
		XMFLOAT2		vMinBaseNoiseUVOffset = { 0.f, 0.f };
		XMFLOAT2		vMaxBaseNoiseUVOffset = { 1.f, 1.f };
		int				iBaseNoiseSampler = { 0 };

		XMFLOAT2		vStartAlphaMaskNoiseUVOffset = { 0.f, 0.f };
		XMFLOAT2		vAlphaMaskNoiseUVSpeed = { 0.f, 0.f };
		XMFLOAT2		vMinAlphaMaskNoiseUVOffset = { 0.f, 0.f };
		XMFLOAT2		vMaxAlphaMaskNoiseUVOffset = { 1.f, 1.f };
		int				iAlphaMaskNoiseSampler = { 0 };

		XMFLOAT2		vStartDistortionUVOffset = { 0.f, 0.f };
		XMFLOAT2		vDistortionUVSpeed = { 0.f, 0.f };
		XMFLOAT2		vMinDistortionUVOffset = { 0.f, 0.f };
		XMFLOAT2		vMaxDistortionUVOffset = { 1.f, 1.f };
		int				iDistortionSampler = { 0 };

		XMFLOAT2		vStartUVSliceClipUVOffset = { 0.f, 0.f };
		XMFLOAT2		vUVSliceClipUVSpeed = { 0.f, 0.f };
		XMFLOAT2		vMinUVSliceClipUVOffset = { 0.f, 0.f };
		XMFLOAT2		vMaxUVSliceClipUVOffset = { 1.f, 1.f };
		int				iUVSliceClipSampler = { 0 };

		int				iRadialMappingFlag = { 0 };

		XMFLOAT3		vBaseNoiseUVInfoFlag = { 0.f,0.f,0.f };
		XMFLOAT3		vAlphaNoiseUVInfoFlag = { 0.f,0.f,0.f };

		XMFLOAT2		vBaseNoiseRadiusFlag = { 0.f, 0.f };
		XMFLOAT2		vAlphaNoiseRadiusFlag = { 0.f, 0.f };
 		XMFLOAT2		vNoise_RadiusFlag = { 0.f, 0.f };
		XMFLOAT2		vDistortion_RadiusFlag = { 0.f, 0.f };

	};

	using PARTICLE_DESC = struct tagParticle_Desc
	{
		float	 fSpeed = { 1.f };
		XMFLOAT3 vSpeedDir = { 0.f,0.f,0.f };
		XMFLOAT3 vPowerDir = { 0.f,0.f,0.f };
		XMFLOAT3 vRot = { 0.f,0.f,0.f };
		float	 fStartDelay = { 0.f };
	};

	using PARTICLE_DESC_TEST = struct tagParticle_Desc_Test
	{
		float	 fSpeed = { 1.f };
		XMFLOAT3 vSpeedDir = { 0.f,0.f,0.f };

		XMFLOAT3 vPowerDir = { 0.f,0.f,0.f };
		float	 fStartDelay = { 0.f };

		XMFLOAT3 vRot = { 0.f,0.f,0.f };
		float    fDissolveTime = { 0.f };

		XMFLOAT3 vAfterStop_Dir = { 0.f,0.f,0.f };
		float	 fMoveDist = { 0.f };
		
		float   fSpriteChangeTime = { 0.f };
		float	fSpriteTimeAcc = { 0.f };
		BOOL	bDelayEnd = { FALSE };
		float	fPadding = { 0.f};
	};

	using PARTICLE_INIT_DESC = struct tagParticle_Init_Desc
	{

		unsigned int	iNumInstance = { 10 };
		unsigned int	eParticleMode = { 0 };
		bool			bParticleLoop = { false };
		bool			bisSquare = { false };

		XMFLOAT2		vStartSizeMin = { 0.1f, 0.1f };
		XMFLOAT2		vStartSizeMax = { 0.5f, 0.5f };

		XMFLOAT2		vSizeOverLife = { 0.f, 0.f };
		XMFLOAT2		vPadding0 = { 0.f, 0.f };

		XMFLOAT3		vRotAxis = { 0.f,0.f,0.f };
		float			fRotSpeed = { 0.f};

		bool			bBillboardWithoutUp = { false };
		float			fStretchFactor = { 0.f };
		XMFLOAT2		vPadding1 = { 0.f, 0.f };

		XMFLOAT3		vLightDir = { 0.f,0.f,0.f };
		float			fPadding2 = { 0.f };

		XMFLOAT4		vLightAmbient = { 0.f,0.f,0.f,0.f };

		bool			bHasTargetPoint = { false };
		bool			bTargetWorldOffSet = { false };
		XMFLOAT2		vPadding2 = { 0.f, 0.f };

		XMFLOAT3		vTargetPointOffset = { 0.f,0.f,0.f };
		float			fMargin = { 0.f };

		bool			bFollowTargetPos = { true };
		bool			bTargetLifeZero = { true };
		bool			bXStretch = { true };
		bool			bYStretch = { false };

		bool			bStopParticle = { false };
		unsigned int	iStopType = { 0 };
		float			fStopTime = { 0.f };
		float			fStopDistance = { 0.f };

		float			fMaintainTime = { 0.f };
		bool			bChangeDirAfterStop = { false };
		XMFLOAT2		vPadding3 = { 0.f, 0.f };

		XMFLOAT3		vAfterDirMin = { 0.f,0.f,0.f };
		float			fPadding4 = { 0.f };
		XMFLOAT3		vAfterDirMax = { 0.f,0.f,0.f };
		float			fPadding5 = { 0.f };

		float			fSpriteChangeTolerance = { 0.f };
		XMFLOAT3		vPadding6  = { 0.f,0.f,0.f };
	};

	using PARTICLE_INIT_DESC_TEST = struct tagParticle_Init_DescTest
	{

		unsigned int	iNumInstance = { 10 };
		unsigned int	eParticleMode = { 0 };
		BOOL			bParticleLoop = { false };
		BOOL			bisSquare = { false };

		XMFLOAT2		vStartSizeMin = { 0.1f, 0.1f };
		XMFLOAT2		vStartSizeMax = { 0.5f, 0.5f };

		XMFLOAT2		vSizeOverLife = { 0.f, 0.f };
		XMFLOAT2		vPadding0 = { 0.f, 0.f };

		XMFLOAT3		vRotAxis = { 0.f,0.f,0.f };
		float			fRotSpeed = { 0.f };

		BOOL			bBillboardWithoutUp = { false };
		float			fStretchFactor = { 0.f };
		XMFLOAT2		vPadding1 = { 0.f, 0.f };

		XMFLOAT3		vLightDir = { 0.f,0.f,0.f };
		float			fPadding2 = { 0.f };

		XMFLOAT4		vLightAmbient = { 0.f,0.f,0.f,0.f };

		BOOL			bHasTargetPoint = { false };
		BOOL			bTargetWorldOffSet = { false };
		XMFLOAT2		vPadding2 = { 0.f, 0.f };

		XMFLOAT3		vTargetPointOffset = { 0.f,0.f,0.f };
		float			fMargin = { 0.f };

		BOOL			bFollowTargetPos = { true };
		BOOL			bTargetLifeZero = { true };
		BOOL			bXStretch = { true };
		BOOL			bYStretch = { false };

		BOOL			bStopParticle = { false };
		unsigned int	iStopType = { 0 };
		float			fStopTime = { 0.f };
		float			fStopDistance = { 0.f };

		float			fMaintainTime = { 0.f };
		BOOL			bChangeDirAfterStop = { false };
		XMFLOAT2		vPadding3 = { 0.f, 0.f };

		XMFLOAT3		vAfterDirMin = { 0.f,0.f,0.f };
		float			fPadding4 = { 0.f };
		XMFLOAT3		vAfterDirMax = { 0.f,0.f,0.f };
		float			fPadding5 = { 0.f };

		float			fSpriteChangeTolerance = { 0.f };
		XMFLOAT3		vPadding6	 = { 0.f,0.f,0.f };
	};

	using MESH_DESC = struct tagMesh_Desc
	{
		bool		bLerpRotation = { false };
		float		fTurnSpeed = { 0.f };
		XMFLOAT4	vRotationAxis = { 0.f, 0.f, 0.f, 0.f };

		XMFLOAT3	vStartSizeMin = { 1.f,1.f,1.f };
		XMFLOAT3	vStartSizeMax = { 1.f,1.f,1.f };
		XMFLOAT3	vScaleOverLife = { 0.f,0.f,0.f };

		XMFLOAT3	vStartRotation = { 0.f,0.f,0.f };
		XMFLOAT3	vRotationOverLife = { 0.f,0.f,0.f };

		bool		bSizePhaseSecond = { false };
		float		fSecondPhaseTime = { 0.f };
		XMFLOAT3	vScaleOverLife_Secod = { 0.f,0.f,0.f };

		bool		bUseMaintainTime = { false };
		float		fMaintainStartTime = { 0.f };
		float		fMaintainTime = { 0.f };

		bool		bDiscardRotation = { false };
	};



}


