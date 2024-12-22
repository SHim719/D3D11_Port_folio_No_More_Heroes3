#include "ImGui_Main.h"
#include "GameInstance.h"

#include "Anim_Tool.h"
#include "Map_Tool.h"
#include "UI_Tool.h"
#include "Effect_Tool.h"
#include "Cinema_Tool.h"

CImGui_Main::CImGui_Main()
{
}

HRESULT CImGui_Main::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ToolType eToolType)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    RECT rect = {};
    GetClientRect(g_hWnd, &rect);
    long Width = rect.right - rect.left;
    long Height = rect.bottom - rect.top;

    io.DisplaySize = ImVec2(float(Width), float(Height));

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

    switch (eToolType)
    {
    case MAP:
        m_pNowTool = CMap_Tool::Create(pDevice, pContext);
        break;
    case ANIM:
        m_pNowTool = CAnim_Tool::Create();
        break;
    case UI:
        m_pNowTool = CUI_Tool::Create();
        break;
    case EFFECT: 
        m_pNowTool = CEffect_Tool::Create();
        break;
    case CINEMATIC:
        m_pNowTool = CCinema_Tool::Create(pDevice, pContext);
        break;
    }
    m_pNowTool->Start_Tool();

    return S_OK;
}

void CImGui_Main::Tick(_float fTimeDelta)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    m_pNowTool->Tick(fTimeDelta);
}

void CImGui_Main::LateTick(_float fTimeDelta)
{
    m_pNowTool->LateTick(fTimeDelta);
}

void CImGui_Main::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


CImGui_Main* CImGui_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ToolType eToolType)
{
    CImGui_Main* pInstance = new CImGui_Main();

    if (FAILED(pInstance->Initialize(pDevice, pContext, eToolType)))
    {
        MSG_BOX(TEXT("Failed To Created : CImGui_Main"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CImGui_Main::Free()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    Safe_Release(m_pNowTool);
}
