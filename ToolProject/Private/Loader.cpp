#include "Loader.h"

#include "GameInstance.h"

#include "ToolAnimObj.h"
#include "ToolMapObj.h"
#include "ToolTriggerObj.h"
#include "ToolColliderObj.h"
#include "Tool_Weapon.h"
#include "TestGround.h"
#include "Terrain.h"
#include "Grid.h"
#include "Water.h"
#include "Effect_Particle_Tool.h"
#include "Effect_Mesh_Tool.h"
#include "Effect_Trail_Tool.h"
#include "Effect_Group_Tool.h"
#include "ToolUIObj.h"
#include "Free_Camera.h"
#include "Grass_Cube.h"

#include "CinemaObject.h"
#include "Cinema_Camera.h"




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

	switch (m_eNextLevelID)
	{
	case LEVEL_ANIMTOOL:
		hr = Loading_For_AnimTool();
		break;
	case LEVEL_MAPTOOL:
		hr = Loading_For_MapTool();
		break;
	case LEVEL_UITOOL:
		hr = Loading_For_UiTool();
		break;
	case LEVEL_EFFECTTOOL:
		hr = Loading_For_EffectTool();
		break;
	case LEVEL_CINEMATOOL:
		hr = Loading_For_CinemaTool();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return 1;

	return 0;

}


HRESULT CLoader::Loading_For_AnimTool()
{
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_ToolAnimObj", CToolAnimObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_ToolColliderObj", CToolColliderObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_ToolWeapon", CTool_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_TestGround", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/TestGround.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_TestGround", CTestGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(Load_Travis()))
		return E_FAIL;

	if (FAILED(Load_Mbone()))
		return E_FAIL;

	/* Boss_MrBlackhole */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_MrBlackhole", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/MrBlackhole/", "MrBlackhole.dat"))))
		return E_FAIL;

	/* Boss_MrBlackhole_ArmL */ 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_MrBlackhole_ArmL", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/MrBlackhole/", "MrBlackhole_ArmL.dat"))))
		return E_FAIL;

	/* Boss_MrBlackhole_ArmR */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_MrBlackhole_ArmR", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/MrBlackhole/", "MrBlackhole_ArmR.dat"))))
		return E_FAIL;

	/* Boss_MrBlackhole_Space */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_MrBlackhole_Space", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/MrBlackhole/", "MrBlackhole_Space.dat"))))
		return E_FAIL;

	/* Treatment */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Treatment", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Treatment/", "Treatment.dat"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Treatment_Tomahawk", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Treatment/", "Tomahawk.dat"))))
		return E_FAIL;

	/* Fullface */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Fullface", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Fullface/", "Fullface.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Fullface_Sword", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Fullface/", "FullfaceSword.dat"))))
		return E_FAIL;
	/* Fullface_Sheath */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Fullface_Sheath", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Fullface/", "FullfaceSheath.dat"))))
		return E_FAIL;
		
	/* Tripleput */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Tripleput", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Tripleput/", "Tripleput.dat"))))
		return E_FAIL;

	/* SonicJuice */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_SonicJuice", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/SonicJuice/", "SonicJuice.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Travis_Bike", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Travis_Bike/", "Travis_Bike.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_SmashBros_Damon", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/SmashBros_Damon/", "SmashBros_Damon.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Leopardon", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Leopardon/", "Leopardon.dat"))))
		return E_FAIL;

	/* DestroymanTF */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_DestroymanTF", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/DestroymanTF/", "DestroymanTF.dat"))))
		return E_FAIL;

	/* Jean */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_NPC_Jeane", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/NPC_Jeane/", "Jeane.dat"))))
		return E_FAIL;

	Loading_For_EffectTool();

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_MapTool()
{
	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_TestGround", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/TestGround.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Texture_Grass_Cube"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Models/MapObjects/Map_Boss/CubeMesh/Tex/Grass_Cube.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_Terrain",
		CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Models/MapObjects/Map_Normal/LakeSideCabin/Tex/grass_fall_a_D.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Grass_Cube", CGrass_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Water", CWater::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_TestGround", CTestGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Terrain", CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Grid", CGrid::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_ToolMapObj", CToolMapObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_ToolTriggerObj", CToolTriggerObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Navigation", CNavigation::Create(m_pDevice, m_pContext, L"../../Resources/DataFiles/NavData/Test.dat"))))
		return E_FAIL;



	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_UiTool()
{
	// UI 관련 텍스쳐 한번에 가져오기
	wstring wstrUIPath(TEXT("../../Resources/UI/"));

	if (FAILED(Load_UI_Texture(wstrUIPath.c_str())))
		return E_FAIL;
	// UI 관련 텍스쳐 한번에 가져오기

	if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, L"Prototype_Texture_UI_TestImg", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/UI/InstanceRect.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_ToolUIObj", CToolUIObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_CinemaTool()
{
	// Effect Textures.
	wstring wstrTexPath(TEXT("../../Resources/Effect/Texture/"));

	if (FAILED(Load_Textures(wstrTexPath.c_str())))
		return E_FAIL;

	// Effect Meshes.
	wstring wstrMeshPath(TEXT("../../Resources/Effect/Mesh/"));

	if (FAILED(Load_Meshes(wstrMeshPath.c_str())))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_CinemaObject", CCinemaObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_Water", CWater::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_ToolMapObj", CToolMapObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ParticleEffect"), CEffect_Paticle_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MeshEffect"), CEffect_Mesh_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrailEffect"), CEffect_Trail_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GroupEffect"), CEffect_Group_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	

	// Battle Ground
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Travis", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Players/Player_Battle_Ground/Player_NMH1/", "Player_NMH1.dat"))))
		return E_FAIL;

	// Battle_Space
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Travis_Space", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Players/Player_Battle_Space/Player_Space/", "Player_Space.dat"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Travis_World", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Players/Player_World/Player_NMH1_World/", "Player_NMH1_World.dat"))))
		return E_FAIL;

	// Battle Ground Armor
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Travis_Armor", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Players/Player_Battle_Ground/Player_Armor/", "Player_Armor.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Mbone", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Mbone/", "Mbone.dat"))))
		return E_FAIL;

	/* Boss_MrBlackhole */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_MrBlackhole", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/MrBlackhole/", "MrBlackhole.dat"))))
		return E_FAIL;

	/* Boss_MrBlackhole_Space */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_MrBlackhole_Space", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/MrBlackhole/", "MrBlackhole_Space.dat"))))
		return E_FAIL;

	/* Treatment */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Treatment", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Treatment/", "Treatment.dat"))))
		return E_FAIL;


	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Treatment_Tomahawk", CModel::Create(m_pDevice, m_pContext,
	//	"../../Resources/Models/Treatment/", "Tomahawk.dat"))))
	//	return E_FAIL;

	/* Fullface */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Fullface", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Fullface/", "Fullface.dat"))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Fullface_Sword", CModel::Create(m_pDevice, m_pContext,
	//	"../../Resources/Models/Fullface/", "FullfaceSword.dat"))))
	//	return E_FAIL;
	///* Fullface_Sheath */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Fullface_Sheath", CModel::Create(m_pDevice, m_pContext,
	//	"../../Resources/Models/Fullface/", "FullfaceSheath.dat"))))
	//	return E_FAIL;

	/* Tripleput */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Tripleput", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Tripleput/", "Tripleput.dat"))))
		return E_FAIL;

	/* SonicJuice */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_SonicJuice", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/SonicJuice/", "SonicJuice.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Travis_Bike", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Travis_Bike/", "Travis_Bike.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_SmashBros_Damon", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/SmashBros_Damon/", "SmashBros_Damon.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_Leopardon", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Leopardon/", "Leopardon.dat"))))
		return E_FAIL;

	/* DestroymanTF */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_DestroymanTF", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/DestroymanTF/", "DestroymanTF.dat"))))
		return E_FAIL;

	/* Jean */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_CINEMATOOL, L"Prototype_Model_NPC_Jeane", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/NPC_Jeane/", "Jeane.dat"))))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_EffectTool()
{
	// Effect Textures.
	wstring wstrTexPath(TEXT("../../Resources/Effect/Texture/"));

	if (FAILED(Load_Textures(wstrTexPath.c_str())))
		return E_FAIL;

	// Effect Meshes.
	wstring wstrMeshPath(TEXT("../../Resources/Effect/Mesh/"));

	if (FAILED(Load_Meshes(wstrMeshPath.c_str())))
		return E_FAIL;

	// Obj Prototypes
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ParticleEffect"), CEffect_Paticle_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MeshEffect"), CEffect_Mesh_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrailEffect"), CEffect_Trail_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GroupEffect"), CEffect_Group_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_bIsFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Meshes(const _tchar* wszPath)
{
	fs::path MeshPath(wszPath);
	for (const fs::directory_entry& entry : fs::directory_iterator(MeshPath))
	{

		if (entry.is_directory())
		{
			wstring wstrPath(entry.path().c_str());
			wstrPath.append(TEXT("/"));
			if (FAILED(Load_Meshes(wstrPath.c_str())))
				return E_FAIL;
		}
		else
		{
			fs::path fileName = entry.path().filename();
			fs::path fileTitle = fileName.stem();
			fs::path fullPath = entry.path();
			fs::path ext = fileName.extension();
			if (fileName.extension().compare(".dat"))
				continue;

			string strFileName = fileName.string();
			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, fileTitle,
				CModel::Create(m_pDevice, m_pContext, fullPath.parent_path().string() + "/", fileName.string()))))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CLoader::Load_Textures(const _tchar* wszPath)
{
	fs::path TexturePath(wszPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(TexturePath))
	{
		if (entry.is_directory())
		{
			wstring wstrPath(entry.path().c_str());
			wstrPath.append(TEXT("/"));

			if (FAILED(Load_Textures(wstrPath.c_str())))
				return E_FAIL;
		}
		else
		{
			fs::path fileName = entry.path().filename();
			fs::path fileTitle = fileName.stem();
			fs::path fullPath = entry.path();

			if (fileName.extension().compare(".dds"))
				continue;

			if (FAILED(m_pGameInstance->Add_Prototype(m_eNextLevelID, fileTitle,
				CTexture::Create(m_pDevice, m_pContext, fullPath.c_str()))))
				return E_FAIL;
		}
	}

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

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_UITOOL, fileTitle,
				CTexture::Create(m_pDevice, m_pContext, wstrTexDir.c_str()))))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CLoader::Load_Travis()
{
	// Battle Ground
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Travis", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Players/Player_Battle_Ground/Player_NMH1/", "Player_NMH1.dat"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Travis_BeamKatana", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Players/Player_Battle_Ground/Player_NMH1/", "Travis_Weapon.dat"))))
		return E_FAIL;

	// Battle_Space
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Travis_Space", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Players/Player_Battle_Space/Player_Space/", "Player_Space.dat"))))
		return E_FAIL;

	// World
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Travis_World", CModel::Create(m_pDevice, m_pContext,
	//	"../../Resources/Models/Players/Player_World/Player_NMH1_Trash/", "Player_NMH1_Trash.dat"))))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Travis_World", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Players/Player_World/Player_NMH1_World/", "Player_NMH1_World.dat"))))
		return E_FAIL;

	// Battle Ground Armor
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Travis_Armor", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Players/Player_Battle_Ground/Player_Armor/", "Player_Armor.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Travis_Armor_Weapon", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Players/Player_Battle_Ground/Player_Armor/", "Player_Armor_Weapon.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Load_Mbone()
{
	/* Mbone */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Mbone", CModel::Create(m_pDevice, m_pContext,
		"../../Resources/Models/Mbone/", "Mbone.dat"))))
		return E_FAIL;
//	/* Mbone_BoneSpear */
//	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_ANIMTOOL, L"Prototype_Model_Mbone_BoneSpear", CModel::Create(m_pDevice, m_pContext,
//		"../../Resources/Models/Mbone/", "BoneSpear.dat"))))
//		return E_FAIL;

	return S_OK;
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

