#include "Cinematic_Manager.h"
#include "Various_Camera.h"

#include "GameInstance.h"

#include "Effect_Manager.h"

#include "Character.h"


IMPLEMENT_SINGLETON(CCinematic_Manager)


CCinematic_Manager::CCinematic_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

CCinematic_Manager::~CCinematic_Manager()
{
	Release();
}

HRESULT CCinematic_Manager::Load_CinematicDatas(const wstring& wstrFolderPath)
{
	fs::path folderPath(wstrFolderPath);

	for (const fs::directory_entry& entry : fs::directory_iterator(folderPath))
	{
		if (entry.is_directory())
			continue;

		if (entry.path().extension().generic_string() != ".json")
			continue;

		wstring wstrPath = entry.path().generic_wstring();

		string strFileName = entry.path().filename().stem().generic_string();

		ifstream fin(wstrPath);

		if (!fin.is_open())
			return E_FAIL;

		json jsonData;

		fin >> jsonData;
		fin.close();

		m_CinemaitcPlayTime.emplace(strFileName, jsonData["PlayTime"]);

		CINEMATIC_DESCS CamCinemaDesc;

		const auto& CamPositionData = jsonData["Camera Desc"]["Positions"];
		if (!CamPositionData.is_null())
		{
			for (size_t i = 0; i < CamPositionData["Time"].size(); ++i)
			{
				_float fTime = CamPositionData["Time"][i];
				_float3 vPosition;
				memcpy(&vPosition, CamPositionData["Position"][i].get<array<_float, 3>>().data(), sizeof(_float3));
				_bool bLerp = CamPositionData["Lerp?"][i];

				CamCinemaDesc.Positions.emplace(fTime, make_pair(vPosition, bLerp));
			}
		}

		const auto& CamLookData = jsonData["Camera Desc"]["Looks"];

		if (!CamLookData.is_null())
		{
			for (size_t i = 0; i < CamLookData["Time"].size(); ++i)
			{
				_float fTime = CamLookData["Time"][i];
				_float3 vLook;
				memcpy(&vLook, CamLookData["Look"][i].get<array<_float, 3>>().data(), sizeof(_float3));
				_bool bLerp = CamLookData["Lerp?"][i];

				CamCinemaDesc.Looks.emplace(fTime, make_pair(vLook, bLerp));
			}

		}

		const auto& CamEventData = jsonData["Camera Desc"]["Events"];

		if (!CamEventData.is_null())
		{
			for (size_t i = 0; i < CamEventData["Time"].size(); ++i)
			{
				CINE_EVENT CineEvent;

				_float fTime = CamEventData["Time"][i];
				CineEvent.eEventType = CamEventData["EventType"][i];
				CineEvent.strParam = CamEventData["Param"][i];

				CamCinemaDesc.Events.emplace(fTime, CineEvent);
			}

		}

		m_CamCinematicDescs.emplace(strFileName, CamCinemaDesc);


		for (const auto& ActorTag : jsonData["ActorTag"])
		{
			CINEMATIC_DESCS CinemaDesc;

			const auto& PositionData = jsonData[ActorTag]["Positions"];

			if (!PositionData.is_null())
			{
				for (size_t i = 0; i < PositionData["Time"].size(); ++i)
				{
					_float fTime = PositionData["Time"][i];
					_float3 vPosition;
					memcpy(&vPosition, PositionData["Position"][i].get<array<_float, 3>>().data(), sizeof(_float3));
					_bool bLerp = PositionData["Lerp?"][i];

					CinemaDesc.Positions.emplace(fTime, make_pair(vPosition, bLerp));
				}
			}

			const auto& LookData = jsonData[ActorTag]["Looks"];

			if (!LookData.is_null())
			{
				for (size_t i = 0; i < LookData["Time"].size(); ++i)
				{
					_float fTime = LookData["Time"][i];
					_float3 vLook;
					memcpy(&vLook, LookData["Look"][i].get<array<_float, 3>>().data(), sizeof(_float3));
					_bool bLerp = LookData["Lerp?"][i];

					CinemaDesc.Looks.emplace(fTime, make_pair(vLook, bLerp));
				}
			}


			const auto& ScaleData = jsonData[ActorTag]["Scales"];


			if (!ScaleData.is_null())
			{
				for (size_t i = 0; i < ScaleData["Time"].size(); ++i)
				{
					_float fTime = ScaleData["Time"][i];
					_float3 vScale;
					memcpy(&vScale, ScaleData["Scale"][i].get<array<_float, 3>>().data(), sizeof(_float3));
					_bool bLerp = ScaleData["Lerp?"][i];

					CinemaDesc.Scales.emplace(fTime, make_pair(vScale, bLerp));
				}
			}


			const auto& EventData = jsonData[ActorTag]["Events"];

			if (!EventData.is_null())
			{
				for (size_t i = 0; i < EventData["Time"].size(); ++i)
				{
					CINE_EVENT CineEvent;

					_float fTime = EventData["Time"][i];
					CineEvent.eEventType = EventData["EventType"][i];
					CineEvent.strParam = EventData["Param"][i];

					CinemaDesc.Events.emplace(fTime, CineEvent);
				}
			}

			m_CinematicDescs[strFileName].emplace(ActorTag, CinemaDesc);
		}
	}
	
	return S_OK;
}

HRESULT CCinematic_Manager::Bind_DefaultEvents()
{
	for (auto& it : m_CamCinematicDescs)
	{
		CINEMATIC_DESCS& CinemaDesc = it.second;

		for (auto& EventIt : CinemaDesc.Events)
		{
			switch (EventIt.second.eEventType)
			{
			case PLAY_SOUND:
				EventIt.second.pFunc = bind(&CGameInstance::Play, m_pGameInstance, EventIt.second.strParam, false, 1.f);
				break;
			case STOP_SOUND:
				EventIt.second.pFunc = bind(&CGameInstance::Stop, m_pGameInstance, EventIt.second.strParam);
				break;
			}
		}
	}

	for (auto& CinemaIt : m_CinematicDescs)
	{
		for (CGameObject*& pCinemaObj : m_CinemaObjects)
		{
			CINEMATIC_DESCS& CinemaDesc = CinemaIt.second[pCinemaObj->Get_CinemaActorTag()];
			for (auto& EventIt : CinemaDesc.Events)
			{
				switch (EventIt.second.eEventType)
				{
				case CHANGE_ANIMATION:
				{
					CModel* pModel = static_cast<CModel*>(pCinemaObj->Find_Component(L"Model"));
					EventIt.second.pFunc = bind(&CModel::Change_Animation, pModel, (_uint)stoi(EventIt.second.strParam), 0.1f, true);
					break;
				}
				case PLAY_SOUND:
					EventIt.second.pFunc = bind(&CGameInstance::Play, m_pGameInstance, EventIt.second.strParam, false, 1.f);
					break;
				case STOP_SOUND:
					EventIt.second.pFunc = bind(&CGameInstance::Stop, m_pGameInstance, EventIt.second.strParam);
					break;
				case ACTIVE_EFFECT:
					EventIt.second.pFunc = bind(&CEffect_Manager::Active_Effect, EFFECTMGR, LEVEL_STATIC, EventIt.second.strParam,
						static_cast<CCharacter*>(pCinemaObj)->Get_EffectDescPtr(), nullptr);
					break;
				case INACITVE_EFFECT:
					EventIt.second.pFunc = bind(&CEffect_Manager::Inactive_Effect, EFFECTMGR, LEVEL_STATIC, EventIt.second.strParam, 0);
					break;
				}
			}
		}

	}
	return S_OK;
}

void CCinematic_Manager::Tick(_float fTimeDelta)
{
	if (m_fPlayTime > 0.f)
	{
		m_fPlayTimeAcc += fTimeDelta;
		if (m_fPlayTimeAcc >= m_fPlayTime)
			End_Cinematic();
	}


}

void CCinematic_Manager::Clear_CinemaObjects()
{
	for (auto& pCinemaObject : m_CinemaObjects)
	{
		pCinemaObject->Set_Destroy(true);
		Safe_Release(pCinemaObject);
	}
	

	m_CinemaObjects.clear();
}


_bool CCinematic_Manager::Bind_CamEvent(const string& strFirstKey, _float fTime, const CINE_EVENT& Event)
{
	auto FirstIt = m_CamCinematicDescs.find(strFirstKey);
	if (m_CamCinematicDescs.end() == FirstIt)
		return false;

	auto SecondIt = FirstIt->second.Events.find(fTime);
	if (FirstIt->second.Events.end() == SecondIt)
		return false;

	SecondIt->second = Event;

	return true;
}

_bool CCinematic_Manager::Bind_ActorEvent(const string& strFirstKey, const string& strSecondKey, _float fTime, const CINE_EVENT& Event)
{
	auto FirstIt = m_CinematicDescs.find(strFirstKey);
	if (m_CinematicDescs.end() == FirstIt)
		return false;

	auto SecondIt = FirstIt->second.find(strSecondKey);
	if (FirstIt->second.end() == SecondIt)
		return false;

	auto ThirdIt = SecondIt->second.Events.find(fTime);
	if (SecondIt->second.Events.end() == ThirdIt)
		return false;

	ThirdIt->second = Event;


	return true;
}

const CINEMATIC_DESCS& CCinematic_Manager::Get_CamCinemaDesc(const string& strCinematicName) const
{
	auto it = m_CamCinematicDescs.find(strCinematicName);

	return it->second;
}

const CINEMATIC_DESCS& CCinematic_Manager::Get_ActorCinemaDesc(const string& strCinematicName, const string& strActorTag) const
{
	auto CinematicIter = m_CinematicDescs.find(strCinematicName);

	auto CinemaDescIter = CinematicIter->second.find(strActorTag);

	return CinemaDescIter->second;
}


void CCinematic_Manager::Add_CinemaObject(CGameObject* pCinemaObj)
{
	m_CinemaObjects.emplace_back(pCinemaObj);
	Safe_AddRef(pCinemaObj);

}

_bool CCinematic_Manager::Start_Cinematic(const string& strCinematicName)
{
	m_strNowCinema = strCinematicName;
	m_fPlayTime = m_CinemaitcPlayTime[m_strNowCinema];
	m_fPlayTimeAcc = 0.f;
	
	GET_CAMERA->Start_Cinematic(Get_CamCinemaDesc(strCinematicName));

	for (auto& pCinemaObj : m_CinemaObjects)
	{
		pCinemaObj->Start_Cinematic(Get_ActorCinemaDesc(strCinematicName, pCinemaObj->Get_CinemaActorTag()));
	}
	
	return true;
}

void CCinematic_Manager::End_Cinematic()
{
	m_fPlayTimeAcc = 0.f;
	m_fPlayTime = 0.f;

	GET_CAMERA->End_Cinematic();

	for (auto& pCinemaObj : m_CinemaObjects)
	{
		pCinemaObj->End_Cinematic();
	}

	if (nullptr != m_CinemaEndFunc)
		m_CinemaEndFunc();
}

void CCinematic_Manager::Release()
{
	Safe_Release(m_pGameInstance);
	Clear_CinemaObjects();

}
