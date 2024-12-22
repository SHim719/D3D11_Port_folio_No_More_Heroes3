#include "..\Public\Renderer.h"
#include "GameObject.h"

#include "GameInstance.h"



CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}


HRESULT CRenderer::Initialize()
{
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_OriginViewPort = ViewportDesc;

	if (FAILED(Ready_Buffers()))
		return E_FAIL;

	if (FAILED(Ready_Shaders()))
		return E_FAIL;

	if (FAILED(Ready_DefaultTargets((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Ready_PickingTargets((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Init_SSAO((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Ready_LightTargets((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Ready_DeferredTarget((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Ready_BlurTargets((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Ready_EffectTargets((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;
	
	if (FAILED(Ready_BloomTargets((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height)))
		return E_FAIL;

	if (FAILED(Ready_CascadeShadow((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Width)))
		return E_FAIL;

	if (FAILED(Ready_Shadow((_uint)ViewportDesc.Width, (_uint)ViewportDesc.Width)))
		return E_FAIL;

	if (FAILED(Ready_StatusTarget()))
		return E_FAIL;

	if (FAILED(Ready_TextTarget()))
		return E_FAIL;
	
	if (FAILED(Bind_Matrices()))
		return E_FAIL;

	_float fAmbientStrength = 1.f;

	m_pDeferredShader->Bind_RawValue("g_fAmbientStrength", &fAmbientStrength, sizeof(_float));

	m_pFogNoiseTexture = CTexture::Create(m_pDevice, m_pContext, L"../../Resources/DissolveNoise.dds");//FogNoise.png");
	if (nullptr == m_pFogNoiseTexture)
		return E_FAIL;

	if (FAILED(m_pFogNoiseTexture->Set_SRV(m_pDeferredShader, "g_FogNoiseTexture")))
		return E_FAIL;

	//m_pLensFlareNoise = CTexture::Create(m_pDevice, m_pContext, L"../../Resources/Noise_LensFlare");
	//if (nullptr == m_pLensFlareNoise)
	//	return E_FAIL;
	//
	//if (FAILED(m_pLensFlareNoise->Set_SRV(m_pDeferredShader, "g_LensFlareNoiseTexture")))
	//	return E_FAIL;

#ifdef _DEBUG
	
	if (FAILED(Ready_DEBUG()))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CRenderer::Init_SSAO(_uint iWidth, _uint iHeight)
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAO"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
		return E_FAIL;

	if (FAILED(Init_SSAONoise()))
		return E_FAIL;
	
	if (FAILED(Init_SSAOKernel(16)))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Init_SSAONoise()
{
	std::random_device RD;
	std::mt19937 gen(RD());

	std::uniform_real_distribution<_float> dis(-1.f, 1.f);

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = 8;
	TextureDesc.Height = 8;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	_float3* vInitialData = new _float3[TextureDesc.Width * TextureDesc.Height];
	for (_uint i = 0; i < TextureDesc.Width * TextureDesc.Height; ++i) {
		XMStoreFloat3(&vInitialData[i], XMVectorSet(dis(gen), dis(gen), 0.f, 0.f));
	}

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vInitialData;
	InitData.SysMemPitch = TextureDesc.Width * sizeof(_float3);
	InitData.SysMemSlicePitch = 0; 

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitData, &m_pSSAONoiseTex)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pSSAONoiseTex, nullptr, &m_pSSAONoiseSRV)))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_ShaderResourceView("g_SSAONoiseTexture", m_pSSAONoiseSRV)))
		return E_FAIL;
	
	Safe_Delete_Array(vInitialData);

	return S_OK;
}

HRESULT CRenderer::Init_SSAOKernel(_uint iKernelSize)
{
	std::random_device RD;
	std::mt19937 gen(RD());

	std::uniform_real_distribution<_float> dis1(-1.f, 1.f);
	std::uniform_real_distribution<_float> dis2(0.f, 1.f);

	_float3* SSAOKernel = new _float3[iKernelSize];

	for (_uint i = 0; i < iKernelSize; ++i)
	{
		_vector vSample = XMVector3Normalize(XMVectorSet(dis1(gen), dis1(gen), dis2(gen), 0.f)); //* dis2(gen);

		_float t = (_float)i / (_float)iKernelSize;
		_float t2 = t * t;
		_float fScale = (1.f - t2) * 0.1f + t2 * 1.f;

		XMStoreFloat3(&SSAOKernel[i], vSample * fScale);
	}

	if (FAILED(m_pDeferredShader->Bind_RawValue("g_vSampleKernel", SSAOKernel, sizeof(_float3) * iKernelSize)))
		return E_FAIL;

	Safe_Delete_Array(SSAOKernel);

	return S_OK;
}

void CRenderer::Set_Active_SSAO(_bool bSSAO)
{
	m_bSSAO = bSSAO;

	m_pDeferredShader->Bind_RawValue("g_bSSAO", &bSSAO, sizeof(_bool));
}

void CRenderer::Set_SSAOPower(_float fPower)
{
	m_pDeferredShader->Bind_RawValue("g_fSSAOPower", &fPower, sizeof(_float));
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].emplace_back(pRenderObject);

	Safe_AddRef(pRenderObject);	

	return S_OK;
}

void CRenderer::Add_ShadowObject(CGameObject* pGameObject)
{
	if (false == m_bRenderShadow)
		return;

	if (m_bRenderCascade)
		m_RenderObjects[RENDER_CASCADE].emplace_back(pGameObject);
	else
		m_RenderObjects[RENDER_SHADOWDEPTH].emplace_back(pGameObject);


	Safe_AddRef(pGameObject);

}

HRESULT CRenderer::Add_RenderComponent(CComponent* pRenderComponent)
{
	if (nullptr == pRenderComponent)
		return E_FAIL;

	m_RenderComponents.emplace_back(pRenderComponent);

	Safe_AddRef(pRenderComponent);

	return S_OK;
}

void CRenderer::Add_UsingShader(CShader* pShader)
{
	Safe_AddRef(pShader);
	m_UsingShaders.emplace_back(pShader);
}

void CRenderer::Tick(_float fTimeDelta)
{
	if (m_bRadialBlur)
		Update_RadialBlur(fTimeDelta);

	if (m_bHitGlitch)
		Update_HitGlitch(fTimeDelta);

	if (m_bNoiseFog)
		m_fFogTime += fTimeDelta;

	m_fSkyAccTime += fTimeDelta * 5.f;
}	

HRESULT CRenderer::Draw()
{
	Bind_ViewProj();
	if (m_bRenderShadow && m_bRenderCascade)
	{
		m_pGameInstance->Update_Cascade();
		Bind_CascadeViewProj();
	}
		

	if (m_bRenderShadow)
	{
		if (m_bRenderCascade)
		{
			if (FAILED(Render_Cascade()))
				return E_FAIL;
		}
			
		else
		{
			if (FAILED(Render_ShadowDepth()))
				return E_FAIL;
		}
	}

	if (FAILED(m_pGameInstance->Clear_Target(L"Target_Deferred")))
		return E_FAIL;
	
	Render_Priority();

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_bSSAO)
	{
		if (FAILED(Render_SSAO()))
			return E_FAIL;
	}
	
	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	if (FAILED(Render_Deferred()))
		return E_FAIL;

	if (m_bSSR)
	{
		if (FAILED(Render_SSR()))
			return E_FAIL;
	}

	if (m_bRenderSky)
	{
		if (FAILED(Render_Sky()))
			return E_FAIL;
	}

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (m_bTransform_Travis)
	{
		if (FAILED(Render_AllBlack_Without_Travis()))
			return E_FAIL;
	}

	if (FAILED(Render_Effect()))
		return E_FAIL;

	if (FAILED(Render_Prev_Distortion()))
		return E_FAIL;
	
	if (FAILED(Render_Distortion()))
		return E_FAIL;

	if (FAILED(Render_SampleForBloom()))
		return E_FAIL;

	if (FAILED(Render_Stencil()))
		return E_FAIL;

	if (FAILED(Render_PostProcess()))
		return E_FAIL;

	if (FAILED(Render_GridDepth()))
		return E_FAIL;

	if (FAILED(Render_Status()))
		return E_FAIL;

	if (FAILED(Render_AfterPostProcess()))
		return E_FAIL;


#ifdef _DEBUG
	if (FAILED(Render_Components()))
		return E_FAIL;

	if (m_pGameInstance->GetKeyDown(eKeyCode::F9))
		m_bRenderRTV = !m_bRenderRTV;

	if (m_bRenderRTV)
	{
		if (FAILED(Render_DEBUG()))
			return E_FAIL;
	}

#endif

	if (FAILED(Render_UI()))
		return E_FAIL;


	if (FAILED(Render_FinalCopy()))
		return E_FAIL;

	return S_OK;
}

void CRenderer::Clear()
{
	for (auto& RenderObjects : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}
}

HRESULT CRenderer::Render_PickingColor()
{
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_ColorPicking")))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_COLORPICKING])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_Picking();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_COLORPICKING].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}



void CRenderer::Bind_CascadeViewProj()
{
	for (auto& pShader : m_UsingShaders)
		pShader->Bind_Matrices("g_CascadeViewProj", m_pGameInstance->Get_CascadeViewProjs(), 3);

}

void CRenderer::Update_ShadowViewProj(_fvector vLightDir, _fvector vShadowCamPos)
{
	_matrix ShadowViewProj = XMMatrixLookToLH(vShadowCamPos, vLightDir, XMVectorSet(0.f, 1.f, 0.f, 0.f))
		* XMMatrixPerspectiveFovLH(To_Radian(90.f), 1280.f / 720.f, 0.1f, 3000.f);

	_float4x4 ShadowViewProj4x4;
	XMStoreFloat4x4(&ShadowViewProj4x4, ShadowViewProj);

	for (auto& pShader : m_UsingShaders)
		pShader->Bind_Matrix("g_ShadowViewProj", &ShadowViewProj4x4);
	
	m_pDeferredShader->Bind_Matrix("g_ShadowViewProj", &ShadowViewProj4x4);
}

void CRenderer::Set_ShadowBias(_float fBias)
{
	m_pDeferredShader->Bind_RawValue("g_fShadowBias", &fBias, sizeof(_float));
}


void CRenderer::Draw_Objects(_uint iGroup)
{
	for (auto& pRenderObject : m_RenderObjects[iGroup])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[iGroup].clear();
}

void CRenderer::Bind_ViewProj()
{
	_float4x4 ViewMatrix = m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);

	for (auto& pShader : m_UsingShaders)
	{
		pShader->Bind_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4));
		pShader->Bind_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4));
	}
}

void CRenderer::Clear_RenderGroup(RENDERGROUP eGroup)
{
	for (auto& pRenderObject : m_RenderObjects[eGroup])
	{
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[eGroup].clear();
}

HRESULT CRenderer::Bind_ShaderResources()
{
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Diffuse"), m_pDeferredShader, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Normal"), m_pDeferredShader, "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Depth"), m_pDeferredShader, "g_DepthTexture")))
		return E_FAIL;


	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Copy_RenderTarget(const wstring& wstrTargetTag)
{


	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Copy")))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Bind_RT_SRV(wstrTargetTag, m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Render_Priority()
{
	if (m_RenderObjects[RENDER_PRIORITY].size() <= 0)
		return S_OK;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(TEXT("MRT_Deferred"))))
		return E_FAIL;

	Draw_Objects(RENDER_PRIORITY);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	Draw_Objects(RENDER_NONBLEND);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Render_NonLight()
{
	if (m_RenderObjects[RENDER_NONLIGHT].size() <= 0)
		return S_OK;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(TEXT("MRT_Deferred"))))
		return E_FAIL;

	Draw_Objects(RENDER_NONLIGHT);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (m_RenderObjects[RENDER_BLEND].size() <= 0)
		return S_OK;

	//if (m_RenderObjects[RENDER_BLEND].size() >= 2)
	//{
	//	m_RenderObjects[RENDER_BLEND].sort([&](CGameObject* pSour, CGameObject* pDest)->_bool
	//		{
	//			_vector vCamPosition = XMLoadFloat4(&m_pGameInstance->Get_CamPosition());
	//			_float fSrcDist = 999999.f;
	//			_float fDstDist = 999999.f;
	//
	//			CTransform* pSrcTransform = pSour->Get_Transform();
	//			if (pSrcTransform)
	//			{
	//				fSrcDist = XMVector3Length(vCamPosition - pSrcTransform->Get_Position()).m128_f32[0];
	//			}
	//
	//			CTransform* pDstTransform = pDest->Get_Transform();
	//			if (pDstTransform)
	//			{
	//				fDstDist = XMVector3Length(vCamPosition - pDstTransform->Get_Position()).m128_f32[0];
	//			}
	//
	//			return fSrcDist > fDstDist;
	//		});
	//}
	//
	if (m_bRenderSonicJuice)
		Copy_RenderTarget(L"Target_Deferred");

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_Deferred")))
		return E_FAIL;

	Draw_Objects(RENDER_BLEND);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_GridDepth()
{
	if (m_RenderObjects[RENDER_GRIDDEPTH].size() <= 0)
		return S_OK;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(TEXT("MRT_GridDepth"))))
		return E_FAIL;

	Draw_Objects(RENDER_GRIDDEPTH);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;

}

HRESULT CRenderer::Render_UI()
{
	m_RenderObjects[RENDER_UI].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			_float fSourZ = 99999.f, fDestZ = 99999.f;

			CTransform* pSrcTransform = (pSour)->Get_Transform();
			if (pSrcTransform)
				fSourZ = XMVectorGetZ(pSrcTransform->Get_Position());

			CTransform* pDstTransform = (pDest)->Get_Transform();
			if (pDstTransform)
				fDestZ = XMVectorGetZ(pDstTransform->Get_Position());
	
			return fSourZ > fDestZ;
		});

	Draw_Objects(RENDER_UI);

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	Draw_Objects(RENDER_UI_FONT);

	return S_OK;
}

HRESULT CRenderer::Render_Mozaic()
{
	if (FAILED(Copy_RenderTarget(L"Target_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(7)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_Stencil()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Stencil"))))
		return E_FAIL;

	Draw_Objects(RENDER_STENCIL);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_SSAO()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO"))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Begin(7)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	////////////////////Horizontal Blur ////////////////////////// 
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_H"))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_SSAO"), m_pDeferredShader, "g_PrevBlurXTexture")))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	////////////////////Horizontal Blur ////////////////////////// 


	////////////////////Vertical Blur ////////////////////////// 
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Blur_H"), m_pDeferredShader, "g_PrevBlurYTexture")))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Begin(9)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	//////////////////// Vertical Blur ////////////////////////// 

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	/* Shade */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (m_bPBR)
	{
		if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_ORM"), m_pDeferredShader, "g_ORMTexture")))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Lights(m_pDeferredShader, m_pVIBuffer, m_bPBR)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	_uint iPassIdx = 0;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Shade"), m_pDeferredShader, "g_ShadeTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Emissive"), m_pDeferredShader, "g_EmissiveTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_RimLight"), m_pDeferredShader, "g_RimLightTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Ambient"), m_pDeferredShader, "g_AmbientTexture")))
		return E_FAIL;

	if (true == m_bSSAO)
	{
		if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_SSAO"), m_pDeferredShader, "g_SSAOTexture")))
			return E_FAIL;
	}
		

	if (true == m_bRenderShadow)
	{
		if (true == m_bRenderCascade)
		{
			if (FAILED(m_pDeferredShader->Bind_Matrices("g_CascadeViewProj", m_pGameInstance->Get_CascadeViewProjs(), 3)))
				return E_FAIL;

			if (FAILED(m_pDeferredShader->Bind_ShaderResourceView("g_CascadeShadowMap", m_pCascadeSRV)))
				return E_FAIL;

			if (FAILED(m_pDeferredShader->Bind_RawValue("g_vLightDir", &m_pGameInstance->Get_LightDir(), sizeof(_float4))))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pDeferredShader->Bind_ShaderResourceView("g_ShadowDepthTexture", m_pShadowSRV)))
				return E_FAIL;
		}	
	}


	if (true == m_bNoiseFog)
	{
		if (FAILED(m_pDeferredShader->Bind_RawValue("g_fFogTime", &m_fFogTime, sizeof(_float))))
			return E_FAIL;
	}

	if (false == m_bPBR)
		iPassIdx = 3;
	else
		iPassIdx = 4;
	
	if (FAILED(m_pDeferredShader->Begin(iPassIdx)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_SSR()
{
	if (FAILED(Copy_RenderTarget(L"Target_Deferred")))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_Deferred")))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pDeferredShader, "g_OriginTexture")))
		return E_FAIL;
	
	if (FAILED(m_pDeferredShader->Begin(11)))
		return E_FAIL;
	
	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Sky()
{
	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_Deferred")))
		return E_FAIL;

	if (FAILED(m_pSkyShader->Bind_Matrix("g_CamViewMatrixInv", &m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pSkyShader->Bind_Matrix("g_CamProjMatrixInv", &m_pGameInstance->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pSkyShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pSkyShader->Bind_RawValue("overwritten_time", &m_fSkyAccTime, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pSkyShader->Begin(0)))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Render_Effect()
{
	if (m_RenderObjects[RENDERGROUP::RENDER_EFFECT_BLEND].empty() && 
		m_RenderObjects[RENDERGROUP::RENDER_EFFECT_NONBLEND].empty())
		return S_OK;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_Effect")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Clear_Target(L"Target_Distortion")))
		return E_FAIL;


	Draw_Objects(RENDER_EFFECT_NONBLEND);

	Draw_Objects(RENDER_EFFECT_BLEND);


	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Prev_Distortion()
{
	if (m_RenderObjects[RENDERGROUP::RENDER_PREV_DISTORTION].empty())
		return S_OK;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_Distortion")))
		return E_FAIL;

	Draw_Objects(RENDERGROUP::RENDER_PREV_DISTORTION);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}



HRESULT CRenderer::Render_Distortion()
{
	if (m_RenderObjects[RENDERGROUP::RENDER_DISTORTION].empty())
		return S_OK;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(Copy_RenderTarget(L"Target_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Distortion"), m_pPostProcessShader, "g_DistortionTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_DISTORTION])
		Safe_Release(pRenderObject);

	m_RenderObjects[RENDER_DISTORTION].clear();

	return S_OK;
}


HRESULT CRenderer::Render_PostProcess()
{
	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;


	if (FAILED(PostProcess_Bloom()))
		return E_FAIL;


	if (m_bDOF)
	{
		if (FAILED(Render_DOF()))
			return E_FAIL;
	}

	if (m_bRadialBlur)
	{
		if (FAILED(Render_RadialBlur()))
			return E_FAIL;
	}

	if (!m_RenderObjects[RENDER_MOZAIC].empty())
	{
		if (FAILED(Render_Mozaic()))
			return E_FAIL;

		for (auto& pRenderObject : m_RenderObjects[RENDER_MOZAIC])
			Safe_Release(pRenderObject);

		m_RenderObjects[RENDER_MOZAIC].clear();
	}

	if (m_bMotionBlur)
	{
		if (FAILED(Render_MotionBlur()))
			return E_FAIL;
	}

	if (m_bHitGlitch)
	{
		if (FAILED(Render_HitGlitch()))
			return E_FAIL;
	}

	if (m_bFinalSlash)
	{
		if (FAILED(Render_FinalSlash()))
			return E_FAIL;
	}

	if (m_bColorInversion)
	{
		if (FAILED(Render_ColorInversion()))
			return E_FAIL;
	}
	
	
	if (FAILED(Render_Final()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::PostProcess_Bloom()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_AfterToneMap"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Deferred"), m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_BloomUpSample" + to_wstring(m_iBloomLevel - 1), m_pPostProcessShader, "g_BloomTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_DOF()
{
	if (FAILED(Copy_RenderTarget(L"Target_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Blur_H")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pDeferredShader, "g_PrevBlurXTexture")))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	//////////////////////////////////////////////////
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Blur_V")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Blur_H"), m_pDeferredShader, "g_PrevBlurYTexture")))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Begin(9)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	//////////////////////////////


	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Blur_V"), m_pDeferredShader, "g_BlurredTexture")))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Begin(12)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_RadialBlur()
{
	if (FAILED(Copy_RenderTarget(L"Target_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Bind_RawValue("g_fBlurStrength", &m_fNowBlurStrength, sizeof(_float))))
		return E_FAIL;
	
	if (FAILED(m_pPostProcessShader->Bind_RawValue("g_fBlurRadius", &m_tRadialBlurDescs.fBlurRadius, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Bind_RawValue("g_vBlurCenter", &Calc_BlurCenterUV(), sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_MotionBlur()
{
	if (FAILED(Copy_RenderTarget(L"Target_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pDeferredShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_PrevViewMatrix", &m_PrevViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Begin(10)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	XMStoreFloat4x4(&m_PrevViewMatrix, m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));

	return S_OK;
}

HRESULT CRenderer::Render_HitGlitch()
{
	if (FAILED(Copy_RenderTarget(L"Target_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	_float fTimeDelta = m_pGameInstance->Get_TimeDelta(L"Timer_60");

	if (FAILED(m_pPostProcessShader->Bind_RawValue("g_fTime", &fTimeDelta, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_FinalSlash()
{
	if (FAILED(Copy_RenderTarget(L"Target_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_RimLight"), m_pPostProcessShader, "g_FinalSlashFlagTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(6)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_AllBlack_Without_Travis()
{
	if (FAILED(Copy_RenderTarget(L"Target_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(8)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_ColorInversion()
{
	if (FAILED(Copy_RenderTarget(L"Target_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT_NoClear(L"MRT_AfterToneMap")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_Copy"), m_pPostProcessShader, "g_OriginTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(9)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Final()
{
	if (FAILED(m_pGameInstance->Bind_RT_SRV(TEXT("Target_AfterToneMap"), m_pPostProcessShader, "g_FinalTexture")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_FinalCopy()
{
	if (m_RenderObjects[RENDERGROUP::RENDER_COPYFINAL].empty())
		return S_OK;

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_CopyFinal")))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Bind_ShaderResourceView("g_FinalTexture", m_pGameInstance->Get_BackBufferSRV())))
		return E_FAIL;

	if (FAILED(m_pPostProcessShader->Begin(3)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_COPYFINAL])
		Safe_Release(pRenderObject);
	
	m_RenderObjects[RENDER_COPYFINAL].clear();


	return S_OK;
}


HRESULT CRenderer::Render_Components()
{
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	for (auto& pRenderComponent : m_RenderComponents)
	{
		if (nullptr != pRenderComponent)
			pRenderComponent->Render();

		Safe_Release(pRenderComponent);
	}

	m_RenderComponents.clear();

	return S_OK;
}

HRESULT CRenderer::Render_Status()
{
	if (m_RenderObjects[RENDER_STATUS].empty())
		return S_OK;
	
	m_pContext->RSSetViewports(1, &m_StatusViewPort);

	m_pContext->ClearDepthStencilView(m_pStatusDSV, D3D11_CLEAR_DEPTH, 1.f, 0);

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Status", m_pStatusDSV)))
		return E_FAIL;

	Draw_Objects(RENDER_STATUS);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	m_pContext->RSSetViewports(1, &m_OriginViewPort);

	return S_OK;
}

HRESULT CRenderer::Render_AfterPostProcess()
{
	Draw_Objects(RENDER_AFTER_POSTPROCESS);
	return S_OK;
}

HRESULT CRenderer::Render_TextToTarget(const wstring& wstrFontTag, const wstring& wstrText
	, const _float2& vPosition, _fvector vColor, CCustomFont::Align eAlign)
{
	m_pContext->RSSetViewports(1, &m_TextViewPort);

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Text")))
		return E_FAIL;

	m_pGameInstance->Render_Font(wstrFontTag, wstrText, vPosition, 1.f, vColor, 0.f, eAlign);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	m_pContext->RSSetViewports(1, &m_OriginViewPort);

	return S_OK;
}

HRESULT CRenderer::Render_Cascade()
{
	m_pContext->ClearDepthStencilView(m_pCascadeDSV, D3D11_CLEAR_DEPTH, 1.f, 0);

	m_pContext->RSSetViewports(1, &m_CascadeShadowViewPort);

	m_pGameInstance->Begin_ShadowDepth(m_pCascadeDSV);

	for (auto& pRenderObject : m_RenderObjects[RENDER_CASCADE])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_Cascade();

		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_CASCADE].clear();

	m_pGameInstance->End_MRT();

	m_pContext->RSSetViewports(1, &m_OriginViewPort);

	return S_OK;
}

HRESULT CRenderer::Render_ShadowDepth()
{
	m_pContext->ClearDepthStencilView(m_pShadowDSV, D3D11_CLEAR_DEPTH, 1.f, 0);

	m_pContext->RSSetViewports(1, &m_ShadowViewPort);

	m_pGameInstance->Begin_ShadowDepth(m_pShadowDSV);

	for (auto& pRenderObject : m_RenderObjects[RENDER_SHADOWDEPTH])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_ShadowDepth();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_SHADOWDEPTH].clear();

	m_pGameInstance->End_MRT();

	m_pContext->RSSetViewports(1, &m_OriginViewPort);

	return S_OK;
}

HRESULT CRenderer::Render_SampleForBloom()
{
	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(Render_BrightPass()))
		return E_FAIL;

	if (FAILED(Render_DownSample()))
		return E_FAIL;

	if (FAILED(Render_UpSample()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BrightPass()
{
	if (FAILED(Copy_RenderTarget(L"Target_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_BrightPass")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RT_SRV(L"Target_Copy", m_pBloomShader, "g_PrevBrightPassTexture")))
		return E_FAIL;

	if (FAILED(m_pBloomShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CRenderer::Render_DownSample()
{
	D3D11_VIEWPORT ViewPort = m_OriginViewPort;

	_int iDiv = 2;

	for (_uint i = 0; i < m_iBloomLevel; ++i)
	{
		iDiv = (_int)pow(2, i + 1);

		ViewPort.Width = 1280.f / iDiv;
		ViewPort.Height = 720.f / iDiv;

		m_pContext->RSSetViewports(1, &ViewPort);

		wstring MRTTag = L"MRT_BloomDownSample" + to_wstring(i);
		wstring BindRTTag = L"";
		if (0 == i)
			BindRTTag = L"Target_BrightPassed";
		else
			BindRTTag = L"Target_BloomDownSample" + to_wstring(i - 1);

		if (FAILED(m_pGameInstance->Begin_MRT(MRTTag)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Bind_RT_SRV(BindRTTag, m_pBloomShader, "g_ToSampleDownTexture")))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Bind_RawValue("g_iDiv", &iDiv, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_UpSample()
{
	D3D11_VIEWPORT ViewPort = m_OriginViewPort;

	_int iDiv = 4;

	for (_uint i = 0; i < m_iBloomLevel; ++i)
	{
		iDiv = (_int)pow(2, m_iBloomLevel - 1 - i);

		ViewPort.Width = 1280.f / iDiv;
		ViewPort.Height = 720.f / iDiv;
		m_pContext->RSSetViewports(1, &ViewPort);

		wstring MRTTag = L"MRT_BloomUpSample" + to_wstring(i);
		wstring BindRTTag = L"";

		if (0 == i)
			BindRTTag = L"Target_BloomDownSample" + to_wstring(m_iBloomLevel - 1);
		else
			BindRTTag = L"Target_BloomUpSample" + to_wstring(i - 1);

		if (FAILED(m_pGameInstance->Begin_MRT(MRTTag)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Bind_RT_SRV(BindRTTag, m_pBloomShader, "g_ToSampleUpTexture")))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Bind_RawValue("g_iDiv", &iDiv, sizeof(_int))))
			return E_FAIL;

		if (FAILED(m_pBloomShader->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;
	}

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::Ready_DEBUG()
{
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Diffuse"), 50.f, 50.0f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Normal"), 50.f, 150.0f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_ORM"), 50.f, 250.0f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Depth"), 50.f, 350.0f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Shade"), 50.f, 450.0f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Deferred"), 50.f, 550.f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Emissive"), 150.f, 50.f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_RimLight"), 150.f, 150.f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Distortion"), 150.f, 250.f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_SSAO"), 150.f, 350.f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BrightPassed"), 350.f, 50.f, 100.f, 100.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BloomDownSample0"), 450.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BloomDownSample1"), 550.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BloomUpSample0"), 650.f, 50.f, 100.f, 100.f)))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_BloomUpSample1"), 750.f, 50.f, 100.f, 100.f)))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_DEBUG()
{
	if (FAILED(Bind_Matrices()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_GameObjects"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_LightAcc"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_Deferred"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_SSAO"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_RimLight"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_Emissive"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_Distortion"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BrightPassed"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;
	

	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BloomDownSample0"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BloomDownSample1"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BloomUpSample0"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Render_SingleRTDebug(TEXT("Target_BloomUpSample1"), m_pDeferredShader, m_pVIBuffer_Rect)))
		return E_FAIL;

	return S_OK;
}


#endif

void CRenderer::Set_BloomStrength(_float fStrength)
{
	m_pPostProcessShader->Bind_RawValue("g_fBloomStrength", &fStrength, sizeof(_float));
}

void CRenderer::Set_BloomThreshold(_float fThreshold)
{
	m_pBloomShader->Bind_RawValue("g_fBloomThreshold", &fThreshold, sizeof(_float));
}


HRESULT CRenderer::Ready_DefaultTargets(_uint iWidth, _uint iHeight)
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ORM"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_RimLight"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Emissive"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_ORM"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_RimLight"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))))
		return E_FAIL;
	


	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Copy"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Copy"), TEXT("Target_Copy"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GridDepth"), TEXT("Target_Depth"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_CopyFinal"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_CopyFinal"), TEXT("Target_CopyFinal"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Stencil"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Stencil"), TEXT("Target_Stencil"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Ready_PickingTargets(_uint iWidth, _uint iHeight)
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_ColorValue", iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ColorPicking"), TEXT("Target_ColorValue"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_LightTargets(_uint iWidth, _uint iHeight)
{
	/* For.Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Ambient"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Ambient"))))
		return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Ready_DeferredTarget(_uint iWidth, _uint iHeight)
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Deferred"), iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Deferred"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_AfterToneMap"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_AfterToneMap"), TEXT("Target_AfterToneMap"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Water"), TEXT("Target_Deferred"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Water"), TEXT("Target_Normal"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Water"), TEXT("Target_Depth"))))
		return E_FAIL; 

	return S_OK;
}


HRESULT CRenderer::Ready_EffectTargets(_uint iWidth, _uint iHeight)
{

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Deferred"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Distortion"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_RimLight"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;
	

	return S_OK;
}


HRESULT CRenderer::Ready_BloomTargets(_uint iWidth, _uint iHeight)
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_BrightPassed", iWidth, iHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_BrightPass", L"Target_BrightPassed")))
		return E_FAIL;


	for (_uint i = 0; i < m_iBloomLevel; ++i)
	{
		_uint iDiv = (_uint)pow(2, i + 1);

		wstring wstrTargetName = L"Target_BloomDownSample" + to_wstring(i);

		if (FAILED(m_pGameInstance->Add_RenderTarget(wstrTargetName, iWidth / iDiv, iHeight / iDiv, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
			return E_FAIL;
	}

	for (_uint i = 0; i < m_iBloomLevel; ++i)
	{
		_uint iDiv = (_uint)pow(2, m_iBloomLevel - 1 - i);

		wstring wstrTargetName = L"Target_BloomUpSample" + to_wstring(i);

		if (FAILED(m_pGameInstance->Add_RenderTarget(wstrTargetName, iWidth / iDiv, iHeight / iDiv, DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
			return E_FAIL;
	}

	for (_uint i = 0; i < m_iBloomLevel; ++i)
	{
		wstring wstrDownMRTTag = L"MRT_BloomDownSample" + to_wstring(i);
		wstring wstrUpMRTTag = L"MRT_BloomUpSample" + to_wstring(i);

		wstring wstrDownRTTag = L"Target_BloomDownSample" + to_wstring(i);
		wstring wstrUpRTTag = L"Target_BloomUpSample" + to_wstring(i);

		if (FAILED(m_pGameInstance->Add_MRT(wstrDownMRTTag, wstrDownRTTag)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_MRT(wstrUpMRTTag, wstrUpRTTag)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CRenderer::Ready_BlurTargets(_uint iWidth, _uint iHeight)
{

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_H"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_V"), iWidth, iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_H"), TEXT("Target_Blur_H"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_V"), TEXT("Target_Blur_V"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_CascadeShadow(_uint iWidth, _uint iHeight)
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWidth * m_iCascadeMapScale;
	TextureDesc.Height = iHeight * m_iCascadeMapScale;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 3;
	TextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;
	
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc = { DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2DARRAY, 0 };
	DSVDesc.Texture2DArray.FirstArraySlice = 0;
	DSVDesc.Texture2DArray.ArraySize = 3;
	DSVDesc.Texture2DArray.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = { DXGI_FORMAT_R32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE2DARRAY, 0, 0 };
	SRVDesc.Texture2DArray.FirstArraySlice = 0;
	SRVDesc.Texture2DArray.ArraySize = 3;
	SRVDesc.Texture2DArray.MipLevels = 1;
	SRVDesc.Texture2DArray.MostDetailedMip = 0;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, &DSVDesc, &m_pCascadeDSV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(pDepthStencilTexture, &SRVDesc, &m_pCascadeSRV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	m_CascadeShadowViewPort = m_OriginViewPort;
	m_CascadeShadowViewPort.Width = (_float)TextureDesc.Width;
	m_CascadeShadowViewPort.Height = (_float)TextureDesc.Height;

	if (FAILED(m_pDeferredShader->Bind_FloatArray("g_CascadeEnd", m_fCascadeEnd, 3)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_Shadow(_uint iWidth, _uint iHeight)
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	
	TextureDesc.Width = iWidth * m_iShadowMapScale;
	TextureDesc.Height = iHeight * m_iShadowMapScale;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc = { DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2D, 0 };
	DSVDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = { DXGI_FORMAT_R32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE2D, 0, 0 };
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, &DSVDesc, &m_pShadowDSV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(pDepthStencilTexture, &SRVDesc, &m_pShadowSRV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	m_ShadowViewPort = m_OriginViewPort;
	m_ShadowViewPort.Width = (_float)TextureDesc.Width;
	m_ShadowViewPort.Height = (_float)TextureDesc.Height;

	return S_OK;
}

HRESULT CRenderer::Ready_StatusTarget()
{
	m_StatusViewPort = m_OriginViewPort;
	m_StatusViewPort.Width = 528.f * 2.f;
	m_StatusViewPort.Height = 404.f * 2.f;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Status"), (_uint)m_StatusViewPort.Width, (_uint)m_StatusViewPort.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Status"), TEXT("Target_Status"))))
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = (_uint)m_StatusViewPort.Width;
	TextureDesc.Height = (_uint)m_StatusViewPort.Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc = { DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2D, 0 };
	DSVDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, &DSVDesc, &m_pStatusDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);


	return S_OK;
}

HRESULT CRenderer::Ready_TextTarget()
{
	m_TextViewPort = m_OriginViewPort;
	m_TextViewPort.Width = 400.f;
	m_TextViewPort.Height = 400.f;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Text"), (_uint)m_TextViewPort.Width, (_uint)m_TextViewPort.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Text"), TEXT("Target_Text"))))
		return E_FAIL;


	return S_OK;
}


void CRenderer::Set_Active_MotionBlur(_bool bActive, _float fStrength)
{
	m_bMotionBlur = bActive;

	if (bActive)
		m_pDeferredShader->Bind_RawValue("g_MotionBlurStrength", &fStrength, sizeof(_float));
}

void CRenderer::Update_HitGlitch(_float fTimeDelta)
{
	m_fGlitchDuration -= fTimeDelta;
	if (m_fGlitchDuration < 0.f)
		m_bHitGlitch = false;
}


void CRenderer::Set_Active_Shadow(_bool bActive)
{
	m_bRenderShadow = bActive;

	m_pDeferredShader->Bind_RawValue("g_bShadow", &m_bRenderShadow, sizeof(_bool));
}

void CRenderer::Set_Active_Cascade(_bool bActive)
{
	m_bRenderCascade = bActive;

	m_pDeferredShader->Bind_RawValue("g_bCascade", &m_bRenderCascade, sizeof(_bool));
}

void CRenderer::Set_Active_Fog(_bool bActive, const FOG_DESCS* pFogDescs)
{
	m_bNoiseFog = false;

	m_pDeferredShader->Bind_RawValue("g_bFog", &bActive, sizeof(_bool));
	if (bActive && nullptr != pFogDescs)
	{
		m_pDeferredShader->Bind_RawValue("g_vFogColor", &pFogDescs->vFogColor, sizeof(_float4));
		m_pDeferredShader->Bind_RawValue("g_fFogHeightStart", &pFogDescs->fFogHeightStart, sizeof(_float));
		m_pDeferredShader->Bind_RawValue("g_fFogHeightEnd", &pFogDescs->fFogHeightEnd, sizeof(_float));
		m_pDeferredShader->Bind_RawValue("g_fFogDistStart", &pFogDescs->fFogDistStart, sizeof(_float));
		m_pDeferredShader->Bind_RawValue("g_fFogDistEnd", &pFogDescs->fFogDistEnd, sizeof(_float));
		m_pDeferredShader->Bind_RawValue("g_fFogDensity", &pFogDescs->fFogDensity, sizeof(_float));

		m_pDeferredShader->Bind_RawValue("g_bNoiseFog", &pFogDescs->bNoisedFog, sizeof(_bool));
		if (&pFogDescs->bNoisedFog)
		{
			m_bNoiseFog = true;
			m_fFogTime = 0.f;
			m_pDeferredShader->Bind_RawValue("g_fFogSpeed", &pFogDescs->fFogSpeed, sizeof(_float));
			m_pDeferredShader->Bind_RawValue("g_fNoiseScale", &pFogDescs->fNoiseScale, sizeof(_float));
		}
	}
}

void CRenderer::Set_DOF_DistMin(_float fDist)
{
	m_pDeferredShader->Bind_RawValue("g_fDOFMinDepth", &fDist, sizeof(_float));
}

void CRenderer::Set_DOF_DistMax(_float fDist)
{
	m_pDeferredShader->Bind_RawValue("g_fDOFMaxDepth", &fDist, sizeof(_float));
}

void CRenderer::Set_SkyDescs(const SKY_DESCS& SkyDescs)
{
	m_pSkyShader->Bind_RawValue("g_vTopColor", &SkyDescs.vTopColor, sizeof(_float4));
	m_pSkyShader->Bind_RawValue("g_vBottomColor", &SkyDescs.vBottomColor, sizeof(_float4));
	m_pSkyShader->Bind_RawValue("horizon_color", &SkyDescs.vHorizon_Color, sizeof(_float3));
	m_pSkyShader->Bind_RawValue("horizon_blur", &SkyDescs.fHorizon_Blur, sizeof(_float));
	m_pSkyShader->Bind_RawValue("sun_color", &SkyDescs.vSunColor, sizeof(_float3));
	m_pSkyShader->Bind_RawValue("light_direction", &SkyDescs.vSunDir, sizeof(_float3));
	m_pSkyShader->Bind_RawValue("sun_size", &SkyDescs.fSunSize, sizeof(_float));
	m_pSkyShader->Bind_RawValue("sun_blur", &SkyDescs.fSunBlur, sizeof(_float));
	m_pSkyShader->Bind_RawValue("clouds_edge_color", &SkyDescs.vCloud_EdgeColor, sizeof(_float3));
	m_pSkyShader->Bind_RawValue("clouds_top_color", &SkyDescs.vCloud_TopColor, sizeof(_float3));
	m_pSkyShader->Bind_RawValue("clouds_middle_color", &SkyDescs.vCloud_MiddleColor, sizeof(_float3));
	m_pSkyShader->Bind_RawValue("clouds_bottom_color", &SkyDescs.vCloud_BottomColor, sizeof(_float3));
	m_pSkyShader->Bind_RawValue("clouds_speed", &SkyDescs.fCloud_Speed, sizeof(_float));
	m_pSkyShader->Bind_RawValue("clouds_direction", &SkyDescs.fCloud_Direction, sizeof(_float));
	m_pSkyShader->Bind_RawValue("clouds_scale", &SkyDescs.fCloud_Scale, sizeof(_float));
	m_pSkyShader->Bind_RawValue("clouds_cutoff", &SkyDescs.fCloud_CutOff, sizeof(_float));
	m_pSkyShader->Bind_RawValue("clouds_fuzziness", &SkyDescs.fCloud_Fuzziness, sizeof(_float));
	m_pSkyShader->Bind_RawValue("clouds_weight", &SkyDescs.fCloud_Weight, sizeof(_float));
	m_pSkyShader->Bind_RawValue("clouds_blur", &SkyDescs.fCloud_Blur, sizeof(_float));
}

void CRenderer::Set_SkyColorStrength(_float fStrength)
{
	m_pSkyShader->Bind_RawValue("g_fColorStrength", &fStrength, sizeof(_float));

}

HRESULT CRenderer::Ready_Buffers()
{
	m_pVIBuffer = CVIBuffer_Screen::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pVIBuffer_Rect = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer_Rect)
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Ready_Shaders()
{
	m_pDeferredShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements, false);
	if (nullptr == m_pDeferredShader)
		return E_FAIL;

	m_pBloomShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Bloom.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements, false);
	if (nullptr == m_pBloomShader)
		return E_FAIL;

	m_pPostProcessShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcess.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements, false);
	if (nullptr == m_pPostProcessShader)
		return E_FAIL;

	m_pSkyShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements, false);
	if (nullptr == m_pSkyShader)
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Bind_Matrices()
{
	_float4x4 ViewMatrix, ProjMatrix;
	XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(1280.f, 720.f, 0.f, 1.f)));

	if (FAILED(m_pDeferredShader->Bind_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pDeferredShader->Bind_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;


	return S_OK;
		
}

void CRenderer::Active_RadialBlur(const RADIALBLUR_DESCS& Descs)
{
	Safe_Release(m_tRadialBlurDescs.pActor);

	m_bRadialBlur = true;
	m_tRadialBlurDescs = Descs;
	m_fNowBlurStrength = Descs.fBlurStrength;
	m_fRadialBlurLerpTimeAcc = 0.f;

	Safe_AddRef(Descs.pActor);
}

void CRenderer::Inactive_RadialBlur(_float fLerpTime)
{
	if (false == m_bRadialBlur || m_fRadialBlurLerpTimeAcc > 0.f)
		return;

	m_fRadialBlurLerpTime = m_fRadialBlurLerpTimeAcc = fLerpTime;
}


void CRenderer::Set_Active_IBL(_bool bActive, const IBL_TEXTURES* pIBL_Textures)
{
	m_bIBL = bActive;

	if (FAILED(m_pDeferredShader->Bind_RawValue("g_bIBL", &m_bIBL, sizeof(_bool))))
		return;

	if (false == m_bIBL)
		return;
	
	Safe_Release(m_tIBLTextures.pBRDFTexture);
	Safe_Release(m_tIBLTextures.pIrradianceIBLTexture);
	Safe_Release(m_tIBLTextures.pSpecularIBLTexture);

	m_tIBLTextures = *pIBL_Textures;

	if (m_tIBLTextures.pBRDFTexture)
		m_tIBLTextures.pBRDFTexture->Set_SRV(m_pDeferredShader, "g_BRDFTexture");

	if (m_tIBLTextures.pSpecularIBLTexture)
		m_tIBLTextures.pSpecularIBLTexture->Set_SRV(m_pDeferredShader, "g_SpecularIBLTexture");

	if (m_tIBLTextures.pIrradianceIBLTexture)
		m_tIBLTextures.pIrradianceIBLTexture->Set_SRV(m_pDeferredShader, "g_IrradianceIBLTexture");


}

void CRenderer::Set_AmbientStrength(_float fStrength)
{
	m_pDeferredShader->Bind_RawValue("g_fAmbientStrength", &fStrength, sizeof(_float));
	m_fAmbientStrength = fStrength;
}



void CRenderer::Update_RadialBlur(_float fTimeDelta)
{
	if (nullptr != m_tRadialBlurDescs.pActor)
		XMStoreFloat3(&m_vBlurCenterWorld, m_tRadialBlurDescs.pActor->Get_Center());

	if (m_fRadialBlurLerpTimeAcc > 0.f)
	{
		m_fRadialBlurLerpTimeAcc -= fTimeDelta;

		_float fRatio = m_fRadialBlurLerpTimeAcc / m_fRadialBlurLerpTime;

		if (m_fRadialBlurLerpTimeAcc < 0.f)
		{
			m_bRadialBlur = false;
			m_fRadialBlurLerpTimeAcc = 0.f;
			return;
		}
		m_fNowBlurStrength = XMVectorLerp(XMVectorZero(), XMVectorSet(m_tRadialBlurDescs.fBlurStrength, 0.f, 0.f, 0.f), fRatio).m128_f32[0];
	}
}

_float2 CRenderer::Calc_BlurCenterUV()
{
	_matrix ViewMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	_vector vBlurCenter = XMLoadFloat3(&m_vBlurCenterWorld);
	vBlurCenter = XMVector3TransformCoord(vBlurCenter, ViewMatrix);
	vBlurCenter = XMVector3TransformCoord(vBlurCenter, ProjMatrix);

	vBlurCenter = vBlurCenter * 0.5f + XMVectorSet(0.5f, 0.5f, 0.f, 0.f);

	_float2 vCenterUV;
	XMStoreFloat2(&vCenterUV, vBlurCenter);

	return vCenterUV;
}

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pContext);
	if (FAILED(pInstance->Initialize())) 
	{
		MSG_BOX(TEXT("Failed to Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pDeferredShader);
	Safe_Release(m_pBloomShader);
	Safe_Release(m_pPostProcessShader);
	Safe_Release(m_pSkyShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVIBuffer_Rect);

	Safe_Release(m_pVIBuffer_UI);

	for (auto& pShader : m_UsingShaders)
		Safe_Release(pShader);
	
	m_UsingShaders.clear();

	Safe_Release(m_tRadialBlurDescs.pActor);

	Safe_Release(m_pSSAONoiseSRV);
	Safe_Release(m_pSSAONoiseTex);
	
	Safe_Release(m_tIBLTextures.pBRDFTexture);
	Safe_Release(m_tIBLTextures.pIrradianceIBLTexture);
	Safe_Release(m_tIBLTextures.pSpecularIBLTexture);

	Safe_Release(m_pCascadeDSV);
	Safe_Release(m_pCascadeSRV);

	Safe_Release(m_pShadowDSV);
	Safe_Release(m_pShadowSRV);

	Safe_Release(m_pFogNoiseTexture);


	for (auto& RenderObjects : m_RenderObjects)
	{		
		for (auto& pRenderObject : RenderObjects)
			Safe_Release(pRenderObject);
		RenderObjects.clear();
	}

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
