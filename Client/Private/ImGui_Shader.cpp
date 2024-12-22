#include "ImGui_Shader.h"
#include "GameInstance.h"

CImGui_Shader::CImGui_Shader() 
    : m_pGameInstance(CGameInstance::Get_Instance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CImGui_Shader::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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

    return S_OK;
}

void CImGui_Shader::Shader_Window()
{
    ImGui::Begin("ShaderOption", (bool*)0);
    
    if (ImGui::Checkbox("SSAO", &m_bSSAO))
        m_pGameInstance->Set_Active_SSAO(m_bSSAO);
    if (ImGui::DragFloat("SSAO Power", &m_fSSAOPower))
        m_pGameInstance->Set_SSAOPower(m_fSSAOPower);

    if (ImGui::Checkbox("SSR", &m_bSSR))
        m_pGameInstance->Set_Active_SSR(m_bSSR);

    if (ImGui::Checkbox("DOF", &m_bDOF))
        m_pGameInstance->Set_Active_DOF(m_bDOF);

    if (ImGui::DragFloat("DOF DistMin", &m_fDOFDistMin, 1.f, 0.f, 1000.f))
        m_pGameInstance->Set_DOF_DistMin(m_fDOFDistMin);
            
    if (ImGui::DragFloat("DOF DistMax", &m_fDOFDistMax, 1.f, 0.f, 1000.f))
        m_pGameInstance->Set_DOF_DistMax(m_fDOFDistMax);

    if (ImGui::DragFloat("Bloom Strength", &m_fBloomStrength, 0.1f, 0.f, 1000.f))
        m_pGameInstance->Set_BloomStrength(m_fBloomStrength);

    if (ImGui::DragFloat("Bloom Threshold", &m_fBloomThreshold, 0.1f, 0.f, 1000.f))
        m_pGameInstance->Set_BloomThreshold(m_fBloomThreshold);

    ImGui::End();
}


void CImGui_Shader::Tick(_float fTimeDelta)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    Shader_Window();


    
}

void CImGui_Shader::LateTick(_float fTimeDelta)
{
}

void CImGui_Shader::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

CImGui_Shader* CImGui_Shader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CImGui_Shader* pInstance = new CImGui_Shader();

    if (FAILED(pInstance->Initialize(pDevice, pContext)))
    {
        MSG_BOX(TEXT("Failed To Created : CImGui_Shader"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CImGui_Shader::Free()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    Safe_Release(m_pGameInstance);
}
