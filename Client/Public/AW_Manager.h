#pragma once

#include "Engine_Defines.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CAW_Manager
{
	DECLARE_SINGLETON(CAW_Manager)

private:
	CAW_Manager();
	~CAW_Manager();

private:
	enum SPAWN_TYPE { ENEMY, SIGN };
public:
	enum GAME_PROGRESS_STATE { START_CINEMA, USUAL, WARNING, BOSS_FIGHT, CINEMATIC };

public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);

	void Change_GameState(GAME_PROGRESS_STATE eState);
private:
	void Progress_Usual(_float fTimeDelta);
	void Progress_Warning(_float fTimeDelta);

	void Check_Chunks();
	void Clear_Spawned();
	void Spawn_Enemy();
	void Spawn_MiddleBoss();
	void Spawn_Sign();
	void Spawn_LastBoss();

	_bool Check_Spawnable() const;

	_float Get_NormalEnemyHP() const {
		return ceil(300.f * Get_ProgressRatio()) + 1.f;
	}

	_float Get_MiddleBossHP() const {
		return ceil(500.f * Get_ProgressRatio()) + 1.f;
	}

	void Spawn_Jeane();
	void Spawn_Dragon();

	void Active_SuperMode();
	void End_SuperMode();

	void Change_Water(_float fTimeDelta);
private:
	CGameInstance*									m_pGameInstance = { nullptr };
	class CVarious_Camera*							m_pCamera = { nullptr };
	class CBike_AW*									m_pPlayer = { nullptr };

	CGameObject*									m_pLastSpawnedObj = { nullptr };
	SPAWN_TYPE										m_eSpawnType = { ENEMY };
	class CUI_AbilityWindow_AW*						m_pAbilityUI = { nullptr };

	list<class CMap_Chunk*>							m_ChunkLists;
	list<class CEnemy_AW*>							m_EnemyLists;
	list<pair<class CSign_AW*, class CSign_AW*>>	m_SignLists;
	class CMiddle_Boss_AW*							m_pMiddleBoss = { nullptr };
	class CLast_Boss_AW*							m_pLastBoss = { nullptr };

	CGameObject*									m_pCinemaJumpBase = { nullptr };
	 
	vector<class CUI_Text_Ability*>					m_TextAbilityUIs = { nullptr };
private:
	GAME_PROGRESS_STATE			m_eProgressState = { USUAL };
	_float						m_fProgressTimeAcc = { 0.f };

	_float						m_fTotalDist = { 900.f };
	_float						m_fMoveDist = { 0.f };
	_float						m_fMoveSpeed = { 5.f };
	_float						m_fMiddleBoss_SpawnDist = { 80.f };// 0.f;
	_bool						m_bStop = { false };

	_float						m_fSpawnPos = { 50.f };
	_float						m_fSpawnOffset = { 0.f };
	_float2						m_fOffsetRange = { 10.f, 20.f };

	_float						m_fClearValue = { -5.f };

	_bool						m_bSuperMode = { false };
	_float						m_fSuperMode_Speed = { 40.f };
	_float						m_fSuperMode_Duration = { 5.f };
	
	class CWater*				m_pWater					= { nullptr };
	_float4						m_vOriginLightColor			= { 0.41f, 0.48f, 0.57f, 1.f };
	_float4						m_vOriginWaterColor			= { 0.07f, 0.26f, 1.1f, 1.f };
	_float4						m_vChangeLightColor			= { 0.11f, 0.24f, 0.65f, 1.f };
	_float4						m_vChangeWaterColor			= { 0.05f, 0.02f, 0.03f, 1.f };
	_bool						m_bChangingWater			= { false };
	_bool						m_bChangingWaterComplete	= { false };

	_float						m_fWaterChangingAcc			= { 0.f };
	_float						m_fWaterChangeTime			= { 5.f };

public:
	_float Get_MoveSpeed() const {
		return m_bSuperMode == true ? m_fSuperMode_Speed : m_fMoveSpeed;
	}

	class CEnemy_AW* Find_NearestEnemy(_float& fDist) const;

	class CBike_AW* Get_Player() const {
		return m_pPlayer;
	}

	_float Get_ProgressRatio() const {
		return m_fMoveDist / m_fTotalDist;
	}

	void Set_Stop(_bool bStop) {
		m_bStop = bStop;
	}

	_bool Is_Stop() const {
		return m_bStop;
	}

	_bool Is_SuperMode() const {
		return m_bSuperMode;
	}
	
	_bool Is_Cinematic() const {
		return m_eProgressState == CINEMATIC;
	}

	_bool Is_Boss_Stage() const {
		return m_eProgressState == BOSS_FIGHT;
	}


	void Add_MoveSpeed(_float fVal);
	void Active_AbilityWindow();
	void Adjust_Reward(REWARD_TYPE eType);
	void Adjust_Chest_Reward(CHEST_REWARD_TYPE eType);
	void Active_Ability_Text(wstring& wstrAbilityText);

private:
	HRESULT Ready_MapChunks();
	HRESULT Ready_Enemies();
	HRESULT Ready_Signs();
	HRESULT Ready_Water();
	HRESULT Ready_Sky();
	HRESULT Ready_AbilityTexts();

	void Release();

};

END


