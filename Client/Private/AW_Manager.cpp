#include "AW_Manager.h"
#include "Bike_AW.h"
#include "Various_Camera.h"

#include "GameInstance.h"

#include "Map_Chunk.h"
#include "Enemy_AW.h"
#include "Sign_AW.h"
#include "Middle_Boss_AW.h"
#include "Last_Boss_AW.h"

#include "UI_AbilityWindow_AW.h"

#include "PlayerBullet_AW.h"
#include "Dragon_Breath.h"

#include "Jeane_AW.h"
#include "Dragon_AW.h"

#include "Water.h"
#include "Effect_Manager.h"

#include "Cinematic_Manager.h"

#include "UI_Text_Ability.h"

IMPLEMENT_SINGLETON(CAW_Manager)


CAW_Manager::CAW_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

CAW_Manager::~CAW_Manager()
{
	Release();
}

HRESULT CAW_Manager::Initialize()
{
   m_pPlayer = static_cast<CBike_AW*>(m_pGameInstance->Add_Clone(LEVEL_ARROW_A_ROW, L"Layer_Player", L"Prototype_Bike_AW"));
	if (nullptr == m_pPlayer)
		return E_FAIL;
	Safe_AddRef(m_pPlayer);

	CVarious_Camera::VARCAM_DESC CameraDesc = {};
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;
	CameraDesc.fFovy = 60.f;
	CameraDesc.vAt = { 0.f, 0.f, 1.f, 1.f };
	CameraDesc.vEye = { 0.f, 2.f, -2.f, 1.f };
	CameraDesc.pOwner = m_pPlayer;

	m_pCamera = static_cast<CVarious_Camera*>(m_pGameInstance->Clone_GameObject(L"Prototype_VariousCamera", &CameraDesc));
	if (nullptr == m_pCamera)
		return E_FAIL;

	CINEMAMGR->Bind_CinemaEndFunc(bind(&CVarious_Camera::Change_State, static_cast<CVarious_Camera*>(m_pCamera), (_uint)VARCAM_STATES::STATE_AW_DEFAULT, nullptr));

	//m_pCamera->Change_State((_uint)VARCAM_STATES::STATE_AW_DEFAULT);

	m_pGameInstance->Change_MainCamera(m_pCamera);

	if (FAILED(Ready_MapChunks()))
		return E_FAIL;
	
	if (FAILED(Ready_Enemies()))
		return E_FAIL;
	
	if (FAILED(Ready_Signs()))
		return E_FAIL;

	if (FAILED(Ready_Water()))
		return E_FAIL;

	if (FAILED(Ready_Sky()))
		return E_FAIL;

	if (FAILED(Ready_AbilityTexts()))
		return E_FAIL;

	m_pGameInstance->Add_Clone(LEVEL_ARROW_A_ROW, L"Layer_UI", L"Prototype_UI_LevelProgress_AW");

	m_pAbilityUI = static_cast<CUI_AbilityWindow_AW*>(m_pGameInstance->Clone_GameObject(L"Prototype_UI_AbilityWindow_AW"));
	if (nullptr == m_pAbilityUI)
		return E_FAIL;

	Change_GameState(START_CINEMA);

	//CINEMAMGR->Bind_CinemaEndFunc(bind(&CAW_Manager::Change_GameState))

	return S_OK;
}

void CAW_Manager::Tick(_float fTimeDelta)
{
	if (true == m_bStop)
		return;

	if (CINEMATIC != m_eProgressState)
		Check_Chunks();

	switch (m_eProgressState)
	{
	case USUAL:
		Progress_Usual(fTimeDelta);
		break;
	case WARNING:
		Progress_Warning(fTimeDelta);
		break;
	}
	

	if (KEY_DOWN(eKeyCode::F4))
	{
		//ADD_EVENT(bind(&CAW_Manager::Active_AbilityWindow, this)); 
		End_SuperMode();
		Active_SuperMode();
	}

	if (KEY_DOWN(eKeyCode::F7))
	{
		Change_GameState(WARNING);
	}

	if (KEY_DOWN(eKeyCode::F6))
	{
		Change_GameState(CINEMATIC);
	}


}

void CAW_Manager::Progress_Usual(_float fTimeDelta)
{
	_float fMoveSpeed = m_bSuperMode ? m_fSuperMode_Speed : m_fMoveSpeed;

	if (Check_Spawnable())
	{
		if (m_fMiddleBoss_SpawnDist <= 0.f)
		{
			m_fMiddleBoss_SpawnDist = 80.f;
			Spawn_MiddleBoss();
		}
		else
		{
			switch (m_eSpawnType)
			{
			case ENEMY:
				Spawn_Enemy();
				break;
			case SIGN:
				Spawn_Sign();
				break;
			}
		}
	}

	Clear_Spawned();

	if (m_fMoveDist >= m_fTotalDist * 0.5f && false == m_bChangingWater && false == m_bChangingWaterComplete)
	{
		Ready_Sky();
		m_bChangingWater = true;
	}
	
	Change_Water(fTimeDelta);

	if (m_bSuperMode)
	{
		m_fSuperMode_Duration -= fTimeDelta;
		if (m_fSuperMode_Duration <= 0.f)
			End_SuperMode();
	}

	m_fMoveDist += fMoveSpeed * fTimeDelta;
	m_fMiddleBoss_SpawnDist -= m_fMoveSpeed * fTimeDelta;

	if (m_fMoveDist > m_fTotalDist - 30.f)
	{
		Change_GameState(WARNING);
	}

}

void CAW_Manager::Progress_Warning(_float fTimeDelta)
{
	m_fProgressTimeAcc += fTimeDelta;
	if (m_fProgressTimeAcc >= 3.f)
	{
		m_fProgressTimeAcc = 0.f;
		Change_GameState(BOSS_FIGHT);
	}

}


void CAW_Manager::Check_Chunks()
{
	if (m_ChunkLists.front()->Is_Exceed())
	{
		CMap_Chunk* pFrontChunk = m_ChunkLists.front();
		CMap_Chunk* pLastChunk = m_ChunkLists.back();
		pFrontChunk->Set_Offset(pLastChunk->Get_ChunkSize() + pLastChunk->Get_Offset());
		m_ChunkLists.pop_front();
		m_ChunkLists.emplace_back(pFrontChunk);
	}
}

void CAW_Manager::Clear_Spawned()
{
	if (m_EnemyLists.front()->Is_InLayer() && m_EnemyLists.front()->Get_Transform()->Get_Position().m128_f32[2] < m_fClearValue)
		m_EnemyLists.front()->Set_ReturnToPool(true);

	if (m_SignLists.front().first->Is_InLayer() && m_SignLists.front().first->Get_Transform()->Get_Position().m128_f32[2] < m_fClearValue)
	{
		m_SignLists.front().first->Set_ReturnToPool(true);
		m_SignLists.front().second->Set_ReturnToPool(true);
	}

	if (m_pMiddleBoss->Get_Transform()->Get_Position().m128_f32[2] < m_fClearValue)
		m_pMiddleBoss->Set_ReturnToPool(true);

}

void CAW_Manager::Spawn_Sign()
{
	if (true == m_SignLists.front().first->Is_InLayer())
		return;

	pair<CSign_AW*, CSign_AW*> pFront = m_SignLists.front();
	pFront.first->OnEnter_Layer(nullptr);
	m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Layer_Enemy", pFront.first);
	Safe_AddRef(pFront.first);

	pFront.first->Get_Transform()->Set_Position(XMVectorSet(-2.f, 2.f, m_fSpawnPos, 1.f));
	pFront.first->Get_Transform()->LookTo(XMVectorSet(0.f, 0.f, 1.f, 0.f));

	pFront.second->OnEnter_Layer(nullptr);
	m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Layer_Enemy", pFront.second);
	Safe_AddRef(pFront.second);
	pFront.second->Get_Transform()->Set_Position(XMVectorSet(2.f, 2.f, m_fSpawnPos, 1.f));
	pFront.second->Get_Transform()->LookTo(XMVectorSet(0.f, 0.f, 1.f, 0.f));

	m_SignLists.pop_front();
	m_SignLists.emplace_back(pFront);

	m_pLastSpawnedObj = pFront.first;

	m_fSpawnOffset = JoRandom::Random_Float(m_fOffsetRange.x, m_fOffsetRange.y);

	m_eSpawnType = ENEMY;
}

void CAW_Manager::Spawn_LastBoss()
{
	m_pLastBoss->OnEnter_Layer(nullptr);
	m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Layer_Enemy", m_pLastBoss);
	Safe_AddRef(m_pLastBoss);

	EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_BOSS_Surface_L", m_pPlayer->Get_EffectDescPtr());
	EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_BOSS_Surface_R", m_pPlayer->Get_EffectDescPtr());
	m_pLastBoss->Get_Transform()->Set_PositionZ(20.f);
}

void CAW_Manager::Spawn_Enemy()
{
	if (true == m_EnemyLists.front()->Is_InLayer())
		return;

	CEnemy_AW* pFront  = m_EnemyLists.front();
	pFront->OnEnter_Layer(nullptr);
	m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Layer_Enemy", pFront);
	Safe_AddRef(pFront);

	_float RandX = JoRandom::Random_Int(0, 1) == 1 ? -2.f : 2.f;

	pFront->Get_Transform()->Set_Position(XMVectorSet(RandX, 0.f, m_fSpawnPos, 1.f));

	m_EnemyLists.pop_front();
	m_EnemyLists.emplace_back(pFront);

	m_pLastSpawnedObj = pFront;

	pFront->Set_CurHp(Get_NormalEnemyHP());

	m_fSpawnOffset = JoRandom::Random_Float(m_fOffsetRange.x, m_fOffsetRange.y);

	m_eSpawnType = SIGN;
}

void CAW_Manager::Spawn_MiddleBoss()
{
	m_pMiddleBoss->Get_Transform()->Set_Position(XMVectorSet(0.f, 0.f, m_fSpawnPos, 1.f));

	m_pMiddleBoss->OnEnter_Layer(nullptr);
	m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Layer_Enemy", m_pMiddleBoss);
	Safe_AddRef(m_pMiddleBoss);

	m_pLastSpawnedObj = m_pMiddleBoss;

	m_pMiddleBoss->Set_CurHp(Get_MiddleBossHP());

	m_fSpawnOffset = JoRandom::Random_Float(m_fOffsetRange.x, m_fOffsetRange.y);
}

_bool CAW_Manager::Check_Spawnable() const
{
	if (nullptr == m_pLastSpawnedObj)
		return true;
	
	_bool bCheck = (m_fSpawnPos - m_pLastSpawnedObj->Get_Transform()->Get_Position().m128_f32[2] >= m_fSpawnOffset)
		&& m_fMoveDist < m_fTotalDist - 50.f;

	return bCheck;
}

HRESULT CAW_Manager::Ready_MapChunks()
{
	size_t iRoadCount = 10;
	_float fRoadScale = 0.5f;
	_float fRoadSize = 8.f;
	size_t iChunkCount = 3;

	CMap_Chunk::MAP_CHUNK_DESC Desc{};
	Desc.ObjInfos.resize(iRoadCount);
	CALC_TF->Set_WorldMatrix(XMMatrixIdentity());
	CALC_TF->Rotation(YAXIS, XM_PIDIV2);
	CALC_TF->Set_Scale({ 1.f, 1.f, fRoadScale });

	Desc.fChunkSize = fRoadSize * iRoadCount;
	Desc.fThreshold = -10.f;
	
	for (size_t i = 0; i < iChunkCount; ++i) {
		CALC_TF->Set_Position(XMVectorSet(0.f, 0.f, -fRoadSize, 1.f));
		Desc.fOffset = Desc.fChunkSize * (_float)i;
		for (size_t j = 0; j < iRoadCount; ++j)
		{
			CALC_TF->Add_Position(XMVectorSet(0.f, 0.f, fRoadSize , 0.f), false);

			Desc.ObjInfos[j] = { L"Prototype_Model_Road", CALC_TF->Get_WorldFloat4x4(), 20.f };
		}
		m_ChunkLists.emplace_back(static_cast<CMap_Chunk*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_MapObj", L"Prototype_MapChunk", &Desc)));
		Safe_AddRef(m_ChunkLists.back());
	}


	CALC_TF->Set_WorldMatrix(XMMatrixIdentity());
	CALC_TF->Set_Scale({ 1.f, 1.f, 1.f });
	CALC_TF->Set_Position({ 0.f, 0.3f, 217.f, 1.f });

	CMapObj::MAPOBJ_DESC MapObjDesc{};
	MapObjDesc.fRadius = 10.f;
	MapObjDesc.strComponentTag = L"Prototype_Model_JumpBase";
	XMStoreFloat4x4(&MapObjDesc.WorldMatrix, CALC_TF->Get_WorldMatrix());

	m_pCinemaJumpBase = m_pGameInstance->Clone_GameObject(L"Prototype_MapObj", &MapObjDesc);
	return S_OK;
}

HRESULT CAW_Manager::Ready_Enemies()
{
	CEnemy_AW::ENEMY_AW_DESC Desc{};
	Desc.wstrModelTag = L"Prototype_Model_Mbone";

	for (size_t i = 0; i < 5; ++i)
	{
		CEnemy_AW* pEnemy = static_cast<CEnemy_AW*>(m_pGameInstance->Clone_GameObject(L"Prototype_Enemy_AW", &Desc));
		pEnemy->Get_Transform()->Set_PositionZ(-6.f);
		pEnemy->Get_Transform()->Set_Scale(1.5f);
		m_EnemyLists.emplace_back(pEnemy);
	}

	m_pMiddleBoss = static_cast<CMiddle_Boss_AW*>(m_pGameInstance->Clone_GameObject(L"Prototype_Middle_Boss_AW"));
	if (nullptr == m_pMiddleBoss)
		return E_FAIL;

	/* Last Boss */ 
	m_pLastBoss = static_cast<CLast_Boss_AW*>(m_pGameInstance->Clone_GameObject(L"Prototype_Last_Boss_AW"));
	if (nullptr == m_pLastBoss)
		return E_FAIL;


	return S_OK;
}

HRESULT CAW_Manager::Ready_Signs()
{
	for (size_t i = 0; i < 3; ++i)
	{
		CSign_AW* pSign1 = static_cast<CSign_AW*>(m_pGameInstance->Clone_GameObject(L"Prototype_Sign_AW"));
		pSign1->Get_Transform()->Set_PositionZ(-6.f);
		CSign_AW* pSign2 = static_cast<CSign_AW*>(m_pGameInstance->Clone_GameObject(L"Prototype_Sign_AW"));
		pSign2->Get_Transform()->Set_PositionZ(-6.f);
		m_SignLists.emplace_back(pSign1, pSign2);
	}

	return S_OK;
}

HRESULT CAW_Manager::Ready_Water()
{
	WATER_DESCS WaterDescs{};
	WaterDescs.fLightColor = { 0.41f, 0.48f, 0.57f, 1.f };
	WaterDescs.fVoronoiScale = 200.f;
	WaterDescs.fWaterNoiseScale = 0.13f;
	WaterDescs.fWaterColor = { 0.07f, 0.26f, 1.1f, 1.f };
	WaterDescs.vFlowSpeed = { -0.2f, -0.2f };

	m_pWater = static_cast<CWater*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Water", L"Prototype_Water", &WaterDescs));
	if (nullptr == m_pWater)
		return E_FAIL;

	m_pWater->Get_Transform()->Set_Scale({ 1000.f, 1000.f, 1.f });
	m_pWater->Get_Transform()->Set_PositionY(-5.f);
	m_pWater->Get_Transform()->Set_PositionZ(200.f);
	return S_OK;
}

HRESULT CAW_Manager::Ready_Sky()
{
	SKY_DESCS SkyDesc{};
	SkyDesc.vTopColor = { -0.01f, 0.04f, 0.94f, 1.f };
	SkyDesc.vBottomColor = { 0.51f, 0.04f, 0.03f, 1.f };
	SkyDesc.vHorizon_Color = { 0.f, 0.f, 1.f };
	SkyDesc.fHorizon_Blur = { 0.11f };
	SkyDesc.vCloud_EdgeColor = { 0.1f, 0.1f, 1.810f };
	SkyDesc.vCloud_TopColor = { 0.71f, 0.f, 0.490f };
	SkyDesc.vCloud_MiddleColor = { 0.96f, 0.13f, 0.15f };
	SkyDesc.vCloud_BottomColor = { 0.22f, 0.04f, 0.41f };
	SkyDesc.fCloud_Speed = 2.f;
	SkyDesc.fCloud_CutOff = 0.35f;
	SkyDesc.fCloud_Fuzziness = 0.51f;
	SkyDesc.fCloud_Weight = 0.f;
	SkyDesc.fCloud_Blur = 0.f;

	m_pGameInstance->Set_SkyDescs(SkyDesc);

	return S_OK;
}

HRESULT CAW_Manager::Ready_AbilityTexts()
{
	m_TextAbilityUIs.resize(10);

	for (size_t i = 0; i < m_TextAbilityUIs.size(); ++i)
		m_TextAbilityUIs[i] = static_cast<CUI_Text_Ability*>(m_pGameInstance->Clone_GameObject(L"Prototype_UI_Text_Ability"));
	


	return S_OK;
}

CEnemy_AW* CAW_Manager::Find_NearestEnemy(_float& fDist) const 
{
	_vector vPlayerPos = m_pPlayer->Get_Transform()->Get_Position();
	_float fMinDist = 9999999.f;
	CEnemy_AW* pNearestEnemy = nullptr;

	for (auto& pEnemy : m_EnemyLists){
		if (false == pEnemy->Is_InLayer() || pEnemy->Get_Hp() <= 0.f)
			continue;

		_vector vEnemyPos = pEnemy->Get_Transform()->Get_Position();
		if (XMVectorGetZ(vEnemyPos) < 1.f)
			continue;


		_float fTargetDist = XMVector3Length(vPlayerPos - vEnemyPos).m128_f32[0];

		if (fTargetDist < fMinDist)
		{
			fMinDist = fTargetDist;
			pNearestEnemy = pEnemy;
		}
	}
	
	if (m_pMiddleBoss->Is_InLayer() && m_pMiddleBoss->Get_Hp() > 0.f)
	{
		_vector vEnemyPos = m_pMiddleBoss->Get_Transform()->Get_Position();
		if (XMVectorGetZ(vEnemyPos) > 1.f)
		{
			_float fTargetDist = XMVector3Length(vPlayerPos - vEnemyPos).m128_f32[0];

			if (fTargetDist < fMinDist)
			{
				fMinDist = fTargetDist;
				pNearestEnemy = m_pMiddleBoss;
			}
		}
	}

	if (m_pLastBoss->Is_InLayer() && m_pLastBoss->Get_Hp() > 0.f)
	{
		_vector vEnemyPos = m_pLastBoss->Get_Transform()->Get_Position();
		if (XMVectorGetZ(vEnemyPos) > 1.f)
		{
			_float fTargetDist = XMVector3Length(vPlayerPos - vEnemyPos).m128_f32[0];

			if (fTargetDist < fMinDist)
			{
				fMinDist = fTargetDist;
				pNearestEnemy = m_pLastBoss;
			}
		}
	}

	fDist = fMinDist;

	return pNearestEnemy;
}

void CAW_Manager::Add_MoveSpeed(_float fVal)
{
	m_fMoveSpeed += fVal;
	for (auto& pChunk : m_ChunkLists)
		pChunk->Add_Speed(fVal);
}

void CAW_Manager::Active_AbilityWindow()
{
	m_bStop = true;

	m_pAbilityUI->OnEnter_Layer(nullptr);
	Safe_AddRef(m_pAbilityUI);

	m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Layer_UI", m_pAbilityUI);
}

void CAW_Manager::Adjust_Reward(REWARD_TYPE eType)
{
	switch (eType)
	{
	case UPGRADE_BULLET:
		CPlayerBullet_AW::Add_BulletStrength();
		break;
	case UPGRADE_BREATH:
		CDragon_Breath::Add_Strength();
		break;
	case SUPERMODE:
		Active_SuperMode();
		break;
	}

}

void CAW_Manager::Adjust_Chest_Reward(CHEST_REWARD_TYPE eType)
{
	switch (eType)
	{
	case ADD_DOG:
		ADD_EVENT(bind(&CAW_Manager::Spawn_Jeane, this));
		AWMGR->Get_Player()->Add_JeanCount();
		break;
	case ADD_DRAGON:
		ADD_EVENT(bind(&CAW_Manager::Spawn_Dragon, this));
		AWMGR->Get_Player()->Add_DragonCount();
		break;
	case ADD_DOG_ATTACK:
		CJeane_AW::Add_Attack(20.f);
		break;
	case ADD_DRAGON_ATTACK:
		CDragon_AW::Add_Attack(20.f);
		break;
	case REDUCE_DOG_ATTACK_INTERVAL:
		CJeane_AW::Adjust_Interval(-0.1f);
		break;
	case REDUCE_DRAGON_ATTACK_INTERVAL:
		CDragon_AW::Adjust_Interval(-0.1f);
		break;
	}

}

void CAW_Manager::Active_Ability_Text(wstring& wstrAbilityText)
{
	for (auto& pTextAbilityUI : m_TextAbilityUIs)
	{
		if (false == pTextAbilityUI->Is_InLayer())
		{
			pTextAbilityUI->OnEnter_Layer(&wstrAbilityText);
			Safe_AddRef(pTextAbilityUI);
			m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Layer_UI", pTextAbilityUI);
			return;
		}
	}
}


void CAW_Manager::Spawn_Jeane()
{
	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Jeane", L"Prototype_Jean_AW");
}

void CAW_Manager::Spawn_Dragon()
{
	m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_Dragon", L"Prototype_Dragon_AW");
}

void CAW_Manager::Active_SuperMode()
{
	m_fSuperMode_Duration = 5.f;
	m_bSuperMode = true;
	m_pPlayer->Active_SuperMode();

}

void CAW_Manager::End_SuperMode()
{
	m_bSuperMode = false;
	m_pPlayer->Inactive_SuperMode();
}

void CAW_Manager::Change_Water(_float fTimeDelta)
{
	if (false == m_bChangingWater)
		return;

	m_fWaterChangingAcc += fTimeDelta;
	if (m_fWaterChangingAcc >= m_fWaterChangeTime)
	{
		m_fWaterChangingAcc = m_fWaterChangeTime;
		m_bChangingWater = false;
		m_bChangingWaterComplete = true;
	}

	_float fRatio = m_fWaterChangingAcc / m_fWaterChangeTime;
	WATER_DESCS WaterDesc = m_pWater->Get_WaterDesc();
	XMStoreFloat4(&WaterDesc.fWaterColor, XMVectorLerp(XMLoadFloat4(&m_vOriginWaterColor), XMLoadFloat4(&m_vChangeWaterColor), fRatio));
	XMStoreFloat4(&WaterDesc.fLightColor, XMVectorLerp(XMLoadFloat4(&m_vOriginLightColor), XMLoadFloat4(&m_vChangeLightColor), fRatio));
	m_pWater->Set_WaterDescs(WaterDesc);

}

void CAW_Manager::Change_GameState(GAME_PROGRESS_STATE eState)
{
	m_eProgressState = eState;

	switch (eState)
	{
	case WARNING:
	{
		End_SuperMode();
		m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_UI", L"Prototype_UI_Warning_Sign");
		m_pCamera->Change_State((_uint)VARCAM_STATES::STATE_AW_BOSSVIEW);

		break;
	}	
	case BOSS_FIGHT:
	{

		ADD_EVENT(bind(&CAW_Manager::Spawn_LastBoss, this));
		break;
	}
	case CINEMATIC:
	{
		ADD_EVENT(bind(&CGameInstance::Insert_GameObject, m_pGameInstance, GET_CURLEVEL, L"Layer_MapObj", m_pCinemaJumpBase));

		_bool bCheckChunk = false;

		size_t iChunkIdx = 0;
		for (auto& pMapChunk : m_ChunkLists)
		{
			pMapChunk->Set_Offset(pMapChunk->Get_ChunkSize() * iChunkIdx - 8.f);
			pMapChunk->Active_Cinematic(true);
			iChunkIdx++;
		}

		m_pPlayer->Get_Transform()->Set_Scale(1.f);
		CINEMAMGR->Start_Cinematic("SonicJuice_Enter");
			
		break;
	}
	}

}

void CAW_Manager::Release()
{
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pPlayer);
	Safe_Release(m_pCamera);

	for (auto& pMapChunk : m_ChunkLists)
		Safe_Release(pMapChunk);

	for (auto& pEnemy : m_EnemyLists)
		Safe_Release(pEnemy);

	m_EnemyLists.clear();

	Safe_Release(m_pMiddleBoss);
	Safe_Release(m_pLastBoss);

	for (auto& Pair : m_SignLists)
	{
		Safe_Release(Pair.first);
		Safe_Release(Pair.second);
	}
		
	m_SignLists.clear();

	Safe_Release(m_pAbilityUI);

	for (auto& pAbilityUI : m_TextAbilityUIs){
		Safe_Release(pAbilityUI);
	}

}
