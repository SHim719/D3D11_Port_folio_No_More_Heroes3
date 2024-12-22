#include "UI_Tool.h"
#include <windows.h>
#include <shobjidl.h>
#include <iostream>

#include "GameInstance.h"
#include "Layer.h"

CUI_Tool::CUI_Tool()
	: CTool_Super()
{
}

CUI_Tool::CUI_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool_Super()
	, m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CUI_Tool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMat, XMMatrixIdentity());

	return S_OK;
}

void CUI_Tool::Start_Tool()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// 툴 폰트
	io.Fonts->AddFontFromFileTTF("../../Resources/UI/Font_UI_Tool/NanumBarunpenR.ttf", 18.f, nullptr, io.Fonts->GetGlyphRangesKorean());

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	//ImGui::StyleColorsLight();
}

void CUI_Tool::Tick(_float fTimeDelta)
{
	// 크기 위치 회전 조정 여기서
	if (KEY_DOWN(eKeyCode::T))
		m_tGuizmoDesc.CurrentGuizmoOperation = ImGuizmo::TRANSLATE;
	
	else if (KEY_DOWN(eKeyCode::R))
		m_tGuizmoDesc.CurrentGuizmoOperation = ImGuizmo::ROTATE;
	
	else if (KEY_DOWN(eKeyCode::S))
		m_tGuizmoDesc.CurrentGuizmoOperation = ImGuizmo::SCALE;
	
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuizmo::BeginFrame();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::SetOrthographic(true);

	Main_Window();
}

void CUI_Tool::LateTick(_float fTimeDelta)
{
	ImGui::EndFrame();
}

void CUI_Tool::Set_ImGuizmo()
{
	static _float fRadius = 1.f;

	_float4x4 ViewMat, ProjMat;

	/* 인스턴스 Area Obj 조회*/
	CGameObject* pObj = GetSelectedObj(true);

	/* 일반 UI Obj 조회 */
	if (pObj == nullptr)
		pObj = GetSelectedObj();

	/* 그래도 없으면 Return */
	if (pObj == nullptr)
		return;

	// 기즈모 적용
	CTransform* pUIObj = pObj->Get_Transform();

	XMStoreFloat4x4(&m_WorldMat, XMLoadFloat4x4(&pUIObj->Get_WorldFloat4x4()));
	XMStoreFloat4x4(&ViewMat, XMMatrixIdentity());
	XMStoreFloat4x4(&ProjMat, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	XMStoreFloat3(&m_vPos, pUIObj->Get_Position());
	m_vScale = pUIObj->Get_Scale();


	ImGuizmo::DecomposeMatrixToComponents(&m_WorldMat._11, &m_vPos.x, &m_vRot.x, &m_vScale.x);
	ImGui::PushItemWidth(200.f);				// 인풋 창 크기 설정하는거
	ImGui::InputFloat3("POS", &m_vPos.x);		// 이동
	ImGui::PushItemWidth(200.f);
	ImGui::InputFloat3("ROT", &m_vRot.x);		// 회전
	m_vRot.x = 0.f;
	m_vRot.y = 0.f;
	ImGui::PushItemWidth(200.f);
	ImGui::InputFloat3("SCALE", &m_vScale.x);	// 크기
	ImGuizmo::RecomposeMatrixFromComponents(&m_vPos.x, &m_vRot.x, &m_vScale.x, &m_WorldMat._11);


	ImGuizmo::Manipulate(&ViewMat._11, &ProjMat._11
		, m_tGuizmoDesc.CurrentGuizmoOperation
		, m_tGuizmoDesc.CurrentGuizmoMode
		, (_float*)&m_WorldMat
		, NULL
		, m_tGuizmoDesc.bUseSnap ? &m_tGuizmoDesc.snap[0] : NULL
		, m_tGuizmoDesc.boundSizing ? m_tGuizmoDesc.bounds : NULL
		, m_tGuizmoDesc.boundSizingSnap ? m_tGuizmoDesc.boundsSnap : NULL);

	_fmatrix UI = XMLoadFloat4x4(&m_WorldMat);
	pUIObj->Set_WorldMatrix(UI);
}

void CUI_Tool::Main_Window()
{
	ImGui::Begin("UI Tool");
	Tab_Bar();
	ImGui::End();
}

void CUI_Tool::Tab_Bar()
{
	static int iAlign = 0;
	static _float fRectSize = { 0 };
	static _float fMargin = { 0 };
	static _float fSpeed = { 0 };
	static _float4 color_rgb(1.f, 0.0f, 0.0f, 1.0f);

	// 공백
	ImVec2 Spacesize100 = { 100.f, 100.f };
	ImVec2 Spacesize5 = { 5.f, 5.f };

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		// 탭1) UI 정보 입력 및 제작
		if (ImGui::BeginTabItem(u8"UI 만들기"))
		{
			// 파일 가져오기
			ImGui::SeparatorText(u8"UI 가져오기");
			if (ImGui::Button(u8"폴더 선택")) OpenFile();
			ImGui::SameLine();
			ImGui::Text(u8"미리보기 : \n");
			ImGui::BeginChild("##TexList", ImVec2(200.f, 150.f));

			if (ImGui::BeginListBox(u8"\t\t", ImVec2(180.f, 150.f)))
			{
				for (int i = 0; i < m_vecFileName.size(); i++)
				{
					const _bool bSelect_list = (m_iFileIndex == i);
					if (ImGui::Selectable(m_vecFileName[i].c_str(), bSelect_list))
						m_iFileIndex = i;

					if (bSelect_list)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndListBox();
			}

			ImGui::EndChild();

			if (false == m_vecSRVs.empty())
			{
				ImGui::SameLine();

				ImGui::BeginChild("##이미지", ImVec2(200.f, 150.f));

				ImGui::Image((void*)m_vecSRVs[m_iFileIndex], ImVec2(150.f, 150.f));

				ImGui::EndChild();
			}

			// UI 정보 확인
			ImGui::SeparatorText(u8"UI 정보 확인");

			// 현재 콤보박스에서 선택된 레이어 테그
			auto ComboPair = m_ComTagMap.begin();

			if (true == m_ComTagMap.empty())
				strcpy_s(m_szSelectedLayer, MAX_PATH, "");

			static bool bShowAll = true;
			ImGui::Checkbox("Show All", &bShowAll);
			if (ImGui::BeginCombo(u8"\tLayer Tag", m_szSelectedLayer, ImGuiComboFlags_None))
			{
				for (_uint i = 0; i < m_ComTagMap.size(); ++i)
				{
					const _bool bSelected = (m_iLayerIndex == i);
					if (ImGui::Selectable(ComboPair->first.c_str(), bSelected))
					{
						m_iLayerIndex = i;
						strcpy_s(m_szSelectedLayer, MAX_PATH, ComboPair->first.c_str());

						if(!bShowAll)
						{
							for (auto ComTagVec : m_ComTagMap)
								Inactive_UI(Convert_StrToWStr(ComTagVec.first));

							Active_UI(Convert_StrToWStr(m_szSelectedLayer));
						}
						else
						{
							for (auto ComTagVec : m_ComTagMap)
								Active_UI(Convert_StrToWStr(ComTagVec.first));
						}
					}

					if (true == bSelected)
						ImGui::SetItemDefaultFocus();

					ComboPair++;
				}
				ImGui::EndCombo();
			}

			// 현재 레이어에 해당하는 컴포넌트 테그
			auto ListPair = m_ComTagMap.find(m_szSelectedLayer);

			if (false == m_ComTagMap.empty() || ListPair != m_ComTagMap.end())
			{
				if (ImGui::BeginListBox(u8"\t Com Tag", ImVec2(270.f, 100.f)))
				{
					for (int i = 0; i < ListPair->second.size(); i++)
					{
						if (ListPair->second.size() < 0)
							return;

						const _bool bSelected = (m_iComIndex == i);
						if (ImGui::Selectable(ListPair->second[i].c_str(), bSelected))
						{
							m_iComIndex = i;
							_int idx = 0;

							strcpy_s(m_szSelectedComTag, MAX_PATH, ListPair->second[i].c_str());
							_char findInstanceTag[MAX_PATH] = {"IN_"};

							if(NULL == strstr(m_szSelectedComTag, findInstanceTag))
							{
								// 다른 ComTag 클릭시 정보 갱신
								for (auto& DescInfo : m_pUIvec)
								{
									if (ListPair->second[i] == Convert_WStrToStr(DescInfo->szProCom))
									{
										m_iSelectedUiType = 0;
										m_pSelectedDesc = DescInfo;
										strcpy_s(m_LayerTag, MAX_PATH, Convert_WStrToStr(DescInfo->szLayer).c_str());
										strcpy_s(m_ComTag, MAX_PATH, Convert_WStrToStr(DescInfo->szProCom).c_str());
										m_iPassNum = DescInfo->iPass;
									}
									++idx;
								}
							}
							else
							{
								// 다른 ComTag 클릭시 정보 갱신
								for (auto& DescInfo : m_pUIInstanceVec)
								{
									if (ListPair->second[i] == Convert_WStrToStr(DescInfo->szProCom))
									{
										m_iSelectedUiType = 1;
										m_pSelectedDesc = DescInfo;
										strcpy_s(m_LayerTag, MAX_PATH, Convert_WStrToStr(DescInfo->szLayer).c_str());
										strcpy_s(m_ComTag, MAX_PATH, Convert_WStrToStr(DescInfo->szProCom).c_str());
										m_iPassNum = DescInfo->iPass;


										fRectSize = DescInfo->fScale;
										m_fMargin = DescInfo->fMargin;
										fSpeed = DescInfo->fSpeed;
										m_iRow = DescInfo->iRow;
										m_iCol = DescInfo->iCol;

									}
									++idx;
								}

							}
						}
						if (bSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndListBox();
				}
			}

			ImGui::Dummy(Spacesize5);

			ImGui::SeparatorText(u8"UI 정보 입력");
			// 객체 추가 제거
			if (ImGui::Button(u8"배치"))
			{
				string szLayer;
				szLayer = Convert_WStrToStr(m_szLyerTag);

				string szCom;
				szCom = Convert_WStrToStr(m_szcomTag);

				_bool bCheck = false;

				/* 컴포넌트 중복 체크 */
				if (0 < m_ComTagMap.size())
				{
					auto Pair = m_ComTagMap.find(szLayer);
					if (Pair != m_ComTagMap.end())
					{
						for (auto iter = Pair->second.begin(); iter != Pair->second.end(); ++iter)
							if (iter->c_str() == szCom)
								bCheck = true;
					}
				}
				if (bCheck)
					MSG_BOX(L"컴포넌트 태그 중복 !");

				else
				{
					/* 저장 */
					_bool bResult = false;

					/* UITYPE : BASIC */
					if (0 == m_iSelectedUiType)
						bResult = PutBasicData(m_szSelectedLayer);

					/* UITYPE : INSTANCING */
					if (1 == m_iSelectedUiType)
					{
						if (m_pAlpha == nullptr)
						{
							bResult = false;
							MSG_BOX(L"박스 안만들었음");
						}
						else
						{
							_int CurIdx = SaveInstanceData(fRectSize, fSpeed, color_rgb, iAlign);
							bResult = PutInstanceData(CurIdx);
						}
					}

					if (bResult)
					{
						auto& Pair = m_ComTagMap.find(szLayer);

						if (Pair == m_ComTagMap.end())
						{
							if (0 == m_ComTagMap.size())
								strcpy_s(m_szSelectedLayer, MAX_PATH, szLayer.c_str());

							vector<string> ComTag;
							ComTag.emplace_back(szCom);
							m_ComTagMap.emplace(szLayer, ComTag);

							/* 추가된 레이어로 활성화 */
							strcpy_s(m_szSelectedLayer, MAX_PATH, szLayer.c_str());
							m_iLayerIndex = (int)m_ComTagMap.size() - 1;

							for (auto ComTagVec : m_ComTagMap)
								Inactive_UI(Convert_StrToWStr(ComTagVec.first));

							Active_UI(Convert_StrToWStr(m_szSelectedLayer));
						}
						else
						{
							Pair->second.emplace_back(szCom);
						}
						strcpy_s(m_szSelectedComTag, MAX_PATH, "");
					}
				}
			}

			ImGui::SameLine();

			if (ImGui::Button(u8"제거"))
			{
				if (0 == m_iSelectedUiType)
					RemoveBasicData(m_szSelectedLayer);

				if (1 == m_iSelectedUiType)
				{
					RemoveInstanceAllData();
				}
			}

			ImGui::SameLine();

			if (ImGui::Button(u8"수정"))
			{
				/* UITYPE : BASIC */
				if (0 == m_iSelectedUiType)
					UpdateBasicData();

				/* UITYPE : INSTANCING */
				if (1 == m_iSelectedUiType)
				{
					wstring szLayer = Convert_StrToWStr(m_szSelectedLayer);
					UpdateInstanceData(szLayer, m_iComIndex, fRectSize, fSpeed, color_rgb, iAlign);
				}

			}

			// 정보 입력
			ImGui::RadioButton(u8"일반", &m_iSelectedUiType, 0); ImGui::SameLine();
			ImGui::RadioButton(u8"인스턴싱", &m_iSelectedUiType, 1);


			// 레이어
			ImGui::Text(u8"UI LayerTag : ");
			ImGui::SameLine();
			ImGui::InputTextWithHint("##LayerTag", "Enter Layer Tag", m_LayerTag, IM_ARRAYSIZE(m_LayerTag));

			lstrcpy(m_szLyerTag, Convert_StrToWStr(m_LayerTag).c_str());

			// 컴포넌트
			ImGui::Text(u8"UI ComTag   : ");
			ImGui::SameLine();
			ImGui::InputTextWithHint("##ComTag", "Enter Component Tag", m_ComTag, IM_ARRAYSIZE(m_ComTag));

			lstrcpy(m_szcomTag, Convert_StrToWStr(m_ComTag).c_str());

			ImGui::Dummy(Spacesize5);

			if(L"" != Convert_StrToWStr(m_szSelectedComTag))
				Set_ImGuizmo();

			ImGui::Text(u8"PASS\t\t   : ");
			ImGui::SameLine();
			ImGui::InputInt("##input int", &m_iPassNum);

			ImGui::Dummy(Spacesize5);

			if (m_iSelectedUiType == 1)
			{
				ImGui::Dummy(Spacesize5);

				ImGui::SeparatorText(u8"인스턴싱 정보 입력");

				ImGui::Text(u8"정렬기준\t : ");
				ImGui::RadioButton(u8"LT", &iAlign, 0); ImGui::SameLine();
				ImGui::RadioButton(u8"RT", &iAlign, 1);

				ImGui::Text(u8"사이즈\t : ");
				ImGui::SameLine();
				ImGui::InputFloat("##fRectSize", &fRectSize);

				ImGui::Text(u8"간격\t : ");
				ImGui::SameLine();
				ImGui::InputFloat("##fMargin", &m_fMargin);

				ImGui::Text(u8"속도\t : ");
				ImGui::SameLine();
				ImGui::InputFloat("##fSpeed", &fSpeed);

				ImGui::Text(u8"행\t : ");
				ImGui::SameLine();
				ImGui::InputInt("##iRow", &m_iRow);

				ImGui::Text(u8"열\t : ");
				ImGui::SameLine();
				ImGui::InputInt("##iCol", &m_iCol);

				ImVec2 button_sz(40, 40);

				// 버튼 효과 미리보기
				ImGui::Text(u8"유효색상 영역 지정");
				ImGui::SameLine();
					
				if(ImGui::Button(u8"All-")) MakeRect(0);
				ImGui::SameLine();
				if(ImGui::Button(u8"All+")) MakeRect(1);

				/* Instance Rect 유효영역 지정 */
				if (m_bActiveArea)
				{
					for (_int col = 0; col < m_iCol; ++col)
					{
						ImGui::BeginGroup();
						for (_int row = 0; row < m_iRow; ++row)
						{
							_uint iIdx = row + m_iRow * col;

							float hue = 0.05f;

							if (m_pAlpha[iIdx])
								ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(hue, 0.6f, 0.6f));
							else
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.f, 0.f, 0.f, 1.0f });

							_char label[128] = "";
							strcpy_s(label, to_string(iIdx + 1).c_str());

							_bool bBtnClick = ImGui::Button(label, button_sz);
							if (bBtnClick == true)
								m_pAlpha[iIdx] = 1 - m_pAlpha[iIdx];

							ImGui::PopStyleColor(1);
							ImGui::SameLine();
						}
						ImGui::EndGroup();
					}
				}

				static _float4 color_hsv(1.f, 0.0f, 0.0f, 1.0f);

				ImGui::Spacing();
				ImGui::Text("HSV encoded colors");
				ImGui::SameLine();
				ImGui::Text("Color widget with InputHSV:");
				ImGui::ColorEdit4("HSV shown as RGB##1", (float*)&color_rgb, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);


				ImGui::ColorConvertHSVtoRGB(color_rgb.x, color_rgb.y, color_rgb.z, color_hsv.x, color_hsv.y, color_hsv.z);
			}

			// 저장 불러오기
			ImGui::SeparatorText(u8"UI 저장 or 불러오기");
			if (ImGui::Button(u8"불러오기")) LoadFile(m_szSelectedLayer);
			ImGui::SameLine();
			if (ImGui::Button(u8"저장")) SaveFile();

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
    }
    ImGui::Separator();
}

void CUI_Tool::OpenFile()
{
	IFileDialog* pfd = nullptr;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

	if (SUCCEEDED(hr))
	{
		DWORD dwOptions;
		pfd->GetOptions(&dwOptions);
		pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);

		hr = pfd->Show(NULL);

		if (SUCCEEDED(hr))
		{
			IShellItem* psi = nullptr;
			hr = pfd->GetResult(&psi);

			if (SUCCEEDED(hr))
			{
				// 선택된 폴더의 경로 가져오기
				PWSTR pszFilePath = L"../../Resources/UI/UI/";
				//PWSTR pszFilePath;
				hr = psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

				if (SUCCEEDED(hr))
				{
					// 폴더 경로를 처리
					MessageBoxW(NULL, pszFilePath, L"Selected Folder", MB_OK);

					Ready_TexVec(pszFilePath);

					for (auto& pTex : m_vecTexture)
						pTex->Get_Textures(&m_vecSRVs);

					CoTaskMemFree(pszFilePath);
				}
				psi->Release();
			}
		}
		pfd->Release();
	}
}

void CUI_Tool::SaveFile()
{
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof OPENFILENAME);

	_tchar szFilePath[MAX_PATH] = L"";
	ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);

    Ofn.lStructSize = sizeof OPENFILENAME;
    Ofn.hwndOwner = g_hWnd;
    Ofn.hInstance = NULL;
    Ofn.lpstrFile = szFilePath;
    Ofn.lpstrFilter = NULL;
    Ofn.nMaxFile = MAX_PATH;
    Ofn.lpstrDefExt = L"dat";
    Ofn.lpstrInitialDir = L"..\\..\\Resources\\UI\\Data\\";
    Ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

	_int iRet = { 0 };
	iRet = GetSaveFileName(&Ofn);

	if (0 == iRet || L"" == Ofn.lpstrFile)
	{
		return;
	}

    _char charFilePath[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, szFilePath, sizeof _tchar * MAX_PATH, charFilePath, MAX_PATH, nullptr, nullptr);

    std::ofstream file(charFilePath, ios::binary);
    if (!file.is_open())
        MSG_BOX(L"File Open Error !");
    else
    {
        for (auto uiDesc : m_pUIvec)
        {
            file.write(reinterpret_cast<const char*>(&uiDesc->eUIType), sizeof(CToolUIObj::UITYPE));
            file.write(reinterpret_cast<const char*>(uiDesc->szProCom), sizeof(_tchar) * MAX_PATH);
            file.write(reinterpret_cast<const char*>(uiDesc->szLayer), sizeof(_tchar) * MAX_PATH);
			file.write(reinterpret_cast<const char*>(&uiDesc->WorldMatrix), sizeof(_float4x4));
            file.write(reinterpret_cast<const char*>(&uiDesc->iPass), sizeof(_int));
            file.write(reinterpret_cast<const char*>(uiDesc->szTexture), sizeof(_tchar) * MAX_PATH);
        }

		for (auto uiInstanceDesc : m_pUIInstanceVec)
		{
			file.write(reinterpret_cast<const char*>(&uiInstanceDesc->eUIType), sizeof(CToolUIObj::UITYPE));
			file.write(reinterpret_cast<const char*>(uiInstanceDesc->szProCom), sizeof(_tchar) * MAX_PATH);
			file.write(reinterpret_cast<const char*>(uiInstanceDesc->szLayer), sizeof(_tchar) * MAX_PATH);
			file.write(reinterpret_cast<const char*>(&uiInstanceDesc->WorldMatrix), sizeof(_float4x4));
			file.write(reinterpret_cast<const char*>(&uiInstanceDesc->iPass), sizeof(_int));
			file.write(reinterpret_cast<const char*>(uiInstanceDesc->szTexture), sizeof(_tchar) * MAX_PATH);

			//file.write(reinterpret_cast<const char*>(&uiInstanceDesc->iAlpha), sizeof(_uint));		// TOOL 에서 보여주기 위한 용도. CLIENT 파싱X
			file.write(reinterpret_cast<const char*>(&uiInstanceDesc->iCol), sizeof(_int));
			file.write(reinterpret_cast<const char*>(&uiInstanceDesc->iRow), sizeof(_int));
			file.write(reinterpret_cast<const char*>(&uiInstanceDesc->fScale), sizeof(_float));
			file.write(reinterpret_cast<const char*>(&uiInstanceDesc->fMargin), sizeof(_float));
			file.write(reinterpret_cast<const char*>(&uiInstanceDesc->fSpeed), sizeof(_float));
			file.write(reinterpret_cast<const char*>(&uiInstanceDesc->vColor), sizeof(_float4));
			file.write(reinterpret_cast<const char*>(&uiInstanceDesc->eUIAlignType), sizeof(CToolUIObj::UIALIGN));

			for (_int col = 0; col < uiInstanceDesc->iCol; ++col)
			{
				for (_int row = 0; row < uiInstanceDesc->iRow; ++row)
				{
					_int idx = row + col * uiInstanceDesc->iRow;
					_int data = uiInstanceDesc->pAlpha[row + col * uiInstanceDesc->iRow];
					file.write(reinterpret_cast<const char*>(&uiInstanceDesc->pAlpha[row + col * uiInstanceDesc->iRow]), sizeof(_uint));
				}
			}
		}
    }

    file.close();

}

void CUI_Tool::LoadFile(char* szPreview)
{
    OPENFILENAME Ofn;
    ZeroMemory(&Ofn, sizeof OPENFILENAME);
    
    _tchar szFilePath[MAX_PATH];
    ZeroMemory(szFilePath, sizeof _tchar * MAX_PATH);
    
    Ofn.lStructSize = sizeof OPENFILENAME;
    Ofn.hwndOwner = g_hWnd;
    Ofn.hInstance = NULL;
    Ofn.lpstrFile = szFilePath;
    Ofn.lpstrFilter = NULL;
    Ofn.nMaxFile = MAX_PATH;
    Ofn.lpstrDefExt = L"dat";
    Ofn.lpstrInitialDir = L"..\\..\\Resources\\UI\\Data\\";
    Ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    
    _int iRet = { 0 };
    iRet = GetSaveFileName(&Ofn);
    
    if (0 == iRet || L"" == Ofn.lpstrFile)
    	return;
    
    _char charFilePath[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, szFilePath, sizeof _tchar * MAX_PATH, charFilePath, MAX_PATH, nullptr, nullptr);
    
    /* 바이너리 읽기 */
    std::ifstream file(charFilePath, std::ios::binary);

	while (file) {

		vector<string> ComTag;
		CToolUIObj::UITYPE eUIType = CToolUIObj::UITYPE::UI_TYPE_END;
		file.read(reinterpret_cast<char*>(&eUIType), sizeof(CToolUIObj::UITYPE));

		if (eUIType == CToolUIObj::UITYPE::UI_BASIC)
		{
			CToolUIObj::UI_DESC* pUIDesc = new CToolUIObj::UI_DESC;

			pUIDesc->eUIType = eUIType;
			file.read(reinterpret_cast<char*>(pUIDesc->szProCom), sizeof(_tchar) * MAX_PATH);
			file.read(reinterpret_cast<char*>(pUIDesc->szLayer), sizeof(_tchar) * MAX_PATH);
			file.read(reinterpret_cast<char*>(&pUIDesc->WorldMatrix), sizeof(_float4x4));
			file.read(reinterpret_cast<char*>(&pUIDesc->iPass), sizeof(_int));
			file.read(reinterpret_cast<char*>(pUIDesc->szTexture), sizeof(_tchar) * MAX_PATH);

			CGameObject* pObj = m_pGameInstance->Add_Clone(LEVEL_UITOOL, pUIDesc->szLayer, L"Prototype_ToolUIObj", pUIDesc);
			if (pObj == nullptr)
				MSG_BOX(L"Clone failed");
			else
			{
				m_pUIvec.push_back(pUIDesc);
				m_vecObjects.emplace_back(dynamic_cast<CToolUIObj*>(pObj));

				/* m_ComTagMap 레이어정보 추가 */
				string szLayer = Convert_WStrToStr(pUIDesc->szLayer);
				strcpy_s(szPreview, MAX_PATH, szLayer.c_str());

				ComTag.emplace_back(Convert_WStrToStr(pUIDesc->szProCom));
				if (m_ComTagMap.find(szLayer) == m_ComTagMap.end())
					m_ComTagMap.emplace(szLayer, ComTag);
				else
					m_ComTagMap.find(szLayer)->second.emplace_back(Convert_WStrToStr(pUIDesc->szProCom));
			}
		}

		if (eUIType == CToolUIObj::UITYPE::UI_INSTANCE)
		{
			CGameObject* pObj = nullptr;
			_bool bFail = false;
			CToolUIObj::UI_INSTANCE_DESC* InstanceUIDesc = new CToolUIObj::UI_INSTANCE_DESC;

			InstanceUIDesc->eUIType = eUIType;
			file.read(reinterpret_cast<char*>(InstanceUIDesc->szProCom), sizeof(_tchar) * MAX_PATH);
			file.read(reinterpret_cast<char*>(InstanceUIDesc->szLayer), sizeof(_tchar) * MAX_PATH);
			file.read(reinterpret_cast<char*>(&InstanceUIDesc->WorldMatrix), sizeof(_float4x4));
			file.read(reinterpret_cast<char*>(&InstanceUIDesc->iPass), sizeof(_int));
			file.read(reinterpret_cast<char*>(InstanceUIDesc->szTexture), sizeof(_tchar) * MAX_PATH);

			file.read(reinterpret_cast<char*>(&InstanceUIDesc->iCol), sizeof(_int));
			file.read(reinterpret_cast<char*>(&InstanceUIDesc->iRow), sizeof(_int));
			file.read(reinterpret_cast<char*>(&InstanceUIDesc->fScale), sizeof(_float));
			file.read(reinterpret_cast<char*>(&InstanceUIDesc->fMargin), sizeof(_float));
			file.read(reinterpret_cast<char*>(&InstanceUIDesc->fSpeed), sizeof(_float));
			file.read(reinterpret_cast<char*>(&InstanceUIDesc->vColor), sizeof(_float4));
			file.read(reinterpret_cast<char*>(&InstanceUIDesc->eUIAlignType), sizeof(CToolUIObj::UIALIGN));

			_uint* piAlpha = new _uint[(InstanceUIDesc->iCol * InstanceUIDesc->iRow)];

			_float4x4 matOrgin = InstanceUIDesc->WorldMatrix;
			memcpy(&matOrgin, &InstanceUIDesc->WorldMatrix, sizeof(_float4x4));

			for (_int col = 0; col < InstanceUIDesc->iCol; ++col)
			{
				for (_int row = 0; row < InstanceUIDesc->iRow; ++row)
				{
					_int idx = row + col * InstanceUIDesc->iRow;
					file.read(reinterpret_cast<char*>(&piAlpha[idx]), sizeof(_uint));

					InstanceUIDesc->iAlpha = piAlpha[idx];
					lstrcpy(InstanceUIDesc->szTexture, L"Prototype_Texture_UI_TestImg");
					lstrcpy(InstanceUIDesc->szLayer, InstanceUIDesc->szLayer);

					_float4x4 matrix = matOrgin;
					matrix._41 += (InstanceUIDesc->fMargin * _float(row));
					matrix._42 += (InstanceUIDesc->fMargin * _float(col) * (-1.f));
					InstanceUIDesc->WorldMatrix = matrix;

					pObj = m_pGameInstance->Add_Clone(LEVEL_UITOOL, InstanceUIDesc->szLayer, L"Prototype_ToolUIObj", InstanceUIDesc);
					if (pObj == nullptr)
						bFail = true;
				}
			}

			InstanceUIDesc->WorldMatrix = matOrgin;
			InstanceUIDesc->pAlpha = piAlpha;

			/* 인스턴싱 영역 렉트 */
			CToolUIObj::UI_INSTANCE_DESC* pRectDesc = new CToolUIObj::UI_INSTANCE_DESC;

			pRectDesc->eUIType = CToolUIObj::UI_BASIC;

			XMMATRIX matrix = XMMatrixIdentity();
			matrix = matrix * InstanceUIDesc->fScale;
			_float4 vPos = { m_vPos.x, m_vPos.y, .1f, 1.f };
			matrix.r[3] = XMLoadFloat4(&vPos);
			XMStoreFloat4x4(&pRectDesc->WorldMatrix, matrix);

			pRectDesc->iPass = m_iPassNum;

			lstrcpy(pRectDesc->szTexture, L"Prototype_Texture_UI_TestImg");
			lstrcpy(pRectDesc->szProCom, m_szcomTag);

			_tcscat_s(pRectDesc->szLayer, InstanceUIDesc->szLayer);
			_tcscat_s(pRectDesc->szLayer, InstanceUIDesc->szProCom);
			_tcscat_s(pRectDesc->szLayer, L"Area");

			if (nullptr == m_pGameInstance->Add_Clone(LEVEL_UITOOL, pRectDesc->szLayer, L"Prototype_ToolUIObj", pRectDesc))
			{
				MSG_BOX(L"Clone failed");
				bFail = true;
			}
			Safe_Delete(pRectDesc);


			/* 인스턴싱, 인스턴싱 렉트 완성되면 ComTagMap 에 넣어주기 */
			if (bFail)
				MSG_BOX(L"error");
			else
			{
				m_pUIInstanceVec.push_back(InstanceUIDesc);
				//m_vecObjects.emplace_back(dynamic_cast<CToolUIObj*>(pObj));

				/* m_ComTagMap 레이어정보 추가 */
				string szLayer = Convert_WStrToStr(InstanceUIDesc->szLayer);
				strcpy_s(szPreview, MAX_PATH, szLayer.c_str());

				ComTag.emplace_back(Convert_WStrToStr(InstanceUIDesc->szProCom));
				if (m_ComTagMap.find(szLayer) == m_ComTagMap.end())
					m_ComTagMap.emplace(szLayer, ComTag);
				else
					m_ComTagMap.find(szLayer)->second.emplace_back(Convert_WStrToStr(InstanceUIDesc->szProCom));
			}
		}
	}

	file.close();
}

void CUI_Tool::Active_UI(const wstring& strLayerTag)
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(GET_CURLEVEL, strLayerTag);
	list<CGameObject*> UIList = pLayer->Get_GameObjects();

	for (auto UIitem : UIList)
	{
		UIitem->Set_Active(true);
	}
}

void CUI_Tool::Inactive_UI(const wstring& strLayerTag)
{
	CLayer* pLayer = m_pGameInstance->Find_Layer(GET_CURLEVEL, strLayerTag);
	list<CGameObject*> UIList = pLayer->Get_GameObjects();

	for (auto UIitem : UIList)
	{
		UIitem->Set_Active(false);
	}
}

HRESULT CUI_Tool::Ready_TexVec(const _tchar* wszPath)
{
	fs::path UIPath(wszPath);
	for (const fs::directory_entry& entry : fs::directory_iterator(UIPath))
	{
		if (entry.is_directory())
		{
			wstring wstrPath(entry.path().c_str());
			wstrPath.append(TEXT("/"));
			if (FAILED(Ready_TexVec(wstrPath.c_str())))
				return E_FAIL;
		}
		else
		{
			fs::path fileName = entry.path().filename();
			fs::path fileTitle = fileName.stem();
			fs::path fullPath = entry.path();
			fs::path ext = fileName.extension();
			if (fileName.extension().compare(".dds"))
				continue;

			m_vecFileName.emplace_back(fileTitle.string());
			m_vecTexture.emplace_back(dynamic_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_UITOOL, fileTitle)));
		}
	}
	return S_OK;
}

_bool CUI_Tool::PutBasicData(_char* _preview)
{
	_bool bResult = false;

	if (m_vecFileName.size() > 0)
	{
		CToolUIObj::UI_DESC* pDesc = nullptr;

		pDesc = new CToolUIObj::UI_DESC;

		pDesc->eUIType = CToolUIObj::UI_BASIC;
		pDesc->WorldMatrix = m_WorldMat;
		pDesc->iPass = m_iPassNum;

		wstring wszTex;
		wszTex = Convert_StrToWStr(m_vecFileName[m_iFileIndex]);

		lstrcpy(pDesc->szTexture, wszTex.c_str());
		lstrcpy(pDesc->szLayer, m_szLyerTag);
		lstrcpy(pDesc->szProCom, m_szcomTag);

		CGameObject* UIObj = m_pGameInstance->Add_Clone(LEVEL_UITOOL, pDesc->szLayer, L"Prototype_ToolUIObj", pDesc);
		m_pUIvec.push_back(pDesc);
		m_vecObjects.emplace_back(dynamic_cast<CToolUIObj*>(UIObj));

		bResult = true;
	}

	return bResult;
}

void CUI_Tool::UpdateBasicData()
{
	if (nullptr == m_pSelectedDesc)
	{
		MSG_BOX(L"다시 선택해주세요");
		return;
	}

	m_pSelectedDesc->eUIType = CToolUIObj::UI_BASIC;
	m_pSelectedDesc->WorldMatrix = m_WorldMat;
	m_pSelectedDesc->iPass = m_iPassNum;

	CLayer* pLayer = m_pGameInstance->Find_Layer(GET_CURLEVEL, m_pSelectedDesc->szLayer);
	CGameObject* pObj = m_pGameInstance->Find_GameObject(GET_CURLEVEL, m_pSelectedDesc->szLayer, (_uint)m_iComIndex);
	if (nullptr == pObj)
		return;

	CTransform* pUITransform = pObj->Get_Transform();
	_fmatrix UI_World = XMLoadFloat4x4(&m_WorldMat);
	pUITransform->Set_WorldMatrix(UI_World);

	for (int i = 0; i < m_pUIvec.size(); ++i)
	{
		if (m_pSelectedDesc->szProCom == m_pUIvec[i]->szProCom)
		{
			m_pUIvec[i]->WorldMatrix = m_pSelectedDesc->WorldMatrix;
			m_pUIvec[i]->iPass = m_pSelectedDesc->iPass;
		}
	}
}

void CUI_Tool::RemoveBasicData(_char* _preview)
{
	if (!m_ComTagMap.empty() && m_vecObjects.size() > 0)
	{
		auto Pair = m_ComTagMap.find(_preview);

		// 그 노드의 Value (ComTag를 모아놓은 벡터)에서 내가 선택한 ComTag 의 주소(iter) 지정(advance이용) 
		auto iter = Pair->second.begin();
		advance(iter, m_iComIndex);

		// 객체 제거
		//if (nullptr != m_vecObjects[m_iComIndex])
		for (_int i = 0; i< m_vecObjects.size(); ++i )
		{
			if (Convert_WStrToStr(m_vecObjects[i]->Get_Desc().szProCom) == m_szSelectedComTag)
			{
				m_vecObjects[i]->Set_Destroy(true);
				m_vecObjects.erase(m_vecObjects.begin() + i);

				// 클라 파싱할 객체 Desc 정보 제거
				m_pUIvec.erase(m_pUIvec.begin() + i);
			}
		}

		// 테그 제거
		if (Pair->second.size() >= m_iComIndex)
		{
			// 그 노드의 Value (ComTag를 모아놓은 벡터)에서 해당 ComTag를 삭제
			Pair->second.erase(iter);

			if (Pair->second.empty())
			{
				m_ComTagMap.erase(Pair);

				if (m_ComTagMap.empty())
					strcpy_s(_preview, MAX_PATH, "");
				else
					strcpy_s(_preview, MAX_PATH, m_ComTagMap.begin()->first.c_str());
			}
		}
	}
}

_bool CUI_Tool::PutInstanceData(_int idx, _bool bModify)
{
	_bool bResult = true;
	m_bToolAlphaUse = true;

	/* 인스턴싱 렉트 */
	for (_int col = 0; col < m_pUIInstanceVec[idx]->iCol; ++col)
	{
		for (_int row = 0; row < m_pUIInstanceVec[idx]->iRow; ++row)
		{
			CToolUIObj::UI_INSTANCE_DESC* pDesc = new CToolUIObj::UI_INSTANCE_DESC;

			pDesc->eUIType = CToolUIObj::UI_INSTANCE;

			XMMATRIX matrix = XMMatrixIdentity();
			matrix = matrix * m_pUIInstanceVec[idx]->fScale;
			_float4 vPos = { m_vPos.x + (m_pUIInstanceVec[idx]->fMargin * _float(row)),
							 m_vPos.y + (m_pUIInstanceVec[idx]->fMargin * _float(col) * (-1.f)),
							 1.f, 1.f };
			matrix.r[3] = XMLoadFloat4(&vPos);
			XMStoreFloat4x4(&pDesc->WorldMatrix, matrix);

			pDesc->iPass = m_iPassNum;
			pDesc->iAlpha = m_pUIInstanceVec[idx]->pAlpha[row + col * m_pUIInstanceVec[idx]->iRow];		// TOOL 에서 보여주기 위한 용도. CLIENT 파싱X
			pDesc->vColor = m_pUIInstanceVec[idx]->vColor;
			pDesc->eUIAlignType = m_pUIInstanceVec[idx]->eUIAlignType;

			lstrcpy(pDesc->szTexture, L"Prototype_Texture_UI_TestImg");
			lstrcpy(pDesc->szLayer, m_pUIInstanceVec[idx]->szLayer);
			lstrcpy(pDesc->szProCom, m_pUIInstanceVec[idx]->szProCom);

			if (nullptr == m_pGameInstance->Add_Clone(LEVEL_UITOOL, pDesc->szLayer, L"Prototype_ToolUIObj", pDesc))
			{
				MSG_BOX(L"Clone failed");
				bResult = false;
			}

			Safe_Delete(pDesc);
		}
	}

	if (bModify)
		return bResult;

	///////////////////////////////////////////////////////////////////////////////// 생각좀
	/* 인스턴싱 영역 렉트 */
	CToolUIObj::UI_INSTANCE_DESC* pRectDesc = new CToolUIObj::UI_INSTANCE_DESC;
	
	pRectDesc->eUIType = CToolUIObj::UI_BASIC;
	
	XMMATRIX matrix = XMMatrixIdentity();
	matrix = matrix * m_pUIInstanceVec[idx]->fScale;
	_float4 vPos = { m_vPos.x, m_vPos.y, 1.f, 1.f };
	matrix.r[3] = XMLoadFloat4(&vPos);
	XMStoreFloat4x4(&pRectDesc->WorldMatrix, matrix);
	
	pRectDesc->iPass = m_iPassNum;
	
	lstrcpy(pRectDesc->szTexture, L"Prototype_Texture_UI_TestImg");
	lstrcpy(pRectDesc->szProCom, m_szcomTag);
	
	_tcscat_s(pRectDesc->szLayer, m_szLyerTag);
	_tcscat_s(pRectDesc->szLayer, pRectDesc->szProCom);
	_tcscat_s(pRectDesc->szLayer, L"Area");
	
	if (nullptr == m_pGameInstance->Add_Clone(LEVEL_UITOOL, pRectDesc->szLayer, L"Prototype_ToolUIObj", pRectDesc))
	{
		MSG_BOX(L"Clone failed");
		bResult = false;
	}
	Safe_Delete(pRectDesc);

	return bResult;
}

/*
* 인스턴스Area 유지
* 인스턴스Rect 삭제 -> 신규생성
*/
void CUI_Tool::UpdateInstanceData(const wstring& strLayerTag, _int iSelectedIdx, _float fRectSize, _float fSpeed, _float4 color_rgb, _int iAlign)
{
	wstring layerTag = Convert_StrToWStr(m_szSelectedLayer);
	wstring comTag = Convert_StrToWStr(m_szSelectedComTag);

	/* 인스턴스Rect 삭제 */
	RemoveInstanceData();

	/* InstanceDescVec 수정 */
	int idx = 0; /* 수정할 m_pUIInstanceVec 인덱스 */ 
	for (int i = 0; i < m_pUIInstanceVec.size(); ++i)
	{
		wstring itemComTag = m_pUIInstanceVec[i]->szProCom;
		if (itemComTag == comTag)
		{
			idx = i;

			//m_pUIInstanceVec[i]->iCol = m_iCol;
			//m_pUIInstanceVec[i]->iRow = m_iRow;
			//m_pUIInstanceVec[i]->pAlpha = m_pAlpha;
			m_iCol = m_pUIInstanceVec[i]->iCol;
			m_iRow = m_pUIInstanceVec[i]->iRow;
			if(m_pAlpha != nullptr)
				m_pUIInstanceVec[i]->pAlpha = m_pAlpha;

			m_pUIInstanceVec[i]->vColor = color_rgb;
			m_pUIInstanceVec[i]->fScale = fRectSize;
			m_pUIInstanceVec[i]->fMargin = m_fMargin;
			m_pUIInstanceVec[i]->fSpeed = fSpeed;
			m_pUIInstanceVec[i]->iPass = m_iPassNum;
			m_pUIInstanceVec[i]->WorldMatrix = m_WorldMat;
			m_pUIInstanceVec[i]->eUIAlignType = (CToolUIObj::UIALIGN)iAlign;
		}
	}

	/* 인스턴스Rect 신규생성 */
	PutInstanceData(idx, true);
}

_int CUI_Tool::SaveInstanceData(_float fScale, _float fSpeed, _float4 color_rgb, _int iAlign)
{
	_int curIdx = 0;

	CToolUIObj::UI_INSTANCE_DESC* tUIitem = new CToolUIObj::UI_INSTANCE_DESC;
	tUIitem->eUIType = CToolUIObj::UI_INSTANCE;

	lstrcpy(tUIitem->szTexture, L"");
	lstrcpy(tUIitem->szLayer, m_szLyerTag);
	lstrcpy(tUIitem->szProCom, m_szcomTag);

	tUIitem->iPass = m_iPassNum;

	tUIitem->iCol = m_iCol;
	tUIitem->iRow = m_iRow;
	tUIitem->pAlpha = m_pAlpha;
	tUIitem->vColor = color_rgb;
	tUIitem->fScale = fScale;
	tUIitem->fSpeed = fSpeed;
	tUIitem->fMargin = m_fMargin;
	tUIitem->WorldMatrix = m_WorldMat;
	tUIitem->eUIAlignType = (CToolUIObj::UIALIGN)iAlign;

	m_pUIInstanceVec.push_back(tUIitem);

	return (int)m_pUIInstanceVec.size() -1;
}

void CUI_Tool::RemoveInstanceData()
{
	/* 클론 Obj 삭제 */
	wstring layerTag = Convert_StrToWStr(m_szSelectedLayer);
	wstring comTag = Convert_StrToWStr(m_szSelectedComTag);

	CLayer* pLayer = m_pGameInstance->Find_Layer(GET_CURLEVEL, layerTag);
	list<CGameObject*> UIList = pLayer->Get_GameObjects();

	for (auto UIitem : UIList)
	{
		CToolUIObj::UI_DESC desc = dynamic_cast<CToolUIObj*>(UIitem)->Get_Desc();

		string componentTag = Convert_WStrToStr(desc.szProCom);

		if (componentTag == m_szSelectedComTag)
		{
			UIitem->Set_Destroy(true);
		}
	}
}

void CUI_Tool::RemoveInstanceAllData()
{
	/* 클론 Obj 삭제 */
	wstring layerTag = Convert_StrToWStr(m_szSelectedLayer);
	wstring comTag = Convert_StrToWStr(m_szSelectedComTag);

	if (comTag == L"")
	{
		MSG_BOX(L"다시 선택해 주세요");
		return;
	}

	m_pGameInstance->Clear_Layer(GET_CURLEVEL, layerTag + comTag + L"Area");

	/* 인스턴스 데이터 삭제 */
	RemoveInstanceData();

	/* 테그 제거 */
	auto Pair = m_ComTagMap.find(m_szSelectedLayer);
	auto iter = Pair->second.begin();
	if (Pair->second.size() >= m_iComIndex)
	{
		// 그 노드의 Value (ComTag를 모아놓은 벡터)에서 해당 ComTag를 삭제
		Pair->second.erase(iter);

		if (Pair->second.empty())
		{
			m_ComTagMap.erase(Pair);

			if (m_ComTagMap.empty())
				strcpy_s(m_szSelectedLayer, MAX_PATH, "");
			else
				strcpy_s(m_szSelectedLayer, MAX_PATH, m_ComTagMap.begin()->first.c_str());
		}
	}

	/* Desc 삭제 */
	for (auto& iter = m_pUIInstanceVec.begin();
		iter != m_pUIInstanceVec.end();
		++iter)
	{
		wstring itemComTag = (*iter)->szProCom;
		if (itemComTag == comTag)
		{
			Safe_Delete(*iter);
			m_pUIInstanceVec.erase(iter);
			return;
		}
	}
}

void CUI_Tool::MakeRect(_int i)
{
	m_bActiveArea = true;

	if (!m_bToolAlphaUse)
	{
		Safe_Delete(m_pAlpha);
		m_bToolAlphaUse = false;
	}

	m_pAlpha = new _uint[m_iCol * m_iRow];

	for (_int col = 0; col < (m_iCol * m_iRow); ++col)
		m_pAlpha[col] = i;
}

CGameObject* CUI_Tool::GetSelectedObj(_bool bInstance)
{
	wstring layerTag = Convert_StrToWStr(m_szSelectedLayer);
	wstring comTag = Convert_StrToWStr(m_szSelectedComTag);

	/* 인스턴스: Area 영역 조회 */
	if (bInstance)
	{
		string sLayerTag = m_szSelectedLayer;
		sLayerTag += Convert_WStrToStr(comTag) + "Area";
		layerTag = Convert_StrToWStr(sLayerTag);
	}

	CLayer* pLayer = m_pGameInstance->Find_Layer(GET_CURLEVEL, layerTag);
	if (pLayer == nullptr)
		return nullptr;

	for (auto UIitem : pLayer->Get_GameObjects())
	{
		CToolUIObj::UI_DESC desc = dynamic_cast<CToolUIObj*>(UIitem)->Get_Desc();

		string componentTag = Convert_WStrToStr(desc.szProCom);
		if (componentTag == Convert_WStrToStr(comTag))
			return UIitem;
	}

	return nullptr;
}

CUI_Tool* CUI_Tool::Create()
{
	CUI_Tool* pInstance = new CUI_Tool();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Tool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Delete(m_pAlpha);

	for (auto desc : m_pUIvec)
		Safe_Delete(desc);

	for (auto desc : m_pUIInstanceVec)
	{
		Safe_Delete(desc->pAlpha);
		Safe_Delete(desc);
	}
}
