#include "Map_Chunk.h"

#include "Travis_AW.h"

#include "MapObj.h"

#include "AW_Manager.h"


CMap_Chunk::CMap_Chunk(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CMap_Chunk::CMap_Chunk(const CMap_Chunk& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CMap_Chunk::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMap_Chunk::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return E_FAIL;

	MAP_CHUNK_DESC* pDesc = (MAP_CHUNK_DESC*)_pArg;

	m_vecMapObjects.reserve(pDesc->ObjInfos.size());

	for (auto& ObjInfo : pDesc->ObjInfos)
	{
		m_vecMapObjects.emplace_back(static_cast<CMapObj*>(m_pGameInstance->Add_Clone(GET_CURLEVEL, L"Layer_MapObj", L"Prototype_MapObj", &ObjInfo)));
		m_OriginWorldMatrices.emplace_back(ObjInfo.WorldMatrix);
	}
		
	m_fChunkSize = pDesc->fChunkSize;
	m_fOffset = pDesc->fOffset;
	m_fThreshold = pDesc->fThreshold;

	return S_OK;
}

void CMap_Chunk::PriorityTick(_float _fTimeDelta)
{
}

void CMap_Chunk::Tick(_float _fTimeDelta)
{
	if (false == AWMGR->Is_Stop() && false == m_bCinematic)
	{
		_float fMoveSpeed = AWMGR->Is_SuperMode() == true? m_fMoveSpeed * 3.f : m_fMoveSpeed;

		m_fOffset -= fMoveSpeed * _fTimeDelta;
		Move_Chunk();
	}
}

void CMap_Chunk::LateTick(_float _fTimeDelta)
{

}

void CMap_Chunk::Move_Chunk()
{
	size_t iIdx = 0;
	for (_float4x4& WorldMatrix : m_OriginWorldMatrices)
	{
		CALC_TF->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));
		CALC_TF->Add_Position(XMVectorSet(0.f, 0.f, m_fOffset, 0.f), false);
		m_vecMapObjects[iIdx++]->Get_Transform()->Set_WorldMatrix(CALC_TF->Get_WorldMatrix());
	}

}

CMap_Chunk* CMap_Chunk::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMap_Chunk* pInstance = new CMap_Chunk(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(L"Failed to Create : CMap_Chunk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMap_Chunk::Clone(void* _pArg)
{
	CMap_Chunk* pInstance = new CMap_Chunk(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMap_Chunk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMap_Chunk::Free()
{
	__super::Free();

}
