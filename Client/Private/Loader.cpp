#include "Loader.h"

#include "GameInstance.h"

#include "Effect_Manager.h"
#include "UI_Manager.h"
#include "Cinematic_Manager.h"

#include "Effect_Group.h"
#include "Effect_Particle.h"
#include "Effect_Mesh.h"
#include "Effect_Trail.h"
#include "Effect_MotionTrail.h"

#include "LightObject.h"
#include "SkyBox.h"
#include "SkyCloud.h"
#include "Water.h"
#include "Mozaic_Quad.h"
#include "Highway_Entrance.h"
#include "Mario_EndPortal.h"

#include "Travis_Ending.h"
#include "Jeane_Ending.h"
#include "UI_Logo.h"
#include "CreditText.h"

#include "Free_Camera.h"

#include "FadeScreen.h"

#include "TrashCollectorObject.h"
#include "Croc.h"

#include "SmashBros_Terrain.h"
#include "SmashBros_Wall.h"
#include "SmashBros_Damon.h"
#include "SmashBros_Damon_Weapon_JumpThorn.h"
#include "SmashBros_Damon_Weapon_GiantNeedle.h"

#include "Bike_AW.h"
#include "Map_Chunk.h"
#include "Enemy_AW.h"
#include "Enemy_Bullet_AW.h"
#include "Middle_Boss_AW.h"
#include "Last_Boss_AW.h"
#include "Sign_AW.h"
#include "HP_Number.h"
#include "Jeane_AW.h"
#include "Chest_AW.h"
#include "Dragon_AW.h"
#include "UI_LevelProgress_AW.h"
#include "UI_AbilityWindow_AW.h"
#include "UI_AbilitySelect_AW.h"
#include "UI_Ability_Icon.h"
#include "UI_Ability_Section.h"
#include "UI_Text_Ability.h"
#include "UI_WarningSign.h"

/* Cinema */
#include "Cinema_Travis_World.h"
#include "Cinema_Travis_Bike.h"
#include "Cinema_Damon.h"
#include "Cinema_Mbone.h"
#include "Cinema_Fullface.h"
#include "Cinema_Treatment.h"
#include "Cinema_Tripleput.h"
#include "Cinema_Jeane.h"
#include "Cinema_Travis_Motel.h"

// Player
#include "Weapon.h"
#include "Various_Camera.h"
#include "Travis.h"
#include "Travis_Weapon_BeamKatana.h"
#include "Travis_Space.h"
#include "Travis_Space_Weapon_RailGun.h"
#include "Travis_Space_Weapon_Missile.h"
#include "Travis_World.h"

/* Monster */
#include "Mbone.h"
#include "BoneSpear.h"
#include "Treatment.h"
#include "Tomahawk.h"
#include "Treatment_JumpSlam.h"
#include "Fullface.h"
#include "Fullface_Sword.h"
#include "Fullface_Sheath.h"
#include "Tripleput.h"
#include "Killer.h"
#include "Goomba.h"
#include "Fish.h"
#include "Togezo.h"
/* Boss - MrBlackhole */
#include "MrBlackhole.h"
#include "MrBlackhole_ArmL.h"
#include "MrBlackhole_ArmR.h"
#include "MrBlackhole_Head.h"
#include "MrBlackhole_Blackhole.h"
#include "MrBlackhole_BigBang.h"
/* Boss - MrBlackhole_Space */
#include "Mrblackhole_Space.h"
#include "MrBlackhole_Space_MDarkMatter.h"
#include "MrBlackhole_Space_DarkMatter.h"
#include "MrBlackhole_Space_ArmL.h"
#include "MrBlackhole_Space_ArmR.h"
/* Boss - SonicJuice */
#include "SonicJuice.h"
#include "SonicJuice_Hummingbird.h"
#include "SonicJuice_LegendaryWater.h"
#include "SonicJuice_AquaSpiral.h"
#include "SonicJuice_AquaTornado.h"
#include "SonicJuice_CosmicZaboon.h"
#include "SonicJuice_SplashStorm.h"

/* UI */
#include "UI_Instance_MonsterHP.h"
#include "UI_SushiShop_PopupText.h"
#include "UI_InBattle_Boss.h"
#include "UI_InBattle_Player.h"
#include "UI_InBattle_Monster.h"
#include "UI_InBattle_Named.h"
#include "UI_InBattle_Letter.h"
#include "UI_Loading.h"
#include "UI_Cosmic_Battle.h"
#include "UI_PowerUp.h"
#include "UI_PowerUp_Icon.h"
#include "UI_PlayerStatus.h"
#include "UI_MoveButton.h"
#include "UI_BattleResult.h"
#include "UI_ToiletCheckPoint.h"
#include "UI_MainMenu.h"
#include "UI_Home.h"
#include "UI_Intro_Logo.h"
#include "UI_SushiShop.h"
#include "UI_TurnRPG.h"
#include "UI_InBattle_Mechanic.h"
#include "UI_Smash.h"
#include "UI_Mario.h"
#include "UI_TurnRPG_AttackOption.h"
#include "UI_PlayerStatus_Obj.h"
#include "UI_World_City.h"
#include "UI_World_City_Minimap.h"
#include "UI_World_City_MinimapTarget.h"
#include "UI_CCTV.h"
#include "UI_CCTV_Chating.h"
#include "UI_CCTV_ChatIcon.h"
#include "UI_CCTV_SkipImg.h"
#include "UI_SubTitle.h"
#include "UI_MiniGame_Toilet.h"
#include "UI_MiniGame_Trash.h"
#include "UI_Notice.h"

/* Map */
#include "MapObj.h"
#include "Instance_MapObj.h"
#include "BH_Terrain.h"
#include "Default_Terrain.h"
#include "Trigger.h"
#include "SmashBros_Block.h"
#include "SmashBros_Coin_Block.h"
#include "SmashBros_Brick_Block.h"
#include "Mario_EndStar.h"

/* Gimmick */
#include "Gimmick_TravisRoom_CeilingFan.h"
#include "Gimmick_TravisRoom_ClosetDoor.h"
#include "Gimmick_PressAnyButton.h"
#include "Gimmick_Portal_Star.h"
#include "Gimmick_Galaxy.h"
#include "Gimmick_WindTurbine_Pillar.h"
#include "Gimmick_WindTurbine_Fan.h"
#include "Gimmick_MarioCoin.h"
#include "Gimmick_MarioStar.h"
#include "Gimmick_Mario_MovingBlock.h"
#include "Gimmick_Mario_Background.h"
#include "Gimmick_Mario_FragileBlock.h"
#include "Gimmick_Crystal.h"
#include "Gimmick_DamonTower.h"

// NPC
#include "NPC_Woman_Wandering.h"
#include "NPC_Man_Wandering.h"
#include "NPC_Jeane.h"
#include "NPC_CarA.h"
#include "NPC_Quest.h"
#include "NPC_Bugjiro.h"
#include "NPC_Silvia.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

unsigned int APIENTRY Loading_Main(void* pArg)
{
	CLoader* pLoader = (CLoader*)pArg;

	return pLoader->Loading();
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Loading_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

unsigned int CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	CoInitializeEx(nullptr, 0);

	HRESULT hr = S_OK;

	const char* strSoundPath[] = {
		"Sound/BGM",
		"Sound/Static"
	};

	_int iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
	{
		LeaveCriticalSection(&m_CriticalSection);
		return 1;
	}

	switch (m_eNextLevelID)
	{
	case LEVEL_MAINMENU:
		hr = Load_Level_MainMenu();
		break;
	case LEVEL_WORLD_HOME:
		hr = Load_Level_World_Home();
		break;
	case LEVEL_WORLD_HOME_LAB:
		hr = Load_Level_World_Home_Lab();
		break;
	case LEVEL_BATTLE_MOTEL_FRONT:
		hr = Load_Level_Battle_Motel_Front();
		break;
	case LEVEL_WORLD_CITY:
		hr = Load_Level_World_City();
		break;
	case LEVEL_WORLD_CITY_TRASHCOLLECTOR:
		hr = Load_Level_World_City_TrashCollector();
		break;
	case LEVEL_WORLD_CITY_TOILET:
		hr = Load_Level_World_City_Toilet();
		break;
	case LEVEL_BATTLE_UFO_OUTSIDE:
		hr = Load_Level_Battle_UFO_Outside();
		break;
	case LEVEL_WORLD_UFO:
		hr = Load_Level_World_UFO();
		break;
	case LEVEL_WORLD_UFO_TOILET:
		hr = Load_Level_World_UFO_Toilet();
		break;
	case LEVEL_BATTLE_UFO_INSIDE:
		hr = Load_Level_Battle_UFO_Inside();
		break;
	case LEVEL_BOSS_BLACKHOLE_GROUND:
		hr = Load_Level_Boss_BlackHole_Ground();
		break;
	case LEVEL_BOSS_BLACKHOLE_SPACE:
		hr = Load_Level_Boss_BlackHole_Space();
		break;
	case LEVEL_BOSS_SONICJUICE:
		hr = Load_Level_Boss_SonicJuice();
		break;
	case LEVEL_SMASHBROS_BOSS_DAMON:
		hr = Load_Level_SmashBros_Boss_Damon();
		break;
	case LEVEL_ARROW_A_ROW:
		hr = Load_Level_Arrow_A_Row();
		break;
	case LEVEL_MARIO:
		hr = Load_Level_Mario();
		break;
	case LEVEL_BATTLE_ISLAND:
		hr = Load_Level_Battle_Island();
		break;
	case LEVEL_ENDING:
		hr = Load_Level_Ending();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return 1;

	return 0;

}

HRESULT CLoader::Load_Level_MainMenu()
{
	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/Level_MainMenu.dat")))
		return E_FAIL;

	if (FAILED(Ready_Prototypes_GameObject()))
		return E_FAIL;

	const char* strSoundPath[] =
	{
		"Sound/MainMenu",
		"Sound/Cinematic"
	};

	_int iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	if (FAILED(Load_Effects_Prototype(LEVEL_STATIC, TEXT("../../Resources/Effect/SaveData/Cinematic"))))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Prototypes_GameObject()
{
#pragma region Common

#ifdef _DEBUG

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Free_Camera", CFree_Camera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#endif // _DEBUG

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_LightObject", CLightObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_MotionTrail", CEffect_MotionTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_SkyBox", CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Water", CWater::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_MapObj", CMapObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Mozaic_Quad", CMozaic_Quad::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Highway_Entrance", CHighway_Entrance::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Mario_EndPortal", CMario_EndPortal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Mario_EndStar", CMario_EndStar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//


	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_SmashBros_Terrain", CSmashBros_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_SmashBros_Wall", CSmashBros_Wall::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_SmashBros_Damon", CSmashBros_Damon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_SmashBros_Damon_Weapon_JumpThorn", CSmashBros_Damon_Weapon_JumpThorn::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_SmashBros_Damon_Weapon_GiantNeedle", CSmashBros_Damon_Weapon_GiantNeedle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Woman_Wandering", CNPC_Woman_Wandering::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Man_Wandering", CNPC_Man_Wandering::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Quest", CNPC_Quest::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_CarA", CNPC_CarA::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Jeane", CNPC_Jeane::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_TrashCollectorObject", CTrashCollectorObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Croc", CCroc::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Bugjiro", CNPC_Bugjiro::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_NPC_Silvia", CNPC_Silvia::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion 

#pragma region MapObjects

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MapObj", CMapObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Instance_MapObj", CInstance_MapObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_BH_Terrain", CBH_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Default_Terrain", CDefault_Terrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Trigger", CTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Gimmick */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_PressAnyButton", CGimmick_PressAnyButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Portal_Star", CGimmick_Portal_Star::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion 

#pragma region Monster
	//============================MBONE==============================
	/* Mbone_Prototype */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Mbone", CMbone::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Mbone_BoneSpear */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_BoneSpear", CBoneSpear::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//============================MRBLACKHOLE_GROUND==================
	/* MrBlackhole */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MrBlackhole", CMrBlackhole::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* MrBlackhole_ArmL */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MrBlackhole_ArmL", CMrBlackhole_ArmL::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* MrBlackhole_ArmR */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MrBlackhole_ArmR", CMrBlackhole_ArmR::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* MrBlackhole_Head */ /* Static Mesh */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MrBlackhole_Head", CMrBlackhole_Head::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* MrBlackhole_Blackhole */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MrBlackhole_Blackhole", CMrBlackhole_Blackhole::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* MrBlackhole_BigBang */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MrBlackhole_BigBang", CMrBlackhole_BigBang::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//============================MRBLACKHOLE_SPACE===================
	/* MrBlackhole_Space */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MrBlackhole_Space", CMrblackhole_Space::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* MrBlackhole_Space_MDarkMatter */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MrBlackhole_Space_MDarkMatter", CMrBlackhole_Space_MDarkMatter::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* MrBlackhole_Space_DarkMatter */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MrBlackhole_Space_DarkMatter", CMrBlackhole_Space_DarkMatter::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* MrBlackhole_SpaceArmL */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MrBlackhole_SpaceArmL", CMrBlackhole_Space_ArmL::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* MrBlackhole_SpaceArmR */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MrBlackhole_SpaceArmR", CMrBlackhole_Space_ArmR::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//============================TREATMENT===========================
	/* Treatment */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Treatment", CTreatment::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Treatment_Tomahawk */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Tomahawk", CTomahawk::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Treatment_JumpSlam */ /* for.Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Treatment_JumpSlam", CTreatment_JumpSlam::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//============================FULLFACE=============================
	/* Fullface */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Fullface", CFullface::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Fullface_Sword */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Fullface_Sword", CFullface_Sword::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Fullface_Sheath */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Fullface_Sheath", CFullface_Sheath::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//============================TRIPLEPUT=============================
	/* Tripleput */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Tripleput", CTripleput::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//============================SONICJUICE============================
	/* SonicJuice */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_SonicJuice", CSonicJuice::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* SonicJuice_HummingBird */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_SonicJuice_Hummingbird", CSonicJuice_Hummingbird::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* SonicJuice_LegendaryWater */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_SonicJuice_LegendaryWater", CSonicJuice_LegendaryWater::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* SonicJuice_AquaSpiral */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_SonicJuice_AquaSpiral", CSonicJuice_AquaSpiral::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* SonicJuice_AquaTornado */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_SonicJuice_AquaTornado", CSonicJuice_AquaTornado::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* SonicJuice_CosmicZaboon */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_SonicJuice_CosmicZaboon", CSonicJuice_CosmicZaboon::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* SonicJuice_SplashStorm */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_SonicJuice_SplashStorm", CSonicJuice_SplashStorm::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Fish */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Fish", CFish::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//============================MARIO============================
	/* Killer */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Killer", CKiller::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Goomba */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Goomba", CGoomba::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* Togezo */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Togezo", CTogezo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Gimmick */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_DamonTower", CGimmick_DamonTower::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_TravisRoom_CeilingFan_Fan", CGimmick_TravisRoom_CeilingFan::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_TravisRoom_ClosetDoor_L", CGimmick_TravisRoom_ClosetDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Wind_Turbine_Pillar", CGimmick_WindTurbine_Pillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Wind_Turbine_Fan", CGimmick_WindTurbine_Fan::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Galaxy", CGimmick_Galaxy::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Mario_Coin", CGimmick_MarioCoin::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Mario_Star", CGimmick_MarioStar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Crystal", CGimmick_Crystal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region
	if (FAILED(Load_UI()))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CLoader::Load_Level_World_Home()
{
	if (FAILED(Load_NPC((_uint)m_eNextLevelID)))
		return E_FAIL;

	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/TravisHouse.dat", L"../../Resources/DataFiles/NavData/Nav_TravisHouse.dat")))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_World_Home_Lab()
{
	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/TravisHouseB1_NaomiLab.dat", L"../../Resources/DataFiles/NavData/Nav_TravisRoomB1_NaomiLab.dat")))
		return E_FAIL;

	if (FAILED(EFFECTMGR->Effect_Loader(LEVEL_WORLD_HOME_LAB, TEXT("../../Resources/Effect/SaveData/Etc/Wrist_Cutter.json"), m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Navigation_Camera",
		CNavigation::Create(m_pDevice, m_pContext, L"../../Resources/DataFiles/Camera_NavData/CamNav_Lab.dat"))))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_Battle_Motel_Front()
{
	/* Gimmick */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE_MOTEL_FRONT, L"Prototype_Model_Wind_Turbine_Fan", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/MapObjects/Map_Normal/SantaDestroy_Road/", "Wind_Turbine_Fan.dat"))))
		return E_FAIL;

	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/TravisMotel.dat", L"../../Resources/DataFiles/NavData/Nav_MotelFront.dat")))
		return E_FAIL;

	const char* strSoundPath[] =
	{
		"Sound/Wimp",
	};

	_int iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	/* Monster Load */
	if (FAILED(Load_Enemy((_uint)LEVEL::LEVEL_BATTLE_MOTEL_FRONT)))
		return E_FAIL;

	//Effect 
	if (FAILED(Load_Effects_Prototype(LEVEL_BATTLE_MOTEL_FRONT, TEXT("../../Resources/Effect/SaveData/Tripleput"))))
		return E_FAIL;
	if (FAILED(Load_Effects_Prototype(LEVEL_BATTLE_MOTEL_FRONT, TEXT("../../Resources/Effect/SaveData/Blood_Green"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Navigation_Camera",
		CNavigation::Create(m_pDevice, m_pContext, L"../../Resources/DataFiles/Camera_NavData/CamNav_MotelFront.dat"))))
		return E_FAIL;

	/* Cinema */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE_MOTEL_FRONT, L"Prototype_Model_SmashBros_Damon", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/SmashBros_Damon/", "SmashBros_Damon.dat", "../../Resources/KeyFrame/SmashBros_Damon/SmashBros_Damon_BindedKeyFrames/"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BATTLE_MOTEL_FRONT, L"Prototype_Model_NPC_Jeane", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/NPC_Jeane/", "Jeane.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Cinema_Damon", CCinema_Damon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Cinema_Mbone", CCinema_Mbone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Cinema_Fullface", CCinema_Fullface::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Cinema_Treatment", CCinema_Treatment::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Cinema_Tripleput", CCinema_Tripleput::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Cinema_Jeane", CCinema_Jeane::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Cinema_Travis_Motel", CCinema_Travis_Motel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CINEMAMGR->Load_CinematicDatas(TEXT("../../Resources/CinematicData/Motel/"))))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_World_City()
{
	/* Gimmick */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_WORLD_CITY, L"Prototype_Model_Wind_Turbine_Fan", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/MapObjects/Map_Normal/SantaDestroy_Road/", "Wind_Turbine_Fan.dat"))))
		return E_FAIL;

	if (FAILED(Load_NPC((_uint)m_eNextLevelID)))
		return E_FAIL;

	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/TravisMotel.dat", L"../../Resources/DataFiles/NavData/Nav_MotelFront.dat")))
		return E_FAIL;

	const char* strSoundPath[] =
	{
		"Sound/OpenWorld",
		"Sound/Bike",
		"Sound/Volunteer",
		"Sound/Mario"
	};

	_uint iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	if (FAILED(Load_Travis_Bike()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Navigation_Camera",
		CNavigation::Create(m_pDevice, m_pContext, L"../../Resources/DataFiles/Camera_NavData/CamNav_MotelFront.dat"))))
		return E_FAIL;

	EFFECTMGR->Effect_Loader(LEVEL_WORLD_CITY, TEXT("../../Resources/Effect/SaveData/Etc/City_Highway_Entrance.json"), m_pDevice, m_pContext);


	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_World_City_TrashCollector()
{
	// Trash Models
	if (FAILED(Load_Trashes()))
		return E_FAIL;

	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/Garbage_Collecting.dat", L"../../Resources/DataFiles/NavData/Nav_GarbageCollecting.dat")))
		return E_FAIL;

	const char* strSoundPath[] =
	{
		"Sound/Garbage",
		"Sound/Volunteer",
	};

	_uint iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	if (FAILED(Load_Effects_Prototype(LEVEL_WORLD_CITY_TRASHCOLLECTOR, TEXT("../../Resources/Effect/SaveData/Trash_Collector/"))))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_World_City_Toilet()
{
	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/Toilet_Repair.dat")))
		return E_FAIL;

	const char* strSoundPath[] =
	{
		"Sound/Toilet",
		"Sound/Volunteer",
	};

	_uint iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_Battle_UFO_Outside()
{
	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/UFO_Outside.dat", L"../../Resources/DataFiles/NavData/Nav_UFO_Outside.dat")))
		return E_FAIL;

	const char* strSoundPath[] =
	{
		"Sound/Wimp",
	};

	_int iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;


	/* Monster Load */
	if (FAILED(Load_Enemy((_uint)LEVEL::LEVEL_BATTLE_UFO_OUTSIDE)))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_World_UFO()
{
	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/World_UFO.dat", L"../../Resources/DataFiles/NavData/Nav_UFO_Outside.dat")))
		return E_FAIL;


	const char* strSoundPath[] =
	{
		"Sound/Wimp",
	};

	_int iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Navigation_Camera",
		CNavigation::Create(m_pDevice, m_pContext, L"../../Resources/DataFiles/NavData/Nav_UFO_Outside.dat"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Model_NPC_Bugjiro",
		CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/NPC_Bugjiro/", "NPC_Bugjiro.dat"))))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_World_UFO_Toilet()
{

	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/Toilet_Repair.dat")))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_Battle_UFO_Inside()
{
	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/UFO_Inside.dat", L"../../Resources/DataFiles/NavData/Nav_UFO_Inside.dat")))
		return E_FAIL;

	const char* strSoundPath[] =
	{
		"Sound/Wimp",
	};

	_int iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	/* Monster Load */
	if (FAILED(Load_Enemy((_uint)LEVEL::LEVEL_BATTLE_UFO_INSIDE)))
		return E_FAIL;

	if (FAILED(Load_Effects_Prototype(LEVEL_BATTLE_UFO_INSIDE, TEXT("../../Resources/Effect/SaveData/Treatment/"))))
		return E_FAIL;
	if (FAILED(Load_Effects_Prototype(LEVEL_BATTLE_UFO_INSIDE, TEXT("../../Resources/Effect/SaveData/Blood_Green/"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Navigation_Camera",
		CNavigation::Create(m_pDevice, m_pContext, L"../../Resources/DataFiles/NavData/Nav_UFO_Inside.dat"))))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_Boss_BlackHole_Ground()
{
	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/Land_MrBlackHole.dat", L"../../Resources/DataFiles/NavData/Nav_Land_MrBlackHole.dat")))
		return E_FAIL;

	const char* strSoundPath[] =
	{
		"Sound/MrBlackHole",
	};

	_int iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	/* Monster Load */
	if (FAILED(Load_Enemy((_uint)LEVEL::LEVEL_BOSS_BLACKHOLE_GROUND)))
		return E_FAIL;

	if (FAILED(Load_Effects_Prototype(LEVEL_BOSS_BLACKHOLE_GROUND, TEXT("../../Resources/Effect/SaveData/Blackhole/"))))
		return E_FAIL;

	

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_Boss_BlackHole_Space()
{
	const char* strSoundPath[] =
	{
		"Sound/MrBlackHole",
		"Sound/MrBlackHole_Space",
		"Sound/SpaceBattle",
	};

	_uint iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;


	/* Monster Load */
	if (FAILED(Load_Enemy((_uint)LEVEL::LEVEL_BOSS_BLACKHOLE_SPACE)))
		return E_FAIL;

	if (FAILED(Load_Effects_Prototype(LEVEL_BOSS_BLACKHOLE_SPACE, TEXT("../../Resources/Effect/SaveData/Travis_Space/"))))
		return E_FAIL;

	if (FAILED(Load_Effects_Prototype(LEVEL_BOSS_BLACKHOLE_SPACE, TEXT("../../Resources/Effect/SaveData/BlackHole_Space/"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_CubeTexture_Galaxy", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/SkyBox/BlackHole_Space/GalaxyEnvHDR.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Irradiance_Space", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/SkyBox/IBL_Resources/Space/SpaceDiffuseHDR.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_SpecularIBL_Space", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/SkyBox/IBL_Resources/Space/SpaceSpecularHDR.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_BRDF_Space", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/SkyBox/IBL_Resources/Space/SpaceBrdf.dds"))))
		return E_FAIL;

	EFFECTMGR->Effect_Loader(LEVEL_BOSS_BLACKHOLE_SPACE, TEXT("../../Resources/Effect/SaveData/Etc/Space_Explosion.json"), m_pDevice, m_pContext);


	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_Boss_SonicJuice()
{
	const char* strMapPath[] =
	{
		"Map_Boss/SonicJuice",
	};

	_int iPathCount = sizeof(strMapPath) / sizeof(strMapPath[0]);

	if (FAILED(Load_Map(strMapPath, iPathCount, L"../../Resources/DataFiles/NavData/Nav_SonicJuice.dat")))
		return E_FAIL;


	const char* strSoundPath[] =
	{
		"Sound/SonicJuice_P1",
		"Sound/SonicJuice_P2",
	};

	iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	/* Monster Load */
	if (FAILED(Load_Enemy((_uint)LEVEL::LEVEL_BOSS_SONICJUICE)))
		return E_FAIL;

	if (FAILED(Load_Effects_Prototype(LEVEL_BOSS_SONICJUICE, TEXT("../../Resources/Effect/SaveData/Sonic_Juice/"))))
		return E_FAIL;

	if (FAILED(Load_Effects_Prototype(LEVEL_BOSS_SONICJUICE, TEXT("../../Resources/Effect/SaveData/Travis_Armor/"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Cinema_Travis_World", CCinema_Travis_World::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Cinema_Travis_Bike", CCinema_Travis_Bike::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(CINEMAMGR->Load_CinematicDatas(TEXT("../../Resources/CinematicData/SonicJuice/"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_BOSS_SONICJUICE, L"Prototype_SonicJuice_EnvMap", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/Models/SonicJuice/Tex/SonicJuice_Env.dds"))))
		return E_FAIL;


	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_SmashBros_Boss_Damon()
{
	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/Damon_BossMap.dat")))
		return E_FAIL;

	const char* strSoundPath[] =
	{
		"Sound/Damon",
	};

	_int iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_SMASHBROS_BOSS_DAMON, L"Prototype_Model_SmashBros_Damon", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/SmashBros_Damon/", "SmashBros_Damon.dat", "../../Resources/KeyFrame/SmashBros_Damon/SmashBros_Damon_BindedKeyFrames/"))))
		return E_FAIL;

	//Effect 
	if (FAILED(Load_Effects_Prototype(LEVEL_SMASHBROS_BOSS_DAMON, TEXT("../../Resources/Effect/SaveData/Damon/"))))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_Arrow_A_Row()
{
	m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Model_Road", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/MapObjects/Map_Normal/SantaDestroy_Road/", "Road001.dat"));

	m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Model_JumpBase", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/MapObjects/Map_Normal/IndustrialProps/", "JumpBase_L.dat"));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Model_Chest_Up", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/MapObjects/Arrow_A_Row/", "Chest_Up.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Model_Chest_Down", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/MapObjects/Arrow_A_Row/", "Chest_Down.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Model_Mbone", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Mbone/", "Mbone.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Model_Jeane", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/NPC_Jeane/", "Jeane.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Model_Dragon", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Dragon/", "Dragon.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Model_Leopardon", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Leopardon/", "Leopardon.dat", "../../Resources/KeyFrame/Leopardon/Leopardon_BindedKeyFrames/"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Model_DestroymanTF", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/DestroymanTF/", "DestroymanTF.dat", "../../Resources/KeyFrame/DestroymanTF/DestroymanTF_BindedKeyFrames/"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Texture_Progress_Empty_AW", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/UI/UI/Arrow_A_Row/Stage_Progress_Empty.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Texture_Progress_Full_AW", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/UI/UI/Arrow_A_Row/Stage_Progress_Full.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Texture_Travis_Icon", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/UI/UI/Arrow_A_Row/Travis_Icon.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Texture_Ability_Window", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/UI/UI/Arrow_A_Row/UI_AbilityWindow.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Texture_Ability_Select", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/UI/UI/Arrow_A_Row/UI_AbilitySelect.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, TEXT("Prototype_Texture_Warning_Sign"), CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/UI/UI/Arrow_A_Row/Warning_Sign.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Texture_Icon_Stronger_Bullet", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/UI/UI/Arrow_A_Row/AbilityIcon/Icon_Stronger_Bullet.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Texture_Icon_Speed_Up", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/UI/UI/Arrow_A_Row/AbilityIcon/Icon_Speed_Up.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ARROW_A_ROW, L"Prototype_Texture_Icon_Stronger_Dragon", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/UI/UI/Arrow_A_Row/AbilityIcon/Icon_Stronger_Dragon.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Bike_AW", CBike_AW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_MapChunk", CMap_Chunk::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Enemy_AW", CEnemy_AW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Middle_Boss_AW", CMiddle_Boss_AW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Enemy_Bullet_AW", CEnemy_Bullet_AW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Sign_AW", CSign_AW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_HP_Number", CHP_Number::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Jean_AW", CJeane_AW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Chest_AW", CChest_AW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Dragon_AW", CDragon_AW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Last_Boss_AW", CLast_Boss_AW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_LevelProgress_AW", CUI_LevelProgress_AW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_AbilityWindow_AW", CUI_AbilityWindow_AW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_AbilitySelect_AW", CUI_AbilitySelect_AW::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Ability_Section", CUI_Ability_Section::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Ability_Icon", CUI_Ability_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Warning_Sign", CUI_WarningSign::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Text_Ability", CUI_Text_Ability::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	
	if (FAILED(Load_Effects_Prototype(LEVEL_ARROW_A_ROW, TEXT("../../Resources/Effect/SaveData/Arrow_A_Row/"))))
		return E_FAIL;

	if (FAILED(CINEMAMGR->Load_CinematicDatas(TEXT("../../Resources/CinematicData/Arrow_A_Row/"))))
		return E_FAIL;

	const char* strSoundPath[] =
	{
		"Sound/Arrow_A_Row",
	};

	_int iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_Mario()
{
	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/Level_Mario.dat")))
		return E_FAIL;

	const char* strSoundPath[] =
	{
		"Sound/Mario",
	};

	_int iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MARIO, L"Prototype_Model_SmashBros_Damon", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/SmashBros_Damon/", "SmashBros_Damon.dat", "../../Resources/KeyFrame/SmashBros_Damon/SmashBros_Damon_BindedKeyFrames/"))))
		return E_FAIL;

	/* Effect */
	/* Monster Load */
	if (FAILED(Load_Enemy((_uint)LEVEL::LEVEL_MARIO)))
		return E_FAIL;

	//Effect 
	if (FAILED(Load_Effects_Prototype(LEVEL_MARIO, TEXT("../../Resources/Effect/SaveData/Mario/"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_Mario_Bush",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/MapObjects/Map_Boss/Mario/Tex/Mario_Bush.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_Mario_PillarFront",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/MapObjects/Map_Boss/Mario/Tex/Mario_PillarFront.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_Mario_PillarBack",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/MapObjects/Map_Boss/Mario/Tex/Mario_PillarBack.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_Mario_Cloud",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/MapObjects/Map_Boss/Mario/Tex/Mario_Cloud.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_Mario_Sky",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/MapObjects/Map_Boss/Mario/Tex/Mario_Sky.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Mario_Background", CGimmick_Mario_Background::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_Battle_Island()
{
	if (FAILED(Load_Resource(L"../../Resources/DataFiles/AddressData/Level_BattleIsland.dat", L"../../Resources/DataFiles/NavData/Nav_Battle_Island.dat")))
		return E_FAIL;

	const char* strSoundPath[] =
	{
		"Sound/Wimp",
		//"Sound/Static",
	};

	_int iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	/* Monster Load */
	if (FAILED(Load_Enemy((_uint)LEVEL::LEVEL_BATTLE_ISLAND)))
		return E_FAIL;

	/* Effect */
	if (FAILED(Load_Effects_Prototype(LEVEL_BATTLE_ISLAND, TEXT("../../Resources/Effect/SaveData/FullFace/"))))
		return E_FAIL;

	if (FAILED(Load_Effects_Prototype(LEVEL_BATTLE_ISLAND, TEXT("../../Resources/Effect/SaveData/Blood_Green/"))))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Level_Ending()
{
	const char* strSoundPath[] =
	{
		"Sound/Ending",
	};

	_int iPathCount = sizeof(strSoundPath) / sizeof(strSoundPath[0]);

	if (FAILED(Load_Sound(strSoundPath, iPathCount)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ENDING, L"Prototype_Model_Jeane", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/NPC_Jeane/", "Jeane.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Travis_Ending", CTravis_Ending::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Jeane_Ending", CJeane_Ending::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ENDING, L"Prototype_Texture_Logo", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/UI/UI/Main/UI_Intro_Logo/UI_Intro_Logo.dds"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Logo", CUI_Logo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_CreditText", CCreditText::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Travis(_uint _iLevelIndex)
{
	//// Travis Battle Ground
	//if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Travis", CModel::Create(m_pDevice, m_pContext,
	//	"../../Resources/Models/Player_NMH1/", "Player_NMH1.dat", "../../Resources/KeyFrame/Travis/Travis_BindedKeyFrames/"))))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Travis_Stubble", CModel::Create(m_pDevice, m_pContext,
	//	"../../Resources/Models/Player_Stubble/", "Player_Stubble.dat", "../../Resources/KeyFrame/Travis/Travis_BindedKeyFrames/"))))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Travis_BeamKatana", CModel::Create(m_pDevice, m_pContext,
	//	"../../Resources/Models/Player_NMH1/", "Travis_Weapon.dat"))))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_PlayerCamera", CModel::Create(m_pDevice, m_pContext,
	//	"../../Resources/Models/Player_Camera/", "Player_Camera.dat"))))
	//	return E_FAIL;


	//// Travis Battle Space
	//if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Travis_Space_Missile", CModel::Create(m_pDevice, m_pContext,
	//	"../../Resources/Models/Player_Projectiles/Player_Space_Missile/", "Player_Space_Missile.dat"))))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Travis_Space", CModel::Create(m_pDevice, m_pContext,
	//	"../../Resources/Models/Player_Space/", "Player_Space.dat", "../../Resources/KeyFrame/Travis_Space/TravisSpace_BindedKeyFrames/"))))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Player_Space_Camera", CModel::Create(m_pDevice, m_pContext,
	//	"../../Resources/Models/Player_Space_Camera/", "Player_Space_Camera.dat"))))
	//	return E_FAIL;

	//// Travis World
	//if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Travis_World", CModel::Create(m_pDevice, m_pContext,
	//	"../../Resources/Models/Player_NMH1_World/", "Player_NMH1_World.dat"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Enemy(_uint _iLevelIndex)
{
	//==============================================Model=====================================================================
	if (_iLevelIndex == LEVEL_BOSS_BLACKHOLE_GROUND) {
		//======================MRBLACKHOLE_GROUND=============
		/* MrBlackhole */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_MrBlackhole", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/MrBlackhole/", "MrBlackhole.dat", "../../Resources/KeyFrame/MrBlackhole/MrBlackhole_BindedKeyFrame"))))
			return E_FAIL;
		/* MrBlackhole_ArmL */ /* GROUND_SPACE */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_MrBlackhole_ArmL", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/MrBlackhole/", "MrBlackhole_ArmL.dat", "../../Resources/KeyFrame/MrBlackhole/MrBlackhole_ArmL_BindedKeyFrames/"))))
			return E_FAIL;
		/* MrBlackhole_ArmR */ /* GROUND_SPACE */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_MrBlackhole_ArmR", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/MrBlackhole/", "MrBlackhole_ArmR.dat", "../../Resources/KeyFrame/MrBlackhole/MrBlackhole_ArmR_BindedKeyFrames/"))))
			return E_FAIL;
		/* MrBlackhole_Head */ /* Static Mesh */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_MrBlackhole_Head", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/MrBlackhole/", "MrBlackhole_Head.dat"))))
			return E_FAIL;
	}
	else if (_iLevelIndex == LEVEL_BOSS_BLACKHOLE_SPACE) {
		//======================MRBLACKHOLE_SPACE==============
		/* MrBlackhole_Space */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_MrBlackhole_Space", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/MrBlackhole/", "MrBlackhole_Space.dat", "../../Resources/KeyFrame/MrBlackhole/MrBlackhole_Space_BindedKeyFrame/"))))
			return E_FAIL;
		/* MrBlackhole_ArmL */ /* GROUND_SPACE */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_MrBlackhole_ArmL", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/MrBlackhole/", "MrBlackhole_ArmL.dat", "../../Resources/KeyFrame/MrBlackhole/MrBlackhole_ArmL_BindedKeyFrames/"))))
			return E_FAIL;
		/* MrBlackhole_ArmR */ /* GROUND_SPACE */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_MrBlackhole_ArmR", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/MrBlackhole/", "MrBlackhole_ArmR.dat", "../../Resources/KeyFrame/MrBlackhole/MrBlackhole_ArmR_BindedKeyFrames/"))))
			return E_FAIL;
	}
	else if (_iLevelIndex == LEVEL_SMASHBROS_BOSS_DAMON) {


	}
	else if (_iLevelIndex == LEVEL_BOSS_SONICJUICE) {
		//======================SONICJUICE=======================
		/* SonicJuice */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_SonicJuice", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/SonicJuice/", "SonicJuice.dat", "../../Resources/KeyFrame/SonicJuice/SonicJuice_BindedKeyFrames/"))))
			return E_FAIL;
	}
	else if (_iLevelIndex == LEVEL_MARIO) {
		//======================KILLER===========================
		/* Killer */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Killer", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Killer/", "Killer.dat"))))
			return E_FAIL;
		//======================GOOMBA===========================
		/* Goomba */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Goomba", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Goomba/", "Goomba.dat"))))
			return E_FAIL;
		/* Goomba_Press */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Goomba_Press", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Goomba/", "GoombaPress.dat"))))
			return E_FAIL;
		//======================TOGEZO===========================
		/* Togezo */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Togezo", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Togezo/", "Togezo.dat"))))
			return E_FAIL;
	}
	else {
		//======================MBONE==========================
		/* Mbone */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Mbone", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Mbone/", "Mbone.dat", "../../Resources/KeyFrame/MBone/Mbone_BindedKeyFrames/"))))
			return E_FAIL;
		/* Mbone2 */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Mbone2", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Mbone/", "Mbone2.dat", "../../Resources/KeyFrame/MBone/Mbone_BindedKeyFrames/"))))
			return E_FAIL;
		/* Mbone_BoneSpear */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Mbone_BoneSpear", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Mbone/", "BoneSpear.dat"))))
			return E_FAIL;
		//======================TREATMENT======================
		/* Treatment */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Treatment", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Treatment/", "Treatment.dat", "../../Resources/KeyFrame/Treatment/Treatment_BindedKeyFrames/"))))
			return E_FAIL;
		/* Treatment_Tomahawk */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Treatment_Tomahawk", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Treatment/", "Tomahawk.dat"))))
			return E_FAIL;
		//======================FULLFACE=======================
		/* Fullface */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Fullface", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Fullface/", "Fullface.dat", "../../Resources/KeyFrame/Fullface/Fullface_BindedKeyFrames/"))))
			return E_FAIL;
		/* Fullface_Sword */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Fullface_Sword", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Fullface/", "FullfaceSword.dat"))))
			return E_FAIL;
		/* Fullface_Sheath */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Fullface_Sheath", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Fullface/", "FullfaceSheath.dat"))))
			return E_FAIL;
		//======================TRIPLEPUT=======================
		/* Tripleput */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_Tripleput", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/Tripleput/", "Tripleput.dat", "../../Resources/KeyFrame/Tripleput/Tirpleput_BindedKeyFrames/"))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Load_Travis_Bike()
{


	return S_OK;
}

HRESULT CLoader::Load_Resource(const wstring& strFilePath, const wstring& strNavigationPath)
{
	HANDLE hFile = CreateFile(strFilePath.c_str(),  // 파일 경로와 이름을 명시
		GENERIC_READ,   // 파일 접근 모드(GENERIC_WRITE : 쓰기 전용, GENERIC_READ : 읽기 전용)
		NULL,   // 공유 방식, 파일이 개방된 상태에서 다른 프로세스가 파일 개방을 하는 것을 허가할 것인가(NULL 공유하지 않음)
		NULL,   // 보안 속성, NULL 인 경우 기본 보안 상태
		OPEN_EXISTING,  // 생성 방식, (CREATE_ALWAYS : 파일이 없다면 생성, 있다면 덮어쓰기, OPEN_EXISTING : 파일이 있을 경우에만 열기)
		FILE_ATTRIBUTE_NORMAL,  // 파일 속성(읽기 전용, 숨김 파일 등등),FILE_ATTRIBUTE_NORMAL : 특수 속성이 없는 일반 파일 형태
		NULL);  // 생성될 파일의 속성을 제공할 템플릿 파일 ( 사용하지 않기 때문에 NULL)

	if (INVALID_HANDLE_VALUE == hFile)
	{
		assert(false);

		return E_FAIL;
	}

	DWORD dwByte(0);
	_uint iReadFlag = 0;
	_uint iModelCnt = 0;

	iReadFlag += (_uint)ReadFile(hFile, &iModelCnt, sizeof(_uint), &dwByte, nullptr);
	if (1 != iReadFlag)
		return E_FAIL;

	for (_uint i = 0; i < iModelCnt; ++i)
	{
		DWORD       iLen1 = 0;
		DWORD       iLen2 = 0;
		wstring		wstrName;
		wstring		wstrAddress;

		iReadFlag += (_uint)ReadFile(hFile, &iLen1, sizeof DWORD, &dwByte, nullptr);
		iReadFlag += (_uint)ReadFile(hFile, &iLen2, sizeof DWORD, &dwByte, nullptr);

		wstrName.resize(iLen1);
		wstrAddress.resize(iLen2);
		iReadFlag += (_uint)ReadFile(hFile, wstrName.data(), iLen1 * sizeof(wchar_t), &dwByte, nullptr);
		iReadFlag += (_uint)ReadFile(hFile, wstrAddress.data(), iLen2 * sizeof(wchar_t), &dwByte, nullptr);

		if (5 != iReadFlag)
			return E_FAIL;

		wstring strPrototypeComponent = L"Prototype_Model_" + wstrName;
		wstring strPrototypeInstanceComponent = L"Prototype_Model_Instance_" + wstrName;

		string str_dat = ".dat";
		string strName_dat = Convert_WStrToStr(wstrName) + str_dat;
		string strFullPath = Convert_WStrToStr(wstrAddress);

		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, strPrototypeComponent, CModel::Create(m_pDevice, m_pContext, strFullPath, strName_dat))))
				return E_FAIL;

		CModel* pModel = static_cast<CModel*>(m_pGameInstance->Find_Prototype(m_eNextLevelID, strPrototypeComponent));

		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, strPrototypeInstanceComponent, CModel_Instance::Create(m_pDevice, m_pContext, pModel))))
			return E_FAIL;

		if (L"../../Resources/Models/MapObjects/Map_Boss/CubeMesh/" == wstrAddress && wstrName.find(L"Grass") == wstring::npos)
		{
			wstring strPrototype = L"Prototype_GameObject_" + wstrName;

			if (L"Mario_QuestionBlock" == wstrName)
			{
				if (FAILED(m_pGameInstance->Add_Prototype(strPrototype, CSmashBros_Coin_Block::Create(m_pDevice, m_pContext))))
					return E_FAIL;
			}
			else if (L"Mario_BrickBlock" == wstrName)
			{
				if (FAILED(m_pGameInstance->Add_Prototype(strPrototype, CSmashBros_Brick_Block::Create(m_pDevice, m_pContext))))
					return E_FAIL;
			}
			else if (L"DiamondBlock_Blue" == wstrName)
			{
				if (FAILED(m_pGameInstance->Add_Prototype(strPrototype, CGimmick_Mario_MovingBlock::Create(m_pDevice, m_pContext))))
					return E_FAIL;
			}
			else if (L"CobbleStone" == wstrName)
			{
				if (FAILED(m_pGameInstance->Add_Prototype(strPrototype, CGimmick_Mario_FragileBlock::Create(m_pDevice, m_pContext))))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pGameInstance->Add_Prototype(strPrototype, CSmashBros_Block::Create(m_pDevice, m_pContext))))
					return E_FAIL;
			}
		}

		iReadFlag = 1;
	}


	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_Default_Terrain",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/TestGround.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_BH_Terrain",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/MapObjects/Map_Boss/Land_MrBlackHole/Tex/T_ground_cracks_D.dds"))))
		return E_FAIL;

	if (strNavigationPath != L"") {
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Navigation",
			CNavigation::Create(m_pDevice, m_pContext, strNavigationPath))))
			return E_FAIL;
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLoader::Load_Map(const char* strMapPath[], _int iPathCount, const wstring& strNavigationPath)
{
	wstring wstrFullPath;
	wstring wstrFullPath1;
	string strFullPath;
	wstring wstrName_dat;
	string strName_dat;
	wstring strPrototypeComponent;
	wstring strPrototypeInstanceComponent;

#pragma region Components

	for (int i = 0; i < iPathCount; ++i)
	{
		wstrFullPath = L"../../Resources/Models/MapObjects/" + Convert_StrToWStr(strMapPath[i]);
		wstrFullPath1 = wstrFullPath + L"/";
		strFullPath = Convert_WStrToStr(wstrFullPath1);

		vModelNames = Find_Models(wstrFullPath);

		for (int j = 0; j < vModelNames.size(); ++j)
		{
			wstrName_dat = vModelNames[j] + L".dat";
			strName_dat = Convert_WStrToStr(wstrName_dat);
			strPrototypeComponent = L"Prototype_Model_" + vModelNames[j];
			strPrototypeInstanceComponent = L"Prototype_Model_Instance_" + vModelNames[j];

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, strPrototypeComponent, CModel::Create(m_pDevice, m_pContext, strFullPath, strName_dat))))
				return E_FAIL;

			CModel* pModel = static_cast<CModel*>(m_pGameInstance->Find_Prototype(m_eNextLevelID, strPrototypeComponent));

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, strPrototypeInstanceComponent, CModel_Instance::Create(m_pDevice, m_pContext, pModel))))
				return E_FAIL;
		}
	}

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_Default_Terrain",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/TestGround.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_BH_Terrain",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/MapObjects/Map_Boss/Land_MrBlackHole/Tex/T_ground_cracks_D.dds"))))
		return E_FAIL;

#pragma endregion Components



#pragma region Prototypes

	//	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_MapObj", CMapObj::Create(m_pDevice, m_pContext))))
	//		return E_FAIL;
	//
	//	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_Instance_MapObj", CInstance_MapObj::Create(m_pDevice, m_pContext))))
	//		return E_FAIL;
	//
	//	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_BH_Terrain", CBH_Terrain::Create(m_pDevice, m_pContext))))
	//		return E_FAIL;
	//
	//	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Default_Terrain", CDefault_Terrain::Create(m_pDevice, m_pContext))))
	//		return E_FAIL;
	//
	//	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Trigger", CTrigger::Create(m_pDevice, m_pContext))))
	//		return E_FAIL;
	//
	if (strNavigationPath != L"") {
		if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Navigation",
			CNavigation::Create(m_pDevice, m_pContext, strNavigationPath))))
			return E_FAIL;
	}

#pragma endregion Prototypes

	return S_OK;
}

HRESULT CLoader::Load_Sound(const char* strSoundPath[], _int iPathCount)
{
	wstring wstrFullPath;
	wstring wstrFullPath1;
	wstring wstrFullPath2;
	string strFullPath;
	wstring wstrName_wav;
	string strName_wav;
	string strSoundName;

	vector<wstring>		vecSoundNames;

	for (int i = 0; i < iPathCount; ++i)
	{
		wstrFullPath = L"../../Resources/" + Convert_StrToWStr(strSoundPath[i]);
		wstrFullPath1 = wstrFullPath + L"/";

		vecSoundNames = Find_Sounds(wstrFullPath);

		for (int j = 0; j < vecSoundNames.size(); ++j)
		{
			wstrName_wav = vecSoundNames[j];// +L".wav";
			wstrFullPath2 = wstrFullPath1 + wstrName_wav;
			strFullPath = Convert_WStrToStr(wstrFullPath2);
			strSoundName = Convert_WStrToStr(vecSoundNames[j].substr(0, vecSoundNames[j].size() - 4));

			if (FAILED(m_pGameInstance->Create_Sound(strFullPath, strSoundName)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoader::Load_UI()
{
	/* 추후 레벨에 맞게 LOAD_UI 구성 */
	// ==========================================Texture================================================================================

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_InstanceImg", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/UI/InstanceRect.png"))))
		return E_FAIL;

	wstring wstrUIPath(TEXT("../../Resources/UI/UI/"));

	if (FAILED(Load_UI_Texture(wstrUIPath.c_str())))
		return E_FAIL;

	// ==========================================Prototype================================================================================
	/* Base */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Instance_MonsterHP", CUI_Instance_MonsterHP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_SushiShop_PopupText", CUI_SushiShop_PopupText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 00. Intro_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Intro_Logo", CUI_Intro_Logo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 00. BattleResult */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Battle_Result", CUI_BattleResult::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 00. MainMenu */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_MainMenu", CUI_MainMenu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 01.InBattle */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_InBattle_Player", CUI_InBattle_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_InBattle_Boss", CUI_InBattle_Boss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_InBattle_Monster", CUI_InBattle_Monster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_InBattle_Named", CUI_InBattle_Named::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_InBattle_Letter", CUI_InBattle_Letter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_InBattle_Mechanic", CUI_InBattle_Mechanic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 02. CosmicBattle */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Cosmic", CUI_Cosmic_Battle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 03. PlayerStatus */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_PlayerStatus", CUI_PlayerStatus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_PlayerStatus_Obj", CUI_PlayerStatus_Obj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 04. PowerUP */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_PowerUp", CUI_PowerUp::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_PowerUp_Icon", CUI_PowerUp_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 05. Home */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Home", CUI_Home::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 06. SushiShop */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_SushiShop", CUI_SushiShop::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 08. TurnRPG */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Smash", CUI_Smash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 13. Mario */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Mario", CUI_Mario::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 10. ButtonA */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_MoveButton", CUI_MoveButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 11. ToiletCheckPoint */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_ToiletCheckPoint", CUI_ToiletCheckPoint::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 12. TurnRPG */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_TurnRPG", CUI_TurnRPG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_TurnRPG_AttackOption", CUI_TurnRPG_AttackOption::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 14. World City */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_World_City", CUI_World_City::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_World_City_Minimap", CUI_World_City_Minimap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_World_City_MinimapTarget", CUI_World_City_MinimapTarget::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 14. CCTV */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_CCTV", CUI_CCTV::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_CCTV_Chating", CUI_CCTV_Chating::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_CCTV_ChatIcon", CUI_CCTV_ChatIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_CCTV_SkipImg", CUI_CCTV_SkipImg::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 15. SubTitle */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_SubTitle", CUI_SubTitle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 16. MiniGame_Trash */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_MiniGame_Trash", CUI_MiniGame_Trash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 17. MiniGame_Toilet */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_MiniGame_Toilet", CUI_MiniGame_Toilet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* 17. Notice */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Notice", CUI_Notice::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// ==========================================LoadData================================================================================
	if (FAILED(UIMGR->LoadUISavedData(m_pDevice, m_pContext)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Load_UI_Texture(const _tchar* wszPath)
{
	fs::path UIPath(wszPath);
	for (const fs::directory_entry& entry : fs::directory_iterator(UIPath))
	{
		if (entry.is_directory())
		{
			wstring wstrPath(entry.path().c_str());
			wstrPath.append(TEXT("/"));
			if (FAILED(Load_UI_Texture(wstrPath.c_str())))
				return E_FAIL;
		}
		else
		{
			fs::path fileName = entry.path().filename();
			fs::path fileTitle = fileName.stem();
			fs::path fullPath = entry.path();
			fs::path ext = fileName.extension();
			if (fileName.extension().compare(".dds"))
				continue;

			string strFileName = fileName.string();
			wstring wstrTexDir(fullPath);

			/* N장의 이미지 넣을 경우 폴더명 "MultiImg" 으로 구분 */
			if (-1 != wstrTexDir.find(L"MultiImg"))
			{
				if (0 == m_iMultiImageCnt)
				{
					wstring sFindImgCnt = fileTitle.c_str();
					sFindImgCnt = sFindImgCnt.substr(0, 2);
					m_iMultiImageCnt = stoi(sFindImgCnt);

					wstring strMultiFileName = fileTitle.c_str();
					strMultiFileName = strMultiFileName.substr(0, strMultiFileName.length() - 1);
					strMultiFileName += L"%d";

					wstring MultiFilePath = wstrTexDir.substr(0, wstrTexDir.length() - strFileName.length());
					MultiFilePath += strMultiFileName + ext.c_str();

					if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, fileTitle,
						CTexture::Create(m_pDevice, m_pContext, MultiFilePath.c_str(), m_iMultiImageCnt))))
						return E_FAIL;
				}

				--m_iMultiImageCnt;
			}
			else
			{
				if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, fileTitle,
					CTexture::Create(m_pDevice, m_pContext, wstrTexDir.c_str()))))
					return E_FAIL;
			}
		}
	}
	return S_OK;
}

HRESULT CLoader::Load_NPC(_uint _iLevelIndex)
{
	switch (_iLevelIndex) {
	case (_uint)LEVEL_WORLD_CITY:
	{
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Texture_NPC_Woman_Variant_Bottom", CTexture::Create(m_pDevice, m_pContext,
			L"../../Resources/Models/NPC_Woman/Variant/TX_CH_NPC_Cast_Female_Bottoms_%02d.dds", 11))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Texture_NPC_Woman_Variant_Top", CTexture::Create(m_pDevice, m_pContext,
			L"../../Resources/Models/NPC_Woman/Variant/TX_CH_NPC_Cast_Female_Tops_%02d.dds", 13))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Texture_NPC_Man_Variant_Bottom", CTexture::Create(m_pDevice, m_pContext,
			L"../../Resources/Models/NPC_Man/Variant/TX_CH_NPC_Cast_Male_Bottoms_D_%02d.dds", 11))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Texture_NPC_Man_Variant_Top", CTexture::Create(m_pDevice, m_pContext,
			L"../../Resources/Models/NPC_Man/Variant/TX_CH_NPC_Cast_Male_Tops_D_%02d.dds", 13))))
			return E_FAIL;
		/* CarA */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Texture_NPC_CarA_Variant", CTexture::Create(m_pDevice, m_pContext,
			L"../../Resources/Models/NPC_CarA/Variant/TX_Vehicle_UtopiLand_CarA_Body_%02d.dds", 6))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_NPC_Woman", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/NPC_Woman/", "NPC_Woman.dat"))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_NPC_Man", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/NPC_Man/", "NPC_Man.dat"))))
			return E_FAIL;
		/* CarA */
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_NPC_CarA", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/NPC_CarA/", "NPC_CarA.dat"))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_NPC_Silvia",
			CModel::Create(m_pDevice, m_pContext, "../../Resources/Models/NPC_Silvia/", "NPC_Silvia.dat"))))
			return E_FAIL;

	}
	break;
	case (_uint)LEVEL_WORLD_HOME:
	{
		if (FAILED(m_pGameInstance->Add_Prototype(_iLevelIndex, L"Prototype_Model_NPC_Jeane", CModel::Create(m_pDevice, m_pContext,
			"../../Resources/Models/NPC_Jeane/", "Jeane.dat"))))
			return E_FAIL;
	}
	break;
	}

	return S_OK;
}


HRESULT CLoader::Load_Effects_Prototype(LEVEL eLevel, const wstring& wstrEffectPath)
{
	fs::path EffectPath(wstrEffectPath);
	for (const fs::directory_entry& entry : fs::directory_iterator(EffectPath))
	{
		if (entry.is_directory())
			continue;
		else
		{
			fs::path fileName = entry.path().filename();

			if (fileName.extension().compare(".json"))
				continue;

			if (FAILED(EFFECTMGR->Effect_Loader(eLevel, entry.path(), m_pDevice, m_pContext)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoader::Load_Trashes()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_WORLD_CITY_TRASHCOLLECTOR, L"Prototype_Model_Trash_Bomb", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/TrashCollectorObjects/", "Trash_Bomb.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_WORLD_CITY_TRASHCOLLECTOR, L"Prototype_Model_Trash_Bottle", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/TrashCollectorObjects/", "Trash_Bottle.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_WORLD_CITY_TRASHCOLLECTOR, L"Prototype_Model_Trash_Can", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/TrashCollectorObjects/", "Trash_Can.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_WORLD_CITY_TRASHCOLLECTOR, L"Prototype_Model_Trash_PET", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/TrashCollectorObjects/", "Trash_PET.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_WORLD_CITY_TRASHCOLLECTOR, L"Prototype_Model_Trash_Shoe", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/TrashCollectorObjects/", "Trash_Shoe.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_WORLD_CITY_TRASHCOLLECTOR, L"Prototype_Model_Trash_Tire", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/TrashCollectorObjects/", "Trash_Tire.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_WORLD_CITY_TRASHCOLLECTOR, L"Prototype_Model_Croc", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Croc/", "Croc.dat"))))
		return E_FAIL;

	return S_OK;
}

vector<wstring> CLoader::Find_Models(const wstring& strFilePath)
{
	vector<wstring> vecFileNames;
	wstring strPath = strFilePath + L"\\*.*";
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW(strPath.c_str(), &findFileData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			wstring wstrFileName = findFileData.cFileName;

			if (wstrFileName != L"." && wstrFileName != L"..") {
				if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					wstring subDirPath = strFilePath + L"\\" + wstrFileName;
					vector<wstring> vecSubFiles = Find_Models(subDirPath);
					vecFileNames.insert(vecFileNames.end(), vecSubFiles.begin(), vecSubFiles.end());
				}
				else if (wstrFileName.substr(wstrFileName.find_last_of(L".") + 1) == L"dat") {
					wstring baseName = wstrFileName.substr(0, wstrFileName.find_last_of(L"."));
					vecFileNames.push_back(baseName);
				}
			}
		} while (FindNextFileW(hFind, &findFileData) != 0);

		FindClose(hFind);
	}
	return vecFileNames;
}

vector<wstring> CLoader::Find_Sounds(const wstring& strFilePath)
{
	vector<wstring> vecFileNames;
	wstring strPath = strFilePath + L"\\*.*";
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW(strPath.c_str(), &findFileData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			wstring wstrFileName = findFileData.cFileName;

			if (wstrFileName != L"." && wstrFileName != L"..") {
				if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					wstring subDirPath = strFilePath + L"\\" + wstrFileName;
					vector<wstring> vecSubFiles = Find_Models(subDirPath);
					vecFileNames.insert(vecFileNames.end(), vecSubFiles.begin(), vecSubFiles.end());
				}
				else if (wstrFileName.substr(wstrFileName.find_last_of(L".") + 1) == L"wav" || wstrFileName.substr(wstrFileName.find_last_of(L".") + 1) == L"mp3") {
					//wstring baseName = wstrFileName.substr(0, wstrFileName.find_last_of(L"."));
					vecFileNames.push_back(wstrFileName);
				}
			}
		} while (FindNextFileW(hFind, &findFileData) != 0);

		FindClose(hFind);
	}
	return vecFileNames;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hThread);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}