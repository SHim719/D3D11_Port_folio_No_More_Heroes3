#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_UI_Rect_Instance final : public CVIBuffer_Instance
{
/*
* vColorAlpha 0: color OFF / 1: color ON
* vColorOff : INSUI_COLOR
*/
public:
	enum INSUI_COLOR { INSUI_BASE, INSUI_READY_BRIGHT, INSUI_BLACK, INSUI_READY_DARK, INSUI_END };
	enum INSUI_MOVETYPE { INSUI_MOVE_TOP, INSUI_MOVE_RIGHT, INSUI_MOVE_LEFT, INSUI_MOVE_BOTTOM, INSUI_MOVE_ALL };
	enum INSUI_MOVE { INSUI_MOVE_TARGET, INSUI_MOVE_WAIT, INSUI_MOVE_INITPOS, INSUI_MOVE_END };
	enum UIALIGN { UI_LT, UI_RT, UI_ALIGN_END };

public:
	typedef struct tagUIInstaceDesc
	{
		_int		iRow{};
		_int		iCol{};						/* iNumInstance = iCol * iRow */
		_float		fMargin{};
		_float		fScale{};
		_float		fSpeed{};
		_float4		vColor{};
		_uint		iColorOff{};				/* CUI_InstanceBase::INSUI_COLOR 와 매핑 */
		_uint*		vColorAlpha{ nullptr };		/* 0 : OFF,  1: ON */
		UIALIGN		eAlign = { UI_LT };
	}UIINSTANCE_DESC;

private:
	CVIBuffer_UI_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_UI_Rect_Instance(const CVIBuffer_UI_Rect_Instance& rhs);
	virtual ~CVIBuffer_UI_Rect_Instance() = default;

public:
	HRESULT Initialize(void* pArg)	override;
	HRESULT Initialize_Prototype(const UIINSTANCE_DESC* pInstanceDesc);
	HRESULT Bind_Buffers()			override;

public:
	virtual void Update_Down(_float fTimeDelta);
	void Beat(_float fTimeDelta, _float fDuringTime);
	void DirMove(_float fTimeDelta, _float fDuringTime, INSUI_MOVETYPE eBeatType);
	void Wave(_float fTimeDelta);
	void SpreadIn(_float fTimeDelta, _float fDuringTime, _float fSize, _float fSpeed, _uint* eParam);
	void SpreadOut(_float fTimeDelta, _float fSize, _float fSpeed, _uint* eParam);

	void MoveCircle(_float fTimeDelta);
	void MoveIn(_float fTimeDelta, _uint* eParam);
	void MoveOut(_float fTimeDelta);

	void ColorDecrease(_float fTimeDelta, _uint iMaxData, _uint iCurData, _uint* iColorStatus);
	void ColorIncrease(_float fTimeDelta, _uint iMaxData, _uint iCurData, _uint* iColorStatus);
	void ColorRowSkillState(_float fTimeDelta, _uint iMaxData, _uint iCurData, _uint iColorStatus);

	void ResetData();

private:
	VTXUIPOSTEX*			m_pInstanceVertices = { nullptr };
	_float4*				m_pInstanceVertexPos = { nullptr };
	_int					m_iNumRow = {};
	_int					m_iNumCol = {};
	_int					m_iIdx = {};
	_float					m_fSpeed = {};
	_float					m_fScale = { 0 };
	
	/* Move Control */
	INSUI_MOVE				m_eMoveStatus = { INSUI_MOVE_END };
	_bool					m_bMoved = { false };
	_float					m_fTimeDelay = { 0.f };
	_float					m_fTimeDelayColor = { 0.f };
	_int					m_iAmountDec = { 0 };	// 증가, 감소한 양
	//_uint					m_iAmountInc = { 0 };	// 증가, 감소한 양
	_int					m_iAmountTot = { 0 };	// 감소한 전체 양
	_int					m_iBlackArea = { 0 };	// 
	_int					m_iAlphaArea = { 0 };	// 
	_int					m_iAlphaAreaTotCnt = { 0 };	// 
	_int					m_iLastChgIdx = { 0 };

	_bool*					m_bSign = { nullptr };
	_int*					m_iSign = { nullptr };

	_bool					m_bIn = false;
	_bool					m_bWave = false;
	_bool					m_bOut = false;

	/* Color Control */
	//_float4					m_vColor = {0.f, 0.f, 0.f, 1.f};

public:
	static CVIBuffer_UI_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const UIINSTANCE_DESC* pInstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END