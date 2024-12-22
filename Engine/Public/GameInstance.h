#pragma once

#include "Manager_Headers.h"


BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, OUT ID3D11Device** ppDevice, OUT ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT	Draw();
	void Clear(_uint iLevelIndex);
	void Clear_Level(_uint iLevelIndex);
	void Clear_Renderer();

#pragma region GRAPHIC_DEVICE
public:
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

	ID3D11ShaderResourceView* Get_BackBufferSRV() const;
	
#pragma endregion

#pragma region LEVEL_MANAGER
public: /* For.Level_Manager */
	void Change_Level(class CLevel* pNewLevel);
	_uint Get_CurrentLevelID();
	void Set_CurrentLevelID(_uint iLevelID);
#pragma endregion

#pragma region TIMER_MANAGER
public:
	_float	Get_TimeDelta(const wstring& strTimerTag);
	HRESULT Add_Timer(const wstring& strTimerTag);
	void Compute_TimeDelta(const wstring& strTimerTag);

	// Slow Or Fast Time
	void Set_TimeScale(_float fTimeScale); 
	_float Get_TimeScale() { return m_fTimeScale; }

	void Set_TimeScaleWithRealTime(_float fTimeScale, _float fRealTime);
	void Update_TimeScaleTimer(_float fTimeDelta);
private:
	_float		m_fTimeScale = 1.f;
	_float		m_fTimerForTimeScale = { 0.f };
#pragma endregion

#pragma region OBJECT_MANAGER
public:
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);

	class CGameObject* Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);

	class CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);

	class CComponent* Find_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex = 0);

	class CGameObject* Find_GameObject(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex = 0);

	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);

	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);

	void Clear_Layer(_uint iLevelIndex, const wstring& strLayerTag);

	// Insert GameObject To Layer
	void Insert_GameObject(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pObj);

	// BH - Get ObjectsList for Auto-Targeting or etc
	list<class CGameObject*>& Get_GameObjects(_uint iLevelIndex, const wstring& strLayerTag);
	
	_uint Get_LayerNum(_uint iLevelIndex);

	wstring Get_LayerName(_uint iLevelIndex, _uint iLayerIndex);

#pragma endregion

#pragma region COMPONENT_MANAGER
public:
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

	class CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);
#pragma endregion


#pragma region RENDERER
public:
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Add_RenderComponent(CComponent* pRenderComponent);
	void Add_ShadowObject(CGameObject* pGameObject);
	void Add_UsingShader(class CShader* pShader);
	void Update_ShadowViewProj(_fvector vLightDir, _fvector vShadowCamPos);

	HRESULT Render_PickingColor();
	HRESULT Render_TextToTarget(const wstring& wstrFontTag, const wstring& wstrText, const _float2& vPosition, _fvector vColor, CCustomFont::Align eAlign);
	HRESULT Copy_RenderTarget(const wstring& wstrTargetTag);

	_float Get_CascadeShadowMapScale() const {
		return m_pRenderer->Get_CascadeShadowMapScale();
	}

	void Set_ShadowBias(_float fBias) const {
		m_pRenderer->Set_ShadowBias(fBias);
	}

	void Active_RadialBlur(const RADIALBLUR_DESCS& Descs);
	void Inactive_RadialBlur(_float fLerpTime);
	void Update_BlurCenterWorld(_vector vBlurCenterWorld);
	_bool Is_Active_RadialBlur() const {
		return m_pRenderer->Is_Active_RadialBlur();
	}

	void Active_HitGlitch(_float fDuration) {
		m_pRenderer->Active_HitGlitch(fDuration);
	}

	void Set_Active_FinalSlash_RedTone(_bool bActive) {
		m_pRenderer->Set_Active_FinalSlash_RedTone(bActive);
	}

	void Set_Active_Transform_Travis(_bool bActive) {
		m_pRenderer->Set_Active_Transform_Travis(bActive);
	}
	_bool Is_Active_Transform_Travis() const {
		return m_pRenderer->Is_Active_Transform_Travis();
	}

	void Set_Active_Color_Inversion(_bool bActive) {
		m_pRenderer->Set_Active_Color_Inversion(bActive);
	}

	void Set_Active_MotionBlur(_bool bActive, _float fStrength = 0.f) {
		m_pRenderer->Set_Active_MotionBlur(bActive, fStrength);
	}

	void Set_PBR(_bool bPBR) {
		m_pRenderer->Set_PBR(bPBR);
	}

	void Set_AmbientStrength(_float fStrength) {
		m_pRenderer->Set_AmbientStrength(fStrength);
	}

	void Set_Active_IBL(_bool bActive, const IBL_TEXTURES* pIBL_Textures) {
		m_pRenderer->Set_Active_IBL(bActive, pIBL_Textures);
	}

	void Set_Active_Fog(_bool bActive, const FOG_DESCS* pFogDescs = nullptr) {
		m_pRenderer->Set_Active_Fog(bActive, pFogDescs);
	}

	void Set_Active_Shadow(_bool bActive) {
		m_pRenderer->Set_Active_Shadow(bActive);
	}

	void Set_Active_DOF(_bool bActive) {
		m_pRenderer->Set_Active_DOF(bActive);
	}

	void Set_Active_SSAO(_bool bActive) {
		m_pRenderer->Set_Active_SSAO(bActive);
	}

	void Set_Active_SSR(_bool bActive) {
		m_pRenderer->Set_Active_SSR(bActive);
	}

	void Set_Active_Sky(_bool bActive) {
		m_pRenderer->Set_Active_Sky(bActive);
	}
	
	void Set_SkyDescs(const SKY_DESCS& SkyDescs) {
		m_pRenderer->Set_SkyDescs(SkyDescs);
	}

	void Set_SkyColorStrength(_float fStrength) {
		m_pRenderer->Set_SkyColorStrength(fStrength);
	}

	void Set_Active_Cascade(_bool bActive) {
		m_pRenderer->Set_Active_Cascade(bActive);
	}

	void Set_Render_SonicJuice(_bool bRender) {
		m_pRenderer->Set_Render_SonicJuice(bRender);
	}

	void Set_SSAOPower(_float fPower) {
		m_pRenderer->Set_SSAOPower(fPower);
	}

	void Set_DOF_DistMin(_float fDist) {
		m_pRenderer->Set_DOF_DistMin(fDist);
	}

	void Set_DOF_DistMax(_float fDist) {
		m_pRenderer->Set_DOF_DistMax(fDist);
	}

	void Set_BloomStrength(_float fStrength) {
		m_pRenderer->Set_BloomStrength(fStrength);
	}

	void Set_BloomThreshold(_float fThreshold) {
		m_pRenderer->Set_BloomThreshold(fThreshold);
	}

#pragma endregion

#pragma region KEY_MANAGER
	bool GetKeyDown(eKeyCode _keyCode) { return	 m_pKey_Manager->GetKeyDown(_keyCode); }
	bool GetKeyUp(eKeyCode _keyCode)	{ return m_pKey_Manager->GetKeyUp(_keyCode); }
	bool GetKey(eKeyCode _keyCode)	{ return m_pKey_Manager->GetKey(_keyCode); }
	bool GetKeyNone(eKeyCode _keyCode) { return m_pKey_Manager->GetKeyNone(_keyCode); }
#pragma endregion


#pragma region COLLISION_MANAGER
	void Add_ColliderToLayer(const string& strLayer, CCollider* pCollider);
	// BH - Add Ray Checking
	void Execute_RayCollision(CGameObject* _pSrcObject, const string& _strRayTag, _fvector _vRayOrigin, _fvector _vRayDir, const string& _strDstLayer);
#pragma endregion

#pragma region SOUND_MANAGER
	HRESULT Create_Sound(const string& strPath, const string& strSoundTag);
	void	Play(const string& strSoundTag, _bool bLoop, _float fVolume = 1.f);
	void	Play_RandomSound(const string& strSoundTag, _int iMin, _int iMax, _bool bLoop, _float fVolume = 1.f);
	void	Stop(const string& strSoundTag);
	void	SetVolume(const string& strSoundTag, const _float& fVolume);
	void	SetPosition(const string& strSoundTag, _float fPosition);
	_bool	Is_Playing(const string& strSoundTag);
	void	Set_Sound_FadeOut(const string& strSoundTag, _float fTime);
	class CSound* Get_Sound(const string& strSoundTag);
#pragma endregion

#pragma region FRUSTUM
	_bool In_WorldFrustum(_fvector vWorldPos, _float fRadius);
#pragma endregion 

#pragma region PIPELINE
	void Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);
	void Set_CascadeViewProj(_uint iIndex, _fmatrix CascadeViewProj);

	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_matrix Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4_Inverse_TP(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4x4 Get_TransformFloat4x4_Inverse(CPipeLine::TRANSFORMSTATE eTransformState) const;
	_float4 Get_CamPosition() const;
	_float4x4* Get_CascadeViewProjs() const;

#pragma endregion

#pragma region EVENT_MANAGER
	void Add_Event(function<void()> pFunc);
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale = 1.f, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRotation = 0.f, CCustomFont::Align eAlign = CCustomFont::FontAlignLeft);

#pragma endregion

#pragma region TARGET_MANAGER
public:
	HRESULT Add_RenderTarget(const wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixel, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag);
	HRESULT Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDSV);
	HRESULT Begin_MRT_NoClear(const wstring& strMRTTag);
	HRESULT End_MRT();
	HRESULT Bind_RT_SRV(const wstring& strRenderTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_RT_Resource(const wstring& strRenderTargetTag, ID3D11Texture2D* pResource);
	HRESULT Clear_Target(const wstring& strTargetTag);

	HRESULT Begin_ShadowDepth(ID3D11DepthStencilView* pShadowDSV);
#ifdef _DEBUG
	HRESULT Ready_RTDebug(const wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RTDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Render_SingleRTDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

#pragma region LIGHT_MANAGER
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Add_Light(class CLight* pLight);
	HRESULT Render_Lights(class CShader* pShader, CVIBuffer* pVIBuffer, _bool bPBR);

	void Set_DirectionalLight(LIGHT_DESC& LightDesc);
	void Set_Active_DirectionalLight(_bool bActive);

	_vector Get_LightDir() const;
#pragma endregion

#pragma region CAMERA
	class CCamera* m_pMain_Camera = { nullptr };

	void Change_MainCamera(class CCamera* pCamera);
	void Update_ViewProj();
	void Update_Cascade();

	class CCamera* Get_MainCamera() {
		return m_pMain_Camera;
	}
#pragma endregion

#pragma region TRANSFORM
	// 계산용 Transform
	CTransform* m_pCalculateTransform = { nullptr };

	CTransform* Get_CalculateTF() {
		return m_pCalculateTransform;
	}
#pragma endregion 

#pragma region PICKING
	_float4 Get_PickedPos();
	_float4 Get_RGBValue();
#pragma endregion

private:
	CGraphic_Device*		m_pGraphic_Device = { nullptr };
	CLevel_Manager*			m_pLevel_Manager = { nullptr };
	CTimer_Manager*			m_pTimer_Manager = { nullptr };
	CObject_Manager*		m_pObject_Manager = { nullptr };
	CComponent_Manager*		m_pComponent_Manager = { nullptr };
	CKey_Manager*			m_pKey_Manager = { nullptr };
	CCollision_Manager*		m_pCollision_Manager = { nullptr };
	CSound_Manager*			m_pSound_Manager = { nullptr };
	CFrustum*				m_pFrustum = { nullptr };
	CRenderer*				m_pRenderer = { nullptr };
	CPipeLine*				m_pPipeLine = { nullptr };
	CEvent_Manager*			m_pEvent_Manager = { nullptr };
	CFont_Manager*			m_pFont_Manager = { nullptr };
	CTarget_Manager*		m_pTarget_Manager = { nullptr };
	CLight_Manager*			m_pLight_Manager = { nullptr };
	CPixelPicking*			m_pPixelPicking = { nullptr };
	
public:
	static void Release_Engine();
	virtual void Free() override;
};

END