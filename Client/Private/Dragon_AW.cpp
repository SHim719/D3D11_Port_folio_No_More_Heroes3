#include "Dragon_AW.h"

#include "AW_Manager.h"

#include "Enemy_AW.h"
#include "Bike_AW.h"

#include "Dragon_Breath.h"
#include "Effect_Manager.h"

_float		CDragon_AW::s_fAttackInterval = 1.5f;
_float		CDragon_AW::s_fAttack = 20.f;
_int		CDragon_AW::s_iSpawnSequence = 0;


CDragon_AW::CDragon_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CDragon_AW::CDragon_AW(const CDragon_AW& rhs)
	: CCharacter(rhs)
{
}


HRESULT CDragon_AW::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDragon_AW::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Breath()))
		return E_FAIL;

	/* Effect */
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	_float fRandomX = JoRandom::Random_Float(-2.5f, -1.f);
	fRandomX = s_iSpawnSequence == 1 ? fRandomX : JoRandom::Random_Float(1.f, 2.5f);

	s_iSpawnSequence = (s_iSpawnSequence + 1) % 2;

	_float fRandomZ = JoRandom::Random_Float(-1.f, 0.2f);

	m_vOffset = { fRandomX, 2.f, fRandomZ, 0.f };

	m_pModel->Change_Animation(0);

	m_pModel->Set_AnimPlay();

	m_pTransform->Set_Scale(0.02f);
	m_pTransform->Rotation(YAXIS, XM_PI);

	m_pShotPosBone = m_pModel->Get_Bone("WYVERN_-Ponytail1");
	Safe_AddRef(m_pShotPosBone);

	EFFECTMGR->Active_Effect(LEVEL_ARROW_A_ROW, "AA_Drangon_Spawn", &m_Effect_Desc);

	PLAY_SOUND("Dragon_Summon", false, 0.5f);

	return S_OK;
}


void CDragon_AW::PriorityTick(_float fTimeDelta)
{

}

void CDragon_AW::Tick(_float fTimeDelta)
{
	if (AWMGR->Is_Cinematic()) {
		m_bDestroyed = true;
		return;
	}

	if (AWMGR->Is_Stop())
		return;

	if (false == m_bEnd_SpawnEffect)
		m_fSpawnEffectTimeAcc += fTimeDelta;
	if (m_fSpawnEffectTimeAcc >= 0.5f)
	{
		m_bEnd_SpawnEffect = true;
	}

	_vector vPlayerPos = AWMGR->Get_Player()->Get_Transform()->Get_Position();

	m_pTransform->Set_Position(vPlayerPos + XMLoadFloat4(&m_vOffset));

	if (true == m_bEnd_SpawnEffect)
	{
		Check_Fire(fTimeDelta);
	}

	m_pModel->Play_Animation(fTimeDelta);

}

void CDragon_AW::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance->In_WorldFrustum(m_pTransform->Get_Position(), 1.f))
	{
		m_pModel->Update_BoneMatrices();
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_ShadowObject(this);
	}

#ifdef _DEBUG
	m_pGameInstance->Add_RenderComponent(m_pCollider);
#endif

}

HRESULT CDragon_AW::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_pTransform->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pModel->Bind_BoneMatrices(m_pShader)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		_int iTextureFlag = 0;

		if (FAILED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		//if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_NORMALS, "g_NormalTexture")))
		//	iTextureFlag |= (1 << (_uint)MATERIAL::NORMALS);
		//
		if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_MRAO, "g_MRAOTexture")))
			iTextureFlag |= (1 << (_uint)MATERIAL::MRAO);

		//if (SUCCEEDED(m_pModel->SetUp_OnShader(m_pShader, i, TextureType_EMISSIVE, "g_EmissiveTexture")))
		//	iTextureFlag |= (1 << (_uint)MATERIAL::EMISSIVE);

		if (FAILED(m_pShader->Bind_RawValue("g_iTextureFlag", &iTextureFlag, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pModel->Bind_Buffers(i)))
			return E_FAIL;

		if (FAILED(m_pModel->Render(m_pShader, i, m_iPassIdx)))
			return E_FAIL;
	}

	return S_OK;
}

void CDragon_AW::Check_Fire(_float fTimeDelta)
{
	m_fIntervalAcc += fTimeDelta;
	if (m_fIntervalAcc >= s_fAttackInterval)
	{
		PLAY_SOUND("Fireball", false, 1.f);

		ADD_EVENT(bind(&CDragon_AW::Spawn_Breath, this));
		m_fIntervalAcc = 0.f;
	}
}

void CDragon_AW::Spawn_Breath()
{
	m_Breathes[m_iBreathIdx]->OnEnter_Layer(nullptr);
	m_Breathes[m_iBreathIdx]->Get_Transform()->Attach_To_Bone(m_pShotPosBone, m_pTransform, XMMatrixIdentity(), true);

	_float fDist;
	CEnemy_AW* pEnemy = AWMGR->Find_NearestEnemy(fDist);
	if (pEnemy)
	{
		_vector vLook = XMVector3Normalize(pEnemy->Get_Transform()->Get_Position() - m_pTransform->Get_Position());
		m_Breathes[m_iBreathIdx]->Get_Transform()->LookTo(vLook);
	}
	else
	{
		m_Breathes[m_iBreathIdx]->Get_Transform()->LookTo(XMVectorSet(0.f, 0.f, 1.f, 0.f));
	}

	Safe_AddRef(m_Breathes[m_iBreathIdx]);
	m_pGameInstance->Insert_GameObject(GET_CURLEVEL, L"Layer_Breath", m_Breathes[m_iBreathIdx]);
	m_iBreathIdx = (m_iBreathIdx + 1) % m_Breathes.size();
}

HRESULT CDragon_AW::Ready_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Transform"), TEXT("Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Shader_VtxAnim"), TEXT("Shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(GET_CURLEVEL, TEXT("Prototype_Model_Dragon"), TEXT("Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDragon_AW::Ready_Breath()
{
	m_Breathes.resize(20);
	for (size_t i = 0; i < m_Breathes.size(); ++i)
		m_Breathes[i] = CDragon_Breath::Create(m_pDevice, m_pContext);


	return S_OK;
}

CDragon_AW* CDragon_AW::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDragon_AW* pInstance = new CDragon_AW(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDragon_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDragon_AW::Clone(void* pArg)
{
	CDragon_AW* pInstance = new CDragon_AW(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDragon_AW"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDragon_AW::Free()
{
	__super::Free();

	for (auto& pBreath : m_Breathes)
		Safe_Release(pBreath);

	m_Breathes.clear();

	Safe_Release(m_pShotPosBone);
}