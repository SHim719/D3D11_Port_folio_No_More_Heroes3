#pragma once

#include "Base.h"
#include "GameInstance.h"

/*클라이언트개발자가 실질적으로 생성해낸 객체타입(Player, Monster)의 부모가 되는 클래스이다. */


BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

	virtual HRESULT OnEnter_Layer(void* pArg);
	virtual void OnExit_Layer();

	virtual void OnActive();
	virtual void OnInActive();

	virtual void PriorityTick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

	virtual HRESULT Render();
	virtual HRESULT Render_Picking();
	virtual HRESULT Render_Cascade();
	virtual HRESULT Render_ShadowDepth();

	virtual	_vector Get_Center() const;
	virtual _vector Get_TopPos() const;

protected:
	virtual HRESULT Bind_ShaderResources();
	virtual void Decide_PassIdx();

protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	class CGameInstance* m_pGameInstance = { nullptr };

protected:
	class CTransform* m_pTransform = nullptr;

	map<const wstring, class CComponent*>	m_Components;

protected:
	CTexture* m_pDissolveTexture = { nullptr };
	_bool							m_bDissolve = { false };
	_float							m_fDissolveAmount = { 0.f };
	_float							m_fDissolveSpeed = { 0.4f };
	_float4							m_vDissolveDiffuse = { 3.f,1.f,1.f,1.f };

protected:
	virtual HRESULT Bind_DissolveDescs();

public:
	void	Active_Dissolve();
	void	Update_Dissolve(_float fTimeDelta);

	_bool Is_Dissolving() const {
		return m_bDissolve;
	}

public:
	virtual _bool Bind_CinematicFunc(CINEMATIC_DESCS& CinematicDescs);
	virtual void Start_Cinematic(const CINEMATIC_DESCS& CinematicDescs);
	virtual void End_Cinematic();
	void Play_Cinematic(_float fPlayTime);

protected:
	string								m_strCinemaActorTag = { "" };
	CINEMATIC_DESCS						m_CinematicDescs = {};

	CINEMATIC_DESCS::CINE_FRAMES_ITER	m_ScaleFrameIt;
	CINEMATIC_DESCS::CINE_FRAMES_ITER	m_PositionFrameIt;
	CINEMATIC_DESCS::CINE_FRAMES_ITER	m_LookFrameIt;
	CINEMATIC_DESCS::CINE_EVENTS_ITER	m_EventFrameIt;

	size_t								m_EventChecked = { 0 };
	_int								m_iCinematicEventIdx = { 0 };

public:
	CTransform* Get_Transform() { return m_pTransform; }
	
public:
	HRESULT Add_Component(_uint iPrototoypeLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);
	HRESULT Erase_Component(const wstring& strComponentTag);
	class CComponent* Find_Component(const wstring& strComponentTag);

public:
	virtual void OnCollisionEnter(CGameObject* pOther, CCollider* pCollider) {}
	virtual void OnCollisionStay(CGameObject* pOther, CCollider* pCollider) {}
	virtual void OnCollisionExit(CGameObject* pOthe, CCollider* pColliderr) {}
	
	// BH - For Ray Checking
	virtual void RayArrived(const string& _strRayTag, _fvector _vRayOrigin, _fvector _vRayDir, _float _fDist, CGameObject* _pOther) {}
	virtual void OnRayCollision(const string& _strRayTag, _fvector _vRayOrigin, _fvector _vRayDir, _float _fDist, CGameObject* _pOther) {}

protected:
	_bool		m_bDestroyed = { false };
	_bool		m_bActive = { true };

	_bool		m_bReturnToPool = { false }; // For Object Pool
	_bool		m_bInLayer = { false };

	_uint		m_iTag = { 0 };

	_uint		m_iPassIdx = { 0 };

public:
	void Set_Destroy(_bool b) { m_bDestroyed = b; }
	_bool Is_Destroyed() { 
		return m_bDestroyed; }

	void Set_Active(_bool b);
	_bool Is_Active() { 
		return m_bActive; }

	_uint Get_Tag() const { 
		return m_iTag; }

	_bool Is_OutOfLayer() const {
		return m_bDestroyed || m_bReturnToPool;
	}

	void Set_ReturnToPool(_bool b) {
		m_bReturnToPool = b;
	}

	_bool Is_ReturnToPool() const {
		return m_bReturnToPool;
	}

	void Set_InLayer(_bool b) {
		m_bInLayer = b;
	}

	_bool Is_InLayer() const {
		return m_bInLayer;
	}

	_float Get_CamDistance() const {
		return XMVector3Length(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()) - m_pTransform->Get_Position()).m128_f32[0];
	}

	const string& Get_CinemaActorTag() const {
		return m_strCinemaActorTag;
	}

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	void Free() override;
};

END