#pragma once

#include "Tool_Super.h"
#include "KeyFrameEvent.h"


BEGIN(Tool)

class CAnim_Tool : public CTool_Super
{
protected:
	CAnim_Tool();
	virtual ~CAnim_Tool() = default;

public:
	HRESULT Initialize()			override;
	void Start_Tool()				override;
	void Tick(_float fTimeDelta)	override;

private:
	HRESULT Load_KeyFrameNames(_int iSelModelIdx);
	HRESULT Load_KeyFrames();
	HRESULT Load_KeyFrameData();
	HRESULT Load_OldKeyFrameData();

	HRESULT Save_KeyFrameData();

	HRESULT Load_Sounds();

	wstring Get_FolderPath(const _tchar* szInitialPath);
private:
	void Main_Window();

	void Menu_Bar();
	void Tab_Bar();

	void Model_ListBox();
	void Model_Button();

	void Anim_ListBox();
	void Anim_Buttons();

	void KeyFrame_Window();
	void KeyFrameEvent_ComboBox();
	void KeyFrameEvent_ListBox();
	void KeyFrameEvent_Button();

	void Sound_ListBox();    
	void Sound_Buttons();
	void Check_KeyFrameSound();

	void Bone_ListBox();
	void Collider_ListBox();
	void ColliderType_CheckBox();
	void Collider_Buttons();

private:
	class CToolAnimObj*	m_pAnimObj = nullptr;
	class CModel*		m_pModel = nullptr;

	vector<string>		m_strModelLists;
	_int				m_iSelModelIdx = 0;

	vector<string>		m_strAnimations;
	_int				m_iSelAnimIdx = 0;
	_float				m_fAnimListBoxStartCursorY;

	vector<string>		m_strEventNames;
	_int				m_iSelEventName = 0;
	_char				m_szEventName[MAX_PATH] = "";

	vector<string>		m_strSounds;
	_int				m_iSelSoundIdx = { 0 };
	_uint				m_iCurKeyFrame = { 0 };
	_uint				m_iPrevKeyFrame = { 0 };


	vector<vector<pair<_int, CKeyFrameEvent::KEYFRAME_DESC>>>	m_KeyFrameEvents;
	_int														m_iSelKeyFrameIdx = 0;

	const char**					m_szBoneNames = nullptr;
	_int							m_iNumBones = 0;
	_int							m_iSelBoneIdx = 0;

	vector<class CToolColliderObj*>	m_Colliders;
	vector<string>					m_strColliders;
	_int							m_iSelColliderIdx = 0;

	CCollider::COLLIDERDESC			m_ColliderDesc;
	_bool							m_bColliderTypes[CCollider::Collider_End] = {};
	_int							m_iTypeIdx = 0;


	class CEffect_Tool*		m_pEffectTool = { nullptr };
	_float					m_fTimeDelta = { 0.f };

public:
	static CAnim_Tool* Create();
	void Free() override;
};

END

