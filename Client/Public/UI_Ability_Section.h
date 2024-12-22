#pragma once

#include "UI.h"

BEGIN(Client)

class CUI_Ability_Section final : public CUI
{
private:
	CUI_Ability_Section(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Ability_Section(const CUI_Ability_Section& rhs);
	virtual ~CUI_Ability_Section() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	HRESULT Render()					override;

	void Start_Rolling();
	void End_Rolling();

	void End_Menu();

	size_t Get_AbilityIdx() {
		return m_iNowIdx;
	}
private:
	class CUI_Ability_Icon* m_pIcon = { nullptr };

	wstring					m_wstrAbilityTitle = { L"" };
	wstring					m_wstrAbilityInfo = { L"" };

	size_t					m_iNowIdx = { 0 };
	_float					m_fTitleChangeAcc = { 0.f };

	_uint					m_iSeqIdx = { 0 };
	_bool					m_bRolling = { false };
private:
	static _float4			s_IconPoses[3];
	static _float2			s_TitlePoses[3];
	static _float2			s_AbilityInfoPoses[3];

public:
	static CUI_Ability_Section* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr)	override;
	void Free()									override;
};

END


