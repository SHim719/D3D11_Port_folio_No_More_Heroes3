#pragma once


#define KEY_DOWN(KEY) m_pGameInstance->GetKeyDown(KEY)
#define KEY_PUSHING(KEY) m_pGameInstance->GetKey(KEY)
#define KEY_UP(KEY) m_pGameInstance->GetKeyUp(KEY)
#define KEY_NONE(KEY) m_pGameInstance->GetKeyNone(KEY)


#define GET_CURLEVEL m_pGameInstance->Get_CurrentLevelID()
#define SET_CURLEVEL(LEVEL) m_pGameInstance->Set_CurrentLevelID(LEVEL)
#define ADD_EVENT(FUNC) m_pGameInstance->Add_Event(FUNC)
#define GET_PLAYER static_cast<CPlayer*>(m_pGameInstance->Find_GameObject(LEVEL_STATIC, L"Player"))
#define GET_CAMERA m_pGameInstance->Get_MainCamera()
#define PLAY_SOUND(SOUND, LOOP, VOLUME) m_pGameInstance->Play(SOUND, LOOP, VOLUME)

#define Get_Inst(TYPE) TYPE::Get_Instance()

#define UIMGR Get_Inst(CUI_Manager)
#define EFFECTMGR Get_Inst(CEffect_Manager_Tool)

#define CALC_TF m_pGameInstance->Get_CalculateTF()


#define YAXIS XMVectorSet(0.f, 1.f, 0.f, 0.f)
 

#define MINMAX(VARIABLE, MIN, MAX) VARIABLE = min(MAX, max(MIN, VARIABLE));

#define BASE				(1 << 1)
#define NORMALTEX			(1 << 2)
#define ALPHA_TEX			(1 << 3)
#define COLORSCALE_UV		(1 << 4)
#define NOISE				(1 << 5)
#define COLORSCALE			(1 << 6)
#define DISTORTIONTEX		(1 << 7)
#define DISSOLVE			(1 << 8)
#define BASE_NOISE_UV		(1 << 9)
#define ALPHA_NOISE_UV		(1 << 10)
#define UV_SLICE_CLIP		(1 << 11)

#define BASERADIAL			(2 << 1)
#define ALPHARADIAL			(2 << 2)
#define NOISERADIAL			(2 << 3)
#define DISTORTIONRADIAL	(2 << 4)

#define DISSOLVE_EDGE		 (4 << 1)
#define DISSOLVE_STOP		 (4 << 2)
#define DISSOLVE_LOOP		 (4 << 3)
#define DISSOLVE_DIRECTIONAL (4 << 4)

//00000011
//00001100
//00011000
