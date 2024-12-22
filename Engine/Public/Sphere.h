#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CSphere final : public CCollider
{
protected:
	CSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSphere(const CSphere& rhs);
	virtual ~CSphere() = default;

	_bool Intersects(CCollider* pTargetCollider)		override;
	virtual _bool Intersects(_fvector _vRayOrigin, _fvector _vRayDir, _float& _fDist) override;	// BH - Ray Checking

	void Remake_Collider(COLLIDERDESC* pColliderDesc)	override;
public:
	BoundingSphere* Get_Collider() {
		return m_pSphere;
	}

public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)								override;
	void Update(_fmatrix TransformMatrix)						override;

	void Set_Size(_fvector vSize) override
	{
		m_pOriginal_Sphere->Radius = XMVectorGetX(vSize);
	};

	 _vector Get_Center() const  override{
		return XMLoadFloat3(&m_pSphere->Center);
	}

#ifdef _DEBUG
public:
	HRESULT Render() override;
#endif 

private:
	BoundingSphere* m_pSphere = nullptr;
	BoundingSphere* m_pOriginal_Sphere = nullptr;

public:
	static CSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent* Clone(void* pArg) override;
	void Free() override;

};

END