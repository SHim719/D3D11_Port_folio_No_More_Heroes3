#include "Effect_Group_Tool.h"
#include "Effect_Mesh_Tool.h"
#include "Effect_Particle_Tool.h"
#include "Effect_Trail_Tool.h"

CEffect_Group_Tool::CEffect_Group_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice, pContext }
{
}

CEffect_Group_Tool::CEffect_Group_Tool(const CEffect_Group_Tool& rhs)
	: CGameObject(rhs)
	, m_Effects(rhs.m_Effects)
{
}

HRESULT CEffect_Group_Tool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Group_Tool::Initialize(void* pArg)
{
	return S_OK;
}

void CEffect_Group_Tool::Tick(_float fTimeDelta)
{
	for (auto& pEffect : m_Effects)
		pEffect->Tick(fTimeDelta);
}

void CEffect_Group_Tool::LateTick(_float fTimeDelta)
{
	for (auto& pEffect : m_Effects)
		pEffect->LateTick(fTimeDelta);

	Check_Destroy();
}

HRESULT CEffect_Group_Tool::OnEnter_Layer(void* pArg)
{
	m_bUsing = true;

	Start_Effect(pArg);

	return S_OK;
}

void CEffect_Group_Tool::Start_Effect(void* pArg)
{
	CEffect::CINE_EFFECT_DESC* pDesc = (CEffect::CINE_EFFECT_DESC*)pArg;
	for (auto& pEffect : m_Effects)
		pEffect->Reset_Effect(pDesc);
}


void CEffect_Group_Tool::End_Effect()
{
	m_bUsing = false;
}

void CEffect_Group_Tool::Check_Destroy()
{
	for (auto& pEffect : m_Effects)
	{
		if (false == pEffect->Get_End())
			return;
	}
	End_Effect();
}

void CEffect_Group_Tool::UpdateAll_Desc(CEffect::EFFECT_TYPE eEffect_Type, HR::EFFECT_COMMON_DESC tCommon, HR::EFFECT_COMMON_UV_DESC tCommonUV, HR::ATTACH_DESC tAttach, void* Desc)
{
	for (auto& pEffect : m_Effects)
	{
		if (eEffect_Type == pEffect->Get_EffetType())
		{
			pEffect->Set_AttachDesc(tAttach);
			pEffect->Update_UV_Desc(tCommonUV);

			switch (eEffect_Type)
			{
			case CEffect::MESH:
				dynamic_cast<CEffect_Mesh_Tool*>(pEffect)->Update_MeshEffect(*static_cast<HR::MESH_DESC*>(Desc), tCommon);
				break;
			case CEffect::PARTICLE:
				dynamic_cast<CEffect_Paticle_Tool*>(pEffect)->Update_Desc(*static_cast<HR::PARTICLE_INIT_DESC*>(Desc), tCommon);
				break;
			case CEffect::TRAIL:
				dynamic_cast<CEffect_Trail_Tool*>(pEffect)->Remake_Trail(*static_cast<TRAIL_DESC*>(Desc), tCommon);
				break;

			}
		}
	}
}

void CEffect_Group_Tool::Save_Group(ofstream& fout, nlohmann::ordered_json& jsonData)
{

	size_t iGroupSize = m_Effects.size();

	jsonData["Common"]["Group Data"]["Group Tag"] = m_szGroupTag;
	jsonData["Common"]["Group Data"]["Clone Number"] = m_iCloneNum;
	jsonData["Common"]["Group Data"]["Group Size"] = iGroupSize;

	for (size_t i = 0; i < iGroupSize; ++i)
	{
		jsonData["Common"]["Effect Tags"].push_back(m_Effects[i]->Get_EffectTag());
	}

	for (size_t i = 0; i < iGroupSize; ++i)
	{
		CEffect::EFFECT_TYPE eEffect_Type = m_Effects[i]->Get_EffetType();

		switch (eEffect_Type)
		{
		case CEffect::PARTICLE:
			if (FAILED(dynamic_cast<CEffect_Paticle_Tool*>(m_Effects[i])->Save_Effect(m_Effects[i]->Get_EffectTag(),jsonData)))
				MSG_BOX(TEXT("FAIL TO SAVE"));
			break;
		case CEffect::MESH:
			if (FAILED(dynamic_cast<CEffect_Mesh_Tool*>(m_Effects[i])->Save_Effect(m_Effects[i]->Get_EffectTag(),jsonData)))
				MSG_BOX(TEXT("FAIL TO SAVE"));
			break;
		case CEffect::TRAIL:
			if (FAILED(dynamic_cast<CEffect_Trail_Tool*>(m_Effects[i])->Save_Effect(m_Effects[i]->Get_EffectTag(),jsonData)))
				MSG_BOX(TEXT("FAIL TO SAVE"));
			break;
		}
	}
}


void CEffect_Group_Tool::Load_Group(json& jsonData)
{
	size_t iGroupSize = jsonData["Common"]["Group Data"]["Group Size"];

	m_iCloneNum = jsonData["Common"]["Group Data"]["Clone Number"];

	for (size_t i = 0; i < iGroupSize; ++i)
	{
		string strTag = jsonData["Common"]["Effect Tags"][i];
		CEffect::EFFECT_TYPE eEffect_Type = jsonData[strTag.c_str()]["Effect Type"];

		CEffect* pEffect = { nullptr };
		switch (eEffect_Type)
		{
		case CEffect::PARTICLE:
			pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleEffect")));
			if (FAILED(dynamic_cast<CEffect_Paticle_Tool*>(pEffect)->Load_Effect(strTag.c_str(), jsonData)))
				MSG_BOX(TEXT("FAIL TO LOAD"));
			break;
		case CEffect::MESH:
			pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_MeshEffect")));
			if (FAILED(dynamic_cast<CEffect_Mesh_Tool*>(pEffect)->Load_Effect(strTag.c_str(), jsonData)))
				MSG_BOX(TEXT("FAIL TO LOAD"));
			break;
		case CEffect::TRAIL:
			pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TrailEffect")));
			if (FAILED(dynamic_cast<CEffect_Trail_Tool*>(pEffect)->Load_Effect(strTag.c_str(), jsonData)))
				MSG_BOX(TEXT("FAIL TO LOAD"));
			break;
		}
		m_Effects.emplace_back(pEffect);


	}
}

void CEffect_Group_Tool::Load_Group_CINEMA(json& jsonData)
{
	size_t iGroupSize = jsonData["Common"]["Group Data"]["Group Size"];


	for (size_t i = 0; i < iGroupSize; i++)
	{
		string strTag = jsonData["Common"]["Effect Tags"][i];
		CEffect::EFFECT_TYPE eEffect_Type = jsonData[strTag.c_str()]["Effect Type"];

		CEffect* pEffect = { nullptr };

		switch (eEffect_Type)
		{
		case CEffect::PARTICLE:
			pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_ParticleEffect")));
			if (FAILED(dynamic_cast<CEffect_Paticle_Tool*>(pEffect)->Load_Effect_CINEMA(strTag.c_str(), jsonData)))
				MSG_BOX(TEXT("FAIL TO LOAD"));
			break;
		case CEffect::MESH:
			pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_MeshEffect")));
			if (FAILED(dynamic_cast<CEffect_Mesh_Tool*>(pEffect)->Load_Effect_CINEMA(strTag.c_str(), jsonData)))
				MSG_BOX(TEXT("FAIL TO LOAD"));
			break;
		case CEffect::TRAIL:
			pEffect = dynamic_cast<CEffect*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_TrailEffect")));
			if (FAILED(dynamic_cast<CEffect_Trail_Tool*>(pEffect)->Load_Effect_CINEMA(strTag.c_str(), jsonData)))
				MSG_BOX(TEXT("FAIL TO LOAD"));
			break;
		}
		m_Effects.emplace_back(pEffect);
	}
}


CEffect_Group_Tool* CEffect_Group_Tool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Group_Tool* pInstance = new CEffect_Group_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEffect_Trail_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Group_Tool::Clone(void* pArg)
{
	CEffect_Group_Tool* pInstance = new CEffect_Group_Tool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEffect_Group_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Group_Tool::Free()
{
	__super::Free();

	for (auto& pEffect : m_Effects)
		Safe_Release(pEffect);
}
