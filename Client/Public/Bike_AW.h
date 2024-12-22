#pragma once

#include "Character.h"


BEGIN(Engine)
class CBone;
END


BEGIN(Client)

class CBike_AW final : public CCharacter
{
private:
	CBike_AW(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBike_AW(const CBike_AW& _rhs);
	virtual ~CBike_AW() = default;

public:
	HRESULT Initialize_Prototype()				override;
	HRESULT Initialize(void* _pArg)				override;
	void	PriorityTick(_float _fTimeDelta)	override;
	void	Tick(_float _fTimeDelta)			override;
	void	LateTick(_float _fTimeDelta)		override;
	HRESULT Render()							override;

	void Hit(const ATTACKDESC& AttackDesc)		override;
private:
	void Bind_KeyFrames()						override;
	void Bind_KeyFrameEffects()					override;
	void Bind_KeyFrameSounds()					override;

	_vector Get_Center() const					override {
		return m_pTransform->Get_Position();
	}

	void Start_Cinematic(const CINEMATIC_DESCS& CinematicDescs) override;
	void End_Cinematic() override;

private:
	void OnCollisionEnter(CGameObject* _pOther, CCollider* pCollider)	override;
	void OnCollisionExit(CGameObject* _pOther, CCollider* pCollider)	override;

public:
	void Adjust_Ability(ABILITY_TYPE eAbility, _float fValue);
	void Active_SuperMode();
	void Inactive_SuperMode();

	_float Get_Hp() const {
		return m_fCurHp;
	}

	_bool Is_SuperMode() const {
		return m_bSuperMode;
	}

	void Add_JeanCount() {
		m_iJeanCount++;
	}

	void Add_DragonCount() {
		m_iDragonCount++;
	}

	_uint Get_JeanCount() const {
		return m_iJeanCount;
	}

	_uint Get_DragonCount() const {
		return m_iDragonCount;
	}

	void Add_HealingAmount(){
		m_fHealingRatio += 2.f;
	}

	void Add_MaxHp(_float fVal) {
		m_fMaxHp += fVal;
		m_fCurHp += fVal;
	}


private:
	CGameObject*						m_pTravis = { nullptr };
	CBone*								m_pBone_Tire_F = { nullptr };
	CBone*								m_pBone_Tire_R = { nullptr };
	CBone*								m_pBone_Tire_L = { nullptr };

	_float								m_fMaxHp = { 100.f };
	_float								m_fCurHp = { 100.f };
	class CHP_Number*					m_pHpNumber = { nullptr };

	_float								m_fHealingRatio = { 20.f };
	
	_uint								m_iJeanCount = { 0 };
	_uint								m_iDragonCount = { 0 };

	_bool								m_bSuperMode = { false };
	_bool								m_bCinematic = { false };

	CINEMATIC_DESCS						m_CinemaDesc = {};
private:
	vector<class CPlayerBullet_AW*>		m_Bullets;
	_float								m_fFireAcc = { 0.f };		
	_float								m_fFireInterval = { 1.f };

	size_t								m_iBulletIdx = { 0 };

	_float								m_fTireRotation = { 0.f };
private:
	void Key_Input(_float fTimeDelta);
	void Rotate_Tires(_float fTimeDelta);
	void Check_Fire(_float fTimeDelta);
	void Spawn_Bullet();

private:
	HRESULT Ready_Components();
	HRESULT Ready_Bullets();
	HRESULT Ready_TireBones();

public:
	static CBike_AW* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg)	override;
	void Free() override;

};

END

