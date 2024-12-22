#pragma once

#include "GameObject.h"
#include "MapObj.h"

BEGIN(Client)

class CMap_Chunk final : public CGameObject
{
public:
	typedef struct tagMapChunkDesc
	{
		vector<CMapObj::MAPOBJ_DESC> ObjInfos;
		_float	fChunkSize = { 1.f };
		_float	fOffset = { 0.f };
		_float	fThreshold = { 0.f };
	} MAP_CHUNK_DESC;

private:
	CMap_Chunk(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMap_Chunk(const CMap_Chunk& _rhs);
	virtual ~CMap_Chunk() = default;

public:
	HRESULT Initialize_Prototype()				override;
	HRESULT Initialize(void* _pArg)				override;
	void	PriorityTick(_float _fTimeDelta)	override;
	void	Tick(_float _fTimeDelta)			override;
	void	LateTick(_float _fTimeDelta)		override;

	void	Move_Chunk();


	// 마지막에 있는 맵오브젝트의 z값이 임계값보다 작으면 맵을 다시 뒤로 돌릴거임.
	_bool   Is_Exceed() {
		return m_vecMapObjects[m_vecMapObjects.size() - 1]->Get_Transform()->Get_Position().m128_f32[2] < m_fThreshold;
	} 

	_float	Get_ChunkSize() const {
		return m_fChunkSize;
	}

	void Set_Offset(_float fOffset) {
		m_fOffset = fOffset;
	}

	_float Get_Offset() const {
		return m_fOffset;
	}

	void Add_Speed(_float fVal) {
		m_fMoveSpeed += fVal;
	}

	void Active_Cinematic(_bool bCinematic) {
		m_bCinematic = true;
		Move_Chunk();
	}

	void Set_Threshold(_float fThreshold) {
		m_fThreshold = fThreshold;
	}

private:
	vector<CMapObj*>			m_vecMapObjects;
	vector<_float4x4>			m_OriginWorldMatrices;
	_float						m_fOffset = { 0.f };
	_float						m_fChunkSize = { 0.f };
	_float						m_fThreshold = { 0.f }; 
	_float						m_fMoveSpeed = { 20.f };

	_bool						m_bCinematic = { false };
public:
	static CMap_Chunk* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;

};

END

