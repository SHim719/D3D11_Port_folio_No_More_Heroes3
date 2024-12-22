#pragma once


#include "Tool_Defines.h"
#include "GameObject.h"

BEGIN(Tool)
class CToolAnimObj final : public CGameObject
{
public:
	enum PrototypeCharacter {
		TRAVIS,
		TRAVIS_SPACE,
		TRAVIS_WORLD,
		TRAVIS_ARMOR,
		MBONE,
		MRBLACKHOLE_ARML,
		MRBLACKHOLE_ARMR,
		MRBLACKHOLE,
		MRBLACKHOLE_SPACE,
		TREATMENT,
		FULLFACE,
		TRIPLEPUT,
		SONICJUICE,
		TRAVIS_BIKE,
		DAMON,
		LEOPARDON,
		DESTROYMANTF,
		JEANE,

		CHARACTER_END
	};
private:
	CToolAnimObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolAnimObj(const CToolAnimObj& rhs);
	virtual ~CToolAnimObj() = default;

public:
	HRESULT Initialize_Prototype()		override;
	HRESULT Initialize(void* pArg)		override;
	void Tick(_float fTimeDelta)		override;
	void LateTick(_float fTimeDelta)	override;
	HRESULT Render()					override;

private:
	CShader*	m_pShader = nullptr;
	CModel*		m_pModel = nullptr; 

	PrototypeCharacter m_eCharacter_Type = { CHARACTER_END };

private:
	HRESULT Ready_Travis();
	HRESULT Ready_Travis_Space();
	HRESULT Ready_Travis_World();
	HRESULT Ready_Travis_Armor();
	HRESULT Ready_Mbone();
	HRESULT Ready_MBone_Weapon();
	HRESULT Ready_MrBlackhole();
	HRESULT Ready_MrBlackhole_ArmL();
	HRESULT Ready_MrBlackhole_ArmR();
	HRESULT Ready_MrBlackhole_Space();
	HRESULT Ready_Treatment();
	HRESULT Ready_Fullface();
	HRESULT Ready_Tripleput();
	HRESULT Ready_SonicJuice();
	HRESULT Ready_TravisBike();
	HRESULT Ready_Damon();
	HRESULT Ready_Leopardon();
	HRESULT Ready_DestroymanTF();
	HRESULT Ready_Jeane();

public:
	CModel* Get_Model() const { 
		return m_pModel; }
	PrototypeCharacter Get_Type() const {
		return m_eCharacter_Type;
	}

public:
	static CToolAnimObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg)	override;
	void Free() override;
};

END

