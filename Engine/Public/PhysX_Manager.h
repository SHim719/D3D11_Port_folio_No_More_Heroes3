#include "Base.h"


BEGIN(Engine)

class CPhysX_Manager final : public CBase
{
private:
	CPhysX_Manager();
	virtual ~CPhysX_Manager() = default;

	HRESULT Initialize();

private:
	//PxFoundation* m_pFoundation;

public:
	static CPhysX_Manager* Create();
	void Free() override;
};

END