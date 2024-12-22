#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Levels_Header.h"

#include "MapLoader.h"

#include "Instancing_Object.h"
#include "Effect_Manager.h"
#include "MapLoader.h"

#include "UI_Manager.h"
#include "UI_Loading.h"

#include "Travis_Manager.h"
//#include "Effect_Mesh.h"
//#include "Effect_Particle.h"

#include "Virtual_Bone.h"
#include "FadeScreen.h"

#include "Cinematic_Manager.h"

#include "ImGui_Shader.h"

CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	GRAPHIC_DESC		GraphicDesc{};
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	GraphicDesc.isWindowed = true;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Font(L"BasicFont", L"../../Resources/Font/Kong_Font1.spritefont")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(L"Font_AW", L"../../Resources/Font/TestAW.spritefont")))
		return E_FAIL;

	if (FAILED(Load_Effects()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Dissolve", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/DissolveNoise.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_Dissolve_for_Crystal", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/DissolveNoiseForCrystal.dds"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Texture_FadeScreen", CTexture::Create(m_pDevice, m_pContext,
		L"../../Resources/FadeScreen.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_GameObject_FadeScreen", CFadeScreen::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_pGameInstance->Set_PBR(true);
	m_pGameInstance->Set_Active_Shadow(true);
	m_pGameInstance->Set_Active_SSAO(true);
	//m_pGameInstance->Set_Active_DOF(true);
	m_pTravis_Manager = CTravis_Manager::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTravis_Manager)
		return E_FAIL;

	m_pImGui_Shader = CImGui_Shader::Create(m_pDevice, m_pContext);

	m_pGameInstance->Change_Level(CLevel_Loading::Create(LEVEL_MAINMENU, m_pDevice, m_pContext));
	 
	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);

	++m_iFrameCnt;
	m_fFrameAcc += fTimeDelta;

	// Shutdown
	if (KEY_PUSHING(eKeyCode::LAlt) && KEY_PUSHING(eKeyCode::F4)) {
		PostQuitMessage(0);
	}

	if (KEY_DOWN(eKeyCode::F7)) {
		m_bActiveShaderOptions = !m_bActiveShaderOptions;
	}

	if (m_bActiveShaderOptions)
		m_pImGui_Shader->Tick(fTimeDelta);

}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.3f, 0.3f, 0.3f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pGameInstance->Draw();

	if (m_bActiveShaderOptions)
		m_pImGui_Shader->Render();

	m_pGameInstance->Present();

	if (m_fFrameAcc > 1.f)
	{
		SetWindowTextA(g_hWnd, to_string(m_iFrameCnt).c_str());
		m_fFrameAcc = 0.f;
		m_iFrameCnt = 0;
	}

	return S_OK;
}


HRESULT CMainApp::Ready_Prototype_Component()
{

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Rect_NorTex"),
		CVIBuffer_Rect_NorTex::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Point"),
		CVIBuffer_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Particle"),
		CVIBuffer_Particle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_CellPoint"),
		CVIBuffer_Sphere::Create(m_pDevice, m_pContext, 3, 3))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Trail"),
		CVIBuffer_SwordTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 70, 70))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VIBuffer_SkyPlane"),
		CVIBuffer_SkyPlane::Create(m_pDevice, m_pContext, 20, 5.f, 0.4f, 0.f, 4))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_AABB"),
		CAABB::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_OBB"),
		COBB::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Sphere"),
		CSphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Rigidbody"),
		CRigidbody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_VirtualBone"),
		CVirtual_Bone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Shader()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_UI()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Shader()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE_DECLARATION::Elements, VTXCUBE_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxModelInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnimInstance.hlsl"), VTXMODELINSTANCE_DECLARATION::Elements, VTXMODELINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnim.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_SonicJuice"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_SonicJuice.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements, false))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_Cell"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_Particle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Particle.hlsl"), VTXPARTICLE_DECLARATION::Elements, VTXPARTICLE_DECLARATION::iNumElements))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_MeshEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_MeshEffect.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_EffectTrail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_EffectTrail.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_UI_Instance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI_Instance.hlsl"), VTXUI_DECLARATION::Elements, VTXUI_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxTerrain"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTerrain.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_Water"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Water.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_VtxQuad"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxQuad.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Shader_Compute"), 
		CCompute_Shader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Compute_Particle.hlsl")))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_UI()
{
	/* 추후 메인화면 UI 프로토타입 추가 예정 */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_BasicBase", CUI_BasicBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_InstanceBase", CUI_InstanceBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* Loading 관련 */
	if (FAILED(m_pGameInstance->Add_Prototype(L"Prototype_UI_Loading", CUI_Loading::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Load_Effects()
{
	EFFECTMGR->Initialize(LEVEL_END);
	if (FAILED(Load_EffectTexture(TEXT("../../Resources/Effect/Texture/"))))
		return E_FAIL;

	if (FAILED(Load_EffectMesh(TEXT("../../Resources/Effect/Mesh/"))))
		return E_FAIL;

	if (FAILED(Load_Travis_Effect(TEXT("../../Resources/Effect/SaveData/Travis/"))))
		return E_FAIL;

	if (FAILED(Load_Blood_Effect(TEXT("../../Resources/Effect/SaveData/Blood_Red/"))))
		return E_FAIL;

	if (FAILED(EFFECTMGR->Effect_Loader(LEVEL_STATIC, TEXT("../../Resources/Effect/SaveData/Etc/Monster_Attack_In.json"), m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(EFFECTMGR->Effect_Loader(LEVEL_STATIC, TEXT("../../Resources/Effect/SaveData/Etc/Monster_Attack_In_Fast.json"), m_pDevice, m_pContext)))
		return E_FAIL;


	EFFECTMGR->Effect_Loader(LEVEL_STATIC, TEXT("../../Resources/Effect/SaveData/Etc/Portal_Circle.json"), m_pDevice, m_pContext);
	EFFECTMGR->Effect_Loader(LEVEL_STATIC, TEXT("../../Resources/Effect/SaveData/Etc/Monster_WarpEffect.json"), m_pDevice, m_pContext);

	
	return S_OK;
}

HRESULT CMainApp::Load_EffectTexture(const wstring& wstrTexturePath)
{
	fs::path TexturePath(wstrTexturePath);

	for (const fs::directory_entry& entry : fs::directory_iterator(TexturePath))
	{
		if (entry.is_directory())
		{
			wstring wstrPath(entry.path().c_str());
			wstrPath.append(TEXT("/"));

			if (FAILED(Load_EffectTexture(wstrPath)))
				return E_FAIL;
		}
		else
		{
			fs::path fileName = entry.path().filename();
			fs::path fileTitle = fileName.stem();
			fs::path fullPath = entry.path();

			if (fileName.extension().compare(".dds"))
				continue;

			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, fileTitle,
				CTexture::Create(m_pDevice, m_pContext, fullPath.c_str()))))
				return E_FAIL;
		}
	}
 
	return S_OK;
}

HRESULT CMainApp::Load_EffectMesh(const wstring& wstrMeshPath)
{

	fs::path MeshPath(wstrMeshPath);
	for (const fs::directory_entry& entry : fs::directory_iterator(MeshPath))
	{

		if (entry.is_directory())
		{
			wstring wstrPath(entry.path().c_str());
			wstrPath.append(TEXT("/"));
			if (FAILED(Load_EffectMesh(wstrPath.c_str())))
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
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, fileTitle,
				CModel::Create(m_pDevice, m_pContext, fullPath.parent_path().string() + "/", fileName.string()))))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CMainApp::Load_Travis_Effect(const wstring& wstrEffetPath)
{
	fs::path EffectPath(wstrEffetPath);
	for (const fs::directory_entry& entry : fs::directory_iterator(EffectPath))
	{

		if (entry.is_directory())
			continue;
		else
		{
			fs::path fileName = entry.path().filename();

			if (fileName.extension().compare(".json"))
				continue;

			if (FAILED(EFFECTMGR->Effect_Loader(LEVEL_STATIC,entry.path(), m_pDevice, m_pContext)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CMainApp::Load_Blood_Effect(const wstring& wstrEffetPath)
{
	fs::path EffectPath(wstrEffetPath);
	for (const fs::directory_entry& entry : fs::directory_iterator(EffectPath))
	{

		if (entry.is_directory())
			continue;
		else
		{
			fs::path fileName = entry.path().filename();

			if (fileName.extension().compare(".json"))
				continue;

			if (FAILED(EFFECTMGR->Effect_Loader(LEVEL_STATIC, entry.path(), m_pDevice, m_pContext)))
				return E_FAIL;
		}
	}

	return S_OK;
}


CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{	
	__super::Free();

	CModel::Release_Textures();
	CUI_Manager::Destroy_Instance();
	CMapLoader::Destroy_Instance();
	Safe_Release(m_pTravis_Manager);
	Safe_Release(m_pImGui_Shader);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);	

	CGameInstance::Release_Engine();
	CEffect_Manager::Destroy_Instance();
	CCinematic_Manager::Destroy_Instance();
}

