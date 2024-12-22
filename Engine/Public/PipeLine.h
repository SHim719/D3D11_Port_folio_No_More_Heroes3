#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPipeLine final : public CBase
{
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);
	void Set_CascadeViewProj(_uint iIndex, _fmatrix CascadeViewProj);

	_matrix Get_TransformMatrix(TRANSFORMSTATE eTransformState) const {
		return XMLoadFloat4x4(&m_TransformMatrix[eTransformState]);
	}

	_float4x4 Get_TransformFloat4x4(TRANSFORMSTATE eTransformState) const {
		return m_TransformMatrix[eTransformState];
	}

	_float4x4 Get_TransformFloat4x4_TP(TRANSFORMSTATE eTransformState) const {
		_float4x4	Transform;
		XMStoreFloat4x4(&Transform, XMMatrixTranspose(Get_TransformMatrix(eTransformState)));
		return Transform;
	}

	_matrix Get_TransformMatrixInverse(TRANSFORMSTATE eTransformState) const {
		return XMLoadFloat4x4(&m_TransformInverseMatrix[eTransformState]);
	}

	_float4x4 Get_TransformFloat4x4_Inverse(TRANSFORMSTATE eTransformState) const {
		_float4x4	Transform;
		XMStoreFloat4x4(&Transform, Get_TransformMatrixInverse(eTransformState));
		return Transform;
	}

	_float4x4 Get_TransformFloat4x4_Inverse_TP(TRANSFORMSTATE eTransformState) const {
		_float4x4	Transform;
		XMStoreFloat4x4(&Transform, XMMatrixTranspose(Get_TransformMatrixInverse(eTransformState)));
		return Transform;
	}

	const _float4& Get_CamPosition() const {
		return m_vCamPosition;
	}

	_float4x4* Get_CascadeViewProjs() {
		return m_CascadeViewProj;
	}

	void Update_ShadowViewProj(_fvector vLightDir);

	_float4x4* Get_ShadowViewProj() {
		return &m_LightViewProj;
	}

public:
	void Update();

private:
	_float4x4				m_TransformMatrix[D3DTS_END];
	_float4x4				m_TransformInverseMatrix[D3DTS_END];
	_float4					m_vCamPosition;

	_float4x4				m_CascadeViewProj[3] = {};

	_float4x4				m_LightViewProj = {};

public:
	static CPipeLine* Create();
	virtual void Free() override;
};

END