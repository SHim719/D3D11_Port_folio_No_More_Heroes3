#pragma once

#include "Base.h"
#include "CustomFont.h"

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CBase
{
public:
	enum RENDERGROUP {
		RENDER_PRIORITY, RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_BLEND,
		 RENDER_EFFECT_NONBLEND, RENDER_EFFECT_BLEND, RENDER_DISTORTION, RENDER_PREV_DISTORTION, RENDER_STENCIL, RENDER_COLORPICKING,
		RENDER_CASCADE, RENDER_SHADOWDEPTH, RENDER_SKYBOX, RENDER_STATUS, RENDER_GRIDDEPTH, RENDER_COPYFINAL, RENDER_MOZAIC, RENDER_AFTER_POSTPROCESS,
		RENDER_UI, RENDER_UI_FONT, RENDER_END
	};
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	class CGameInstance* m_pGameInstance = { nullptr };

	class CVIBuffer_Rect* m_pVIBuffer_Rect = { nullptr };
	class CVIBuffer_Screen* m_pVIBuffer = { nullptr };

	class CShader* m_pDeferredShader = { nullptr };
	class CShader* m_pBloomShader = { nullptr };
	class CShader* m_pPostProcessShader = { nullptr };
	class CShader* m_pSkyShader = { nullptr };

private:
	_float4x4				m_WorldMatrix = {};

public:
	HRESULT Initialize();
	HRESULT Add_RenderObject(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	void    Add_ShadowObject(CGameObject* pGameObject);
	HRESULT Add_RenderComponent(class CComponent* pRenderComponent);
	void	Add_UsingShader(class CShader* pShader);
	void	Tick(_float fTimeDelta);
	HRESULT Draw();
	void	Clear();

	HRESULT Render_PickingColor();
	HRESULT Render_TextToTarget(const wstring& wstrFontTag, const wstring& wstrText, const _float2& vPosition, _fvector vColor, CCustomFont::Align eAlign);

	void Bind_CascadeViewProj();
	void Update_ShadowViewProj(_fvector vLightDir, _fvector vShadowCamPos);
	void Set_ShadowBias(_float fBias);

	HRESULT Copy_RenderTarget(const wstring& wstrTargetTag);

private:
	void Draw_Objects(_uint iGroup);
	void Bind_ViewProj();

	void Clear_RenderGroup(RENDERGROUP eGroup);

	HRESULT Bind_ShaderResources();
	
	HRESULT Render_Cascade();
	HRESULT Render_ShadowDepth();

	HRESULT Render_NonBlend();
	HRESULT Render_NonLight();
	HRESULT Render_Priority();

	HRESULT Render_Stencil();
	HRESULT Render_SSAO();
	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();
	HRESULT Render_SSR();
	HRESULT Render_Sky();

	HRESULT Render_Effect();
	HRESULT Render_Prev_Distortion();
	HRESULT Render_Distortion();

	HRESULT Render_SampleForBloom();
	HRESULT Render_BrightPass();
	HRESULT Render_DownSample();
	HRESULT Render_UpSample();

	HRESULT Render_PostProcess();
	HRESULT PostProcess_Bloom();
	HRESULT Render_DOF();
	HRESULT Render_RadialBlur();
	HRESULT Render_MotionBlur();
	HRESULT Render_HitGlitch();
	HRESULT Render_FinalSlash();
	HRESULT Render_AllBlack_Without_Travis();
	HRESULT Render_ColorInversion();

	HRESULT Render_Final();
	HRESULT Render_FinalCopy();

	HRESULT Render_Blend();
	HRESULT Render_GridDepth();
	HRESULT Render_UI();
	HRESULT Render_Mozaic();
	HRESULT Render_Components();

	HRESULT Render_Status();
	HRESULT Render_AfterPostProcess();
private:
	HRESULT Ready_DefaultTargets(_uint iWidth, _uint iHeight);
	HRESULT Ready_PickingTargets(_uint iWidth, _uint iHeight);
	HRESULT Ready_LightTargets(_uint iWidth, _uint iHeight);
	HRESULT Ready_DeferredTarget(_uint iWidth, _uint iHeight);
	HRESULT Ready_EffectTargets(_uint iWidth, _uint iHeight);
	HRESULT Ready_BloomTargets(_uint iWidth, _uint iHeight);
	HRESULT Ready_BlurTargets(_uint iWidth, _uint iHeight);

	HRESULT Ready_CascadeShadow(_uint iWidth, _uint iHeight);
	HRESULT Ready_Shadow(_uint iWidth, _uint iHeight);

	HRESULT Ready_StatusTarget();
	HRESULT Ready_TextTarget();

#ifdef _DEBUG
private:
	HRESULT Ready_DEBUG();
	HRESULT Render_DEBUG();
	_bool m_bRenderRTV = { false };
#endif

private:
	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	list<class CComponent*>				m_RenderComponents;
	list<class CShader*>				m_UsingShaders;

	class CVIBuffer* m_pVIBuffer_UI = { nullptr };

	D3D11_VIEWPORT						m_OriginViewPort = {};
	D3D11_VIEWPORT						m_TextViewPort;

// Bloom/////////////////////////////////
private:
	_uint								m_iBloomLevel = { 2 };

public:
	void Set_BloomStrength(_float fStrength);
	void Set_BloomThreshold(_float fThreshold);
/////////////////////////////////////

////////////PBR/////////////////////////////////////
private:
	_bool			m_bPBR = { false };
	_bool			m_bIBL = { false };
	IBL_TEXTURES	m_tIBLTextures = {};

	_bool			m_bSSR = { false };
	_bool			m_bRenderSonicJuice = { false };

	_float			m_fAmbientStrength = { 0.f };
public:
	void Set_PBR(_bool bPBR) {
		m_bPBR = bPBR;
	}
	void Set_Active_IBL(_bool bActive, const IBL_TEXTURES* pIBL_Textures = nullptr);
	void Set_AmbientStrength(_float fStrength);

	void Set_Active_SSR(_bool bActive) {
		m_bSSR = bActive;
	}


	void Set_Render_SonicJuice(_bool b) {
		m_bRenderSonicJuice = b;
	}
	
/////////////////////////////////////////////////////

////////////RADIALBLUR/////////////////////////////////////
private:
	_bool				m_bRadialBlur = { false };
	RADIALBLUR_DESCS	m_tRadialBlurDescs = {};

	_float				m_fRadialBlurLerpTime = { 0.f };
	_float				m_fRadialBlurLerpTimeAcc = { 0.f };
	_float				m_fNowBlurStrength = { 0.f };
	_float3				m_vBlurCenterWorld = {};

private:
	void Update_RadialBlur(_float fTimeDelta);
	_float2 Calc_BlurCenterUV();

public:
	void Active_RadialBlur(const RADIALBLUR_DESCS& Descs);
	void Inactive_RadialBlur(_float fLerpTime);

	void Update_BlurCenterWorld(_vector vBlurCenterWorld) {
		XMStoreFloat3(&m_vBlurCenterWorld, vBlurCenterWorld);
	}

	_bool Is_Active_RadialBlur() const {
		return m_bRadialBlur;
	}
////////////RADIALBLUR///////////////////////////////////////


/////////////MOTIONBLUR/////////////////////////////////////
private:
	_bool			m_bMotionBlur = { false };
	_float4x4		m_PrevViewMatrix = {};

public:
	void Set_Active_MotionBlur(_bool bActive, _float fStrength);
	
/////////////MOTIONBLUR/////////////////////////////////////


/////////////HIT_GLITCH//////////////////////////////////////
private:
	_bool		m_bHitGlitch = { false };
	_float		m_fGlitchDuration = { -1.f };

private:
	void Update_HitGlitch(_float fTimeDelta);

public:
	void Active_HitGlitch(_float fDuration) {
		m_bHitGlitch = true;
		m_fGlitchDuration = fDuration;
	}
/////////////HIT_GLITCH//////////////////////////////////////


////////////PostProcess_RedTone/////////////////////////////////////////////////
private:
	_bool		m_bFinalSlash = { false };
	_bool		m_bTransform_Travis = { false };
	_bool		m_bColorInversion = { false };

public:
	void Set_Active_FinalSlash_RedTone(_bool bActive) {
		m_bFinalSlash = bActive;
	}

	void Set_Active_Transform_Travis(_bool bActive) {
		m_bTransform_Travis = bActive;
	}

	_bool Is_Active_Transform_Travis() const { return m_bTransform_Travis; }

	void Set_Active_Color_Inversion(_bool bActive) {
		m_bColorInversion = bActive;
	}

///////////PostProcess_RedTone/////////////////////////////////////////////////


////////////SSAO/////////////////////////////////////////////////
private:
	HRESULT Init_SSAO(_uint iWidth, _uint iHeight);

	HRESULT Init_SSAONoise();
	HRESULT Init_SSAOKernel(_uint iKernelSize);

	ID3D11Texture2D*			m_pSSAONoiseTex = { nullptr };
	ID3D11ShaderResourceView*	m_pSSAONoiseSRV = { nullptr };

	_bool						m_bSSAO = { false };

public:
	void Set_Active_SSAO(_bool bSSAO);
	void Set_SSAOPower(_float fPower);
////////////SSAO/////////////////////////////////////////////////


////////////Shadow/////////////////////////////////////////////////
private:
	_bool						m_bRenderShadow = { false };
	_bool						m_bRenderCascade = { false };

	ID3D11DepthStencilView*		m_pCascadeDSV = { nullptr };
	ID3D11ShaderResourceView*	m_pCascadeSRV = { nullptr };
	_float						m_fCascadeEnd[3] = { 20.f, 50.f, 100.f };

	ID3D11DepthStencilView*		m_pShadowDSV = { nullptr };
	ID3D11ShaderResourceView*	m_pShadowSRV = { nullptr };

	_uint						m_iShadowMapScale = 6;
	_uint						m_iCascadeMapScale = 2;
	D3D11_VIEWPORT				m_ShadowViewPort;
	D3D11_VIEWPORT				m_CascadeShadowViewPort;
public:
	void Set_Active_Shadow(_bool bActive);
	void Set_Active_Cascade(_bool bActive);

	_float Get_CascadeShadowMapScale() const {
		return 1280.f * (_float)m_iCascadeMapScale;
	}

////////////Shadow/////////////////////////////////////////////////


///////////Fog/////////////////////////////////////////////////////
private:
	_bool		m_bNoiseFog = { false };
	_float		m_fFogTime = { 0.f };

	CTexture*	m_pFogNoiseTexture = { nullptr };
public:
	void Set_Active_Fog(_bool bActive, const FOG_DESCS* pFogDescs = nullptr);
///////////Fog//////////////////////////////////////////////////////

///////////DOF///////////////////////////////////////////////////
private:
	_bool	m_bDOF = { false };

public:
	void Set_Active_DOF(_bool bActive) {
		m_bDOF = bActive;
	}
	void Set_DOF_DistMin(_float fDist);
	void Set_DOF_DistMax(_float fDist);

///////////////////////////////////////////////////////////////


////////////////////SKY/////////////////////////////////
private:
	_bool m_bRenderSky = { false };

	_float m_fSkyAccTime = { 0.f };
public:
	void Set_Active_Sky(_bool bActive) {
		m_bRenderSky = bActive;
		m_fSkyAccTime = 0.f;
	}

	void Set_SkyDescs(const SKY_DESCS& SkyDescs);

	void Set_SkyColorStrength(_float fStrength);
////////////////////SKY//////////////////////////////////


////////////////////STATUS////////////////////////////////////
private:
	ID3D11DepthStencilView*				m_pStatusDSV = { nullptr };
	D3D11_VIEWPORT						m_StatusViewPort;

////////////////////////////////////////


private:
	HRESULT Ready_Buffers();
	HRESULT Ready_Shaders();
	HRESULT Bind_Matrices();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END