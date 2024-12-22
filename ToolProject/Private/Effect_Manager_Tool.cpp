#include "Effect_Manager_Tool.h"
#include "Effect_Group_Tool.h"
#include "Effect.h"
#include "Effect_Mesh_Tool.h"


IMPLEMENT_SINGLETON(CEffect_Manager_Tool)

CEffect_Manager_Tool::CEffect_Manager_Tool()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
}

CEffect_Manager_Tool::~CEffect_Manager_Tool()
{
	Release();
}

HRESULT CEffect_Manager_Tool::Initialize(_uint iLevel)
{
	m_iMaxLevel = iLevel;
	m_EffectGroups = new unordered_map<string, vector<CEffect_Group_Tool*>>[iLevel];

	return S_OK;
}

void CEffect_Manager_Tool::Active_Effect(_uint iLevel, const string& strTag, void* pArg, _int* iIndex)
{
	auto it = m_EffectGroups[iLevel].find(strTag);
	if (m_EffectGroups[iLevel].end() == it)
		return;

	vector<CEffect_Group_Tool*>& EffectGroups = it->second;
	_int iGroupIdx = -1;
	for (auto& pEffect_Group : EffectGroups)
	{
		++iGroupIdx;
		if (false == pEffect_Group->Is_Using())
		{
			pEffect_Group->OnEnter_Layer(pArg);
			Safe_AddRef(pEffect_Group);
			if (nullptr != iIndex)
				*iIndex = iGroupIdx;
			ADD_EVENT(bind(&CGameInstance::Insert_GameObject, m_pGameInstance, GET_CURLEVEL, L"Effect", pEffect_Group));
			return;
		}
	}

}


void CEffect_Manager_Tool::End_Effect(_uint iLevel, const string& strTag, size_t iIdx)
{
	auto it = m_EffectGroups[iLevel].find(strTag);
	if (m_EffectGroups[iLevel].end() == it)
		return;
	m_iIndex = 0;
	vector<CEffect_Group_Tool*>& EffectGroups = it->second;

	EffectGroups[iIdx]->End_Effect();
}

CEffect_Group_Tool* CEffect_Manager_Tool::Get_EffectGroup(_uint iLevel, const string& strTag)
{
	auto it = m_EffectGroups[iLevel].find(strTag);
	if (m_EffectGroups[iLevel].end() == it)
		return nullptr;

	vector<CEffect_Group_Tool*>& EffectGroups = it->second;

	for (auto& pEffect_Group : EffectGroups)
	{
		if (false == pEffect_Group->Is_Using())
			return pEffect_Group;
	}

	return nullptr;
}

HRESULT CEffect_Manager_Tool::Effect_Loader(LEVEL eLevel ,const wstring& wstrSavePath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	ifstream fin(wstrSavePath);
	if (!fin.is_open())
	{
		MSG_BOX(TEXT("FAILED TO OPEN SAVE FILE"));
		return E_FAIL;
	}
	
	_tchar wszFileName[MAX_PATH] = { TEXT("") };
	_wsplitpath_s(wstrSavePath.c_str(), nullptr, 0, nullptr, 0, wszFileName, MAX_PATH, nullptr, 0);
	
	json jsonData;
	fin >> jsonData;
	fin.close();
	
	DWORD dwByte = { 0 };
	_uint iType = jsonData["Common"]["Load Type"];
	
	if (0 == iType)
	{
		MSG_BOX(TEXT("FAILED - Try To Load Single Effect"));
		return E_FAIL;
	}
	
	_char szGroupTag[MAX_PATH] = "";
	strcpy_s(szGroupTag, MAX_PATH, string(jsonData["Common"]["Group Data"]["Group Tag"]).c_str());
	string strGrouptag = szGroupTag;

	_uint iCloneNum = jsonData["Common"]["Group Data"]["Clone Number"];
	
	CEffect_Group_Tool* pNewGroup = dynamic_cast<CEffect_Group_Tool*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_GroupEffect")));
	pNewGroup->Set_GroupTag(strGrouptag.c_str());
	pNewGroup->Load_Group_CINEMA(jsonData);


	if (FAILED(m_pGameInstance->Add_Prototype(wszFileName, pNewGroup)))
	{
		Safe_Release(pNewGroup);
	}
	
	if (FAILED(Add_EffectGroups(eLevel, Convert_WStrToStr(wszFileName), iCloneNum, wszFileName)))
		return E_FAIL;


	return S_OK;
}

void CEffect_Manager_Tool::Clear(_uint iLevel)
{
	
	for (auto& LevelMap : m_EffectGroups[LEVEL_STATIC])
		for (auto& Vec : LevelMap.second)
			Vec->End_Effect();
		
	if (true == m_EffectGroups[iLevel].empty())
		return;

	for (auto& Pair : m_EffectGroups[iLevel])
	{
		for (auto& pEffect_Group : Pair.second)
			Safe_Release(pEffect_Group);
	}
}

void CEffect_Manager_Tool::End_AllEffects(_uint iLevel)
{
	for (auto& LevelMap : m_EffectGroups[iLevel])
		for (auto& Vec : LevelMap.second)
			Vec->End_Effect();
}




HRESULT CEffect_Manager_Tool::Add_EffectGroups(_uint iLevel, const string& strEffectGroupTags, _uint iNumGroups, const wstring& wstrPrototypeTag)
{
	vector<CEffect_Group_Tool*> vecGroups;
	vecGroups.reserve(iNumGroups);

	for (_uint i = 0; i < iNumGroups; ++i)
	{
		CEffect_Group_Tool* pEffectGroup = static_cast<CEffect_Group_Tool*>(m_pGameInstance->Clone_GameObject(wstrPrototypeTag));
		if (nullptr == pEffectGroup)
			return E_FAIL;
		vecGroups.emplace_back(pEffectGroup);
	}

	m_EffectGroups[iLevel].emplace(strEffectGroupTags, vecGroups);

	return S_OK;
}

void CEffect_Manager_Tool::Release()
{
	for (_uint i = 0; i < m_iMaxLevel; ++i)
	{
		for (auto& Pair : m_EffectGroups[i])
		{
			for (auto& pEffect_Group : Pair.second)
				Safe_Release(pEffect_Group);
			Pair.second.clear();
		}
		m_EffectGroups[i].clear();
	}
	Safe_Delete_Array(m_EffectGroups);

}
