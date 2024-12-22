#pragma once

#include "Client_Defines.h"
#include "Level.h"
#include "MapLoader.h"

BEGIN(Client)

class CClientLevel :public CLevel
{
protected:
	CClientLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CClientLevel() = default;

public:
	void Tick(_float fTimeDelta);

protected:
	HRESULT Initialize_IBL(const wstring& wstrTag);

protected:
	wstring			m_wstrBGMTag = L"";
	IBL_TEXTURES	m_IBLTextures;

protected:
	virtual void OnExitLevel();

public:
	void Free() override;

};

END

