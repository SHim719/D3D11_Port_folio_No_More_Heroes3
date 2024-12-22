#pragma once

#include "Engine_Defines.h"
#include "Client_Defines.h"


BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)
class CCinematic_Manager
{
	DECLARE_SINGLETON(CCinematic_Manager)

private:
	CCinematic_Manager();
	~CCinematic_Manager();

public:
	HRESULT Load_CinematicDatas(const wstring& wstrFolderPath);
	HRESULT Bind_DefaultEvents();

	void Tick(_float fTimeDelta);

private:
	CGameInstance* m_pGameInstance = { nullptr };

	// ù��° Ű �� - �ó׸�ƽ ���� �̸�
	unordered_map<string, CINEMATIC_DESCS>							m_CamCinematicDescs;
	// ù��° Ű �� - �ó׸�ƽ ���� �̸�, �ι�° Ű �� - ���� �±�.
	unordered_map<string, unordered_map<string, CINEMATIC_DESCS>>	m_CinematicDescs;

	// �ó׸�ƽ �÷���Ÿ�� 
	unordered_map<string, _float>									m_CinemaitcPlayTime;

	vector<CGameObject*>	m_CinemaObjects;

	string	m_strNowCinema = { "" };
	_float	m_fPlayTimeAcc = { 0.f };
	_float	m_fPlayTime = { 0.f };

	function<void()>	m_CinemaEndFunc = { nullptr };

public:
	// ù��° Ű �� - �ó׸�ƽ ���� �̸�
	_bool Bind_CamEvent(const string& strFirstKey, _float fTime, const CINE_EVENT& Event);
	// ù��° Ű �� - �ó׸�ƽ ���� �̸�, �ι�° Ű �� - ���� �±�.
	_bool Bind_ActorEvent(const string& strFirstKey, const string& strSecondKey, _float fTime, const CINE_EVENT& Event);

	const CINEMATIC_DESCS& Get_CamCinemaDesc(const string& strCinematicName) const;
	const CINEMATIC_DESCS& Get_ActorCinemaDesc(const string& strCinematicName, const string& strActorTag) const;

	void Add_CinemaObject(CGameObject* pCinemaObj);
	void Clear_CinemaObjects();
	_bool Start_Cinematic(const string& strCinematicName);
	void End_Cinematic();

	void Bind_CinemaEndFunc(const function<void()>& pFunc) {
		m_CinemaEndFunc = pFunc;
	}

	_float Get_PlayTimeAcc() const {
		return m_fPlayTimeAcc;
	}

private:
	void Release();

};

END


