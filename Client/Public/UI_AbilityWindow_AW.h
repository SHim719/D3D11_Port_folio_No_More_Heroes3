#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_AbilityWindow_AW final : public CUI
{
private:
	CUI_AbilityWindow_AW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_AbilityWindow_AW(const CUI_AbilityWindow_AW& rhs);
	virtual ~CUI_AbilityWindow_AW() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

	HRESULT OnEnter_Layer(void* pArg)	override;
	void OnExit_Layer()					override;

	HRESULT Init_Icons();
	HRESULT Init_Titles();
	HRESULT Init_Infos();

private:
	class CUI_AbilitySelect_AW*		m_pSelectUI = { nullptr };
	class CUI_Ability_Section*		m_pAbilitySections[3] = {};

	_float4		m_vAbilityPoses[3] = {};
	_int		m_iSelectIdx = { 0 };

	_bool		m_bAppearing = { false };
	_bool		m_bRollingSound = { false };

	_float		m_fTimeAcc = { 0.f };

private:
	void Move_AbilityIdx();
	void Select_AbilityIdx();

private:
	static vector<CTexture*>	s_AbilityIcons;
	static vector<wstring>		s_AbilityTitles;
	static vector<wstring>		s_AbilityInfos;

public:
	static vector<CTexture*>& Get_AbilityIcons() {
		return s_AbilityIcons; }

	static vector<wstring>& Get_AbilityTitles() {
		return s_AbilityTitles; }

	static vector<wstring>& Get_AbilityInfos() {
		return s_AbilityInfos; }


private:
	CVIBuffer*	m_pVIBuffer = { nullptr };


private:
	HRESULT Ready_Component();

public:
	static CUI_AbilityWindow_AW* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END