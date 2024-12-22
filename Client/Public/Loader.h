#pragma once

#include "Client_Defines.h"
#include "Base.h"


BEGIN(Engine)
class CGameInstance;
END


class CLoader : public CBase
{
public:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	_bool isFinished() const {
		return m_bIsFinished;
	}

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	unsigned int Loading();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	HANDLE				m_hThread = 0;
	CRITICAL_SECTION	m_CriticalSection;
	LEVEL				m_eNextLevelID = LEVEL_END;

private:
	CGameInstance*		m_pGameInstance = { nullptr };
	_bool				m_bIsFinished = false;

private:	// Execute Only First Time.
	HRESULT Load_Level_MainMenu();
	HRESULT Ready_Prototypes_GameObject();

private:
	HRESULT Load_Level_World_Home();
	HRESULT Load_Level_World_Home_Lab();
	HRESULT Load_Level_Battle_Motel_Front();
	HRESULT Load_Level_World_City();
	HRESULT Load_Level_World_City_TrashCollector();
	HRESULT Load_Level_World_City_Toilet();
	HRESULT Load_Level_Battle_UFO_Outside();
	HRESULT Load_Level_World_UFO();
	HRESULT Load_Level_World_UFO_Toilet();
	HRESULT Load_Level_Battle_UFO_Inside();
	HRESULT Load_Level_Boss_BlackHole_Ground();
	HRESULT Load_Level_Boss_BlackHole_Space();
	HRESULT Load_Level_Boss_SonicJuice();
	HRESULT Load_Level_SmashBros_Boss_Damon();
	HRESULT Load_Level_Arrow_A_Row();
	HRESULT Load_Level_Mario();
	HRESULT Load_Level_Battle_Island();
	HRESULT Load_Level_Ending();

private:	// Execute Every Level Switching.
	HRESULT Load_Travis(_uint _iLevelIndex);
	HRESULT Load_Enemy(_uint _iLevelIndex);

	HRESULT Load_Travis_Bike();

	HRESULT Load_Resource(const wstring& strFilePath, const wstring& strNavigationPath = {});
	HRESULT Load_Map(const char* strMapPath[], _int iPathCount, const wstring& strNavigationPath = {});
	HRESULT Load_Sound(const char* strSoundPath[], _int iPathCount);

	HRESULT Load_UI();
	HRESULT Load_UI_Texture(const _tchar* wszPath);

	HRESULT Load_NPC(_uint _iLevelIndex);
	HRESULT Load_Effects_Prototype(LEVEL eLevel, const wstring& wstrEffectPath);

	HRESULT Load_Trashes();

private:
	vector<wstring>		Find_Models(const wstring& strFilePath);
	vector<wstring>		Find_Sounds(const wstring& strFilePath);

private:
	vector<wstring>		vModelNames;

private:
	_int				m_iMultiImageCnt = 0;

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

