#pragma once

#include "Base.h"

#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"
#include "Transform.h"
#include "Rigidbody.h"

#include "VIBuffer_Rect.h"
#include "VIBuffer_Point.h"
#include "VIBuffer_Cell.h"
#include "VIBuffer_Sphere.h"
#include "VIBuffer_Particle.h"
#include "VIBuffer_Screen.h"
#include "VIBuffer_Rect_NorTex.h"
#include "VIBuffer_UI_Rect_Instance.h"
#include "VIBuffer_SwordTrail.h"
#include "VIBuffer_Cube.h"
#include "CVIBuffer_Terrain.h"
#include "VIBuffer_SkyPlane.h"
#include "VIBuffer_Grid.h"

#include "Model.h"
#include "Model_Instance.h"

#include "Navigation.h"

#include "AABB.h"
#include "OBB.h"
#include "Sphere.h"
#include "Compute_Shader.h"


BEGIN(Engine)

class CComponent_Manager final : public CBase
{
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);

private:
	_uint											m_iNumLevels = { 0 };
	map<const wstring, class CComponent*>*			m_pPrototypes = { nullptr };
	typedef map<const wstring, class CComponent*>	PROTOTYPES;

public:
	class CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	static CComponent_Manager* Create(_uint iNumLevels);
	void Free() override;
};

END