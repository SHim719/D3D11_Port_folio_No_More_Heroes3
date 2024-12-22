#pragma once

#include "Character.h"

#include "Travis_World_Enums.h"
#include "Travis_World_States.h"
#include "FadeScreen.h"
#include "Travis_Structs.h"

BEGIN(Engine)
class CModel;
class CNavigation;
END

BEGIN(Client)

class CTravis_World final : public CCharacter
{
private:
	CTravis_World(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTravis_World(const CTravis_World& _rhs);
	virtual ~CTravis_World() = default;

public:
	virtual HRESULT Initialize_Prototype()				override;
	virtual HRESULT Initialize(void* _pArg)				override;
	virtual void	PriorityTick(_float _fTimeDelta)	override;
	virtual void	Tick(_float _fTimeDelta)			override;
	virtual void	LateTick(_float _fTimeDelta)		override;
	virtual HRESULT Render()							override;
	virtual HRESULT Render_Cascade()					override;
	virtual HRESULT Render_ShadowDepth()				override;

private:
	virtual void Bind_KeyFrames()						override;
	virtual void Bind_KeyFrameEffects()					override;
	virtual void Bind_KeyFrameSounds()					override;

public:
	virtual void OnCollisionEnter(CGameObject* _pOther, CCollider* _pCollider)	override;
	void OnCollisionTriggerEnter(CGameObject* _pOther, CCollider* _pCollider);
	virtual void OnCollisionStay(CGameObject* _pOther, CCollider* _pCollider)	override;
	void OnCollisionTriggerStay(CGameObject* _pOther, CCollider* _pCollider);
	virtual void OnCollisionExit(CGameObject* _pOther, CCollider* _pCollider)	override;
	void OnCollisionTriggerExit(CGameObject* _pOther, CCollider* _pCollider);

public:
	virtual HRESULT OnEnter_Layer(void* _pArg) override;
	virtual _vector Get_Center() const override;
	virtual _vector Get_TopPos() const override;

private:
	virtual HRESULT Bind_ShaderResources()				override;
	HRESULT Ready_Components(const TRAVIS_INITIALIZE_DESC& _tagCloneDesc);
	HRESULT Ready_States();
	HRESULT Ready_Weapons();
	HRESULT Ready_Camera();

private:
	CModel* m_pModel_Camera = { nullptr };
	CNavigation* m_pNavigation = { nullptr };
	class CVirtual_Bone* m_pVirtualBone = { nullptr };

private:
	pair<_bool, CModel*> m_SubModels[(_uint)WORLDTRAVIS_SUBMODELLIST::SUBMODELLIST_END] = {};
	_uint m_iCurModelIndex = { (_uint)WORLDTRAVIS_SUBMODELLIST::PLAYER_NMH1_TOILET };
	CModel* m_pRenderModel = { nullptr };

private:
	class CVarious_Camera* m_pPlayerCamera = { nullptr };
	CFadeScreen* m_pFadeScreen = { nullptr };

private:
	WORLDTRAVIS_RUNNING_TOGGLE m_eCurRunningToggle = { WORLDTRAVIS_RUNNING_TOGGLE::TOGGLE_END };
	_uint m_iCurPlayerPlace = { 0 };
	CTravis_World_State_Base::STATE_CHANGE_DESC m_tagSCD = {};
	_bool m_bControlLock = { false };

public:
	WORLDTRAVIS_RUNNING_TOGGLE Get_CurRunningToggle() const { return m_eCurRunningToggle; }
	class CVarious_Camera* Get_PlayerCamera() { return m_pPlayerCamera; }
	CModel* Get_PlayerCameraModel() { return m_pModel_Camera; }
	_vector Get_VirtualBonePos() const;
	_bool Get_ControlLock() const { return m_bControlLock; }
	_vector Get_PelvisPos();

public:
	void Apply_FadeIO(CFadeScreen::FADEDESC& _tagFadeDesc);
	void Activate_SubModel(_uint _iSubModelIndex);
	void DeActivate_SubModel(_uint _iSubModelIndex);
	void Set_ControlLock(_bool _bVal);
	void Set_EatSushi(_bool _bVal);
	void Disable_Weapon(_int _index);

private:
	void Key_Input_Tick(_float _fTimeDelta);
	void Silvia_Conversation();
	void Enter_the_Pipe();

private:
	class CTravis_Bike* m_pTravis_Bike = { nullptr };
	_bool				m_bCanRide = { false };

public:
	_bool Can_Ride() const {
		return m_bCanRide;
	}

	CTravis_Bike* Get_Travis_Bike() const {
		return m_pTravis_Bike;
	}

	_bool Is_Riding() const {
		return m_iState == (_uint)WORLDTRAVIS_STATES::STATE_RIDE_START || m_iState == (_uint)WORLDTRAVIS_STATES::STATE_RIDING;
	}

	void GetOff_Bike(_uint iNaviIdx);
	void GetOff_Bike_End();


public:
	static CTravis_World* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg)	override;
	virtual void Free() override;

};

END