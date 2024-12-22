#pragma once
#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL CAABB final : public CCollider
{
protected:
	CAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAABB(const CAABB& rhs);
	virtual ~CAABB() = default;

public:
	HRESULT Initialize_Prototype()	override;
	HRESULT Initialize(void* pArg)	override;
	void	Update(_fmatrix TransformMatrix)			override;

	_bool Intersects(CCollider* pTargetCollider)		override;
	virtual _bool Intersects(_fvector _vRayOrigin, _fvector _vRayDir, _float& _fDist) override;	// BH - Ray Checking
	void Remake_Collider(COLLIDERDESC* pColliderDesc)	override;

	_vector Get_Size() const							override;
	void	Set_Size(_fvector vSize)					override;
	void	Set_Center(_fvector vCenter)				override;

#ifdef _DEBUG
public:
	HRESULT  Render() override;
#endif 

private:
	BoundingBox* m_pOriginal_AABB = nullptr;
	BoundingBox* m_pAABB = nullptr;

public:
	BoundingBox* Get_Collider() {
		return m_pAABB;
	}

	_vector Get_Center() const override {
		return XMLoadFloat3(&m_pAABB->Center);
	}

public:
	static CAABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;

};

END

