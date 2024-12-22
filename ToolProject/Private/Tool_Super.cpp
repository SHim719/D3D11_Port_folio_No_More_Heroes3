#include "Tool_Super.h"

CTool_Super::CTool_Super()
    : m_pGameInstance(CGameInstance::Get_Instance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CTool_Super::Initialize()
{
    return S_OK;
}

void CTool_Super::Start_Tool()
{
    m_pCamera->Get_Transform()->Set_Position(XMVectorSet(0.f, 2.f, -1.f, 1.f));
}

void CTool_Super::Tick(_float fTimeDelta)
{
}

void CTool_Super::LateTick(_float fTimeDelta)
{
}

void CTool_Super::Camera_Window()
{
    ImGui::Begin("Camera_State", (bool*)0);
    ImGui::SetCursorPos(ImVec2(23, 36));
    ImGui::Text("Camera Position");

    ImGui::SetCursorPos(ImVec2(16, 57));
    ImGui::PushItemWidth(200);

    _float4 vCamPos;
    XMStoreFloat4(&vCamPos, m_pCamera->Get_Transform()->Get_Position());
    if (ImGui::InputFloat3("##CamPos", &(vCamPos.x)))
        m_pCamera->Get_Transform()->Set_Position(XMLoadFloat4(&vCamPos));
    ImGui::PopItemWidth();


    ImGui::SetCursorPos(ImVec2(25, 90));
    ImGui::Text("FovY");

    ImGui::SetCursorPos(ImVec2(18, 105));
    ImGui::PushItemWidth(183);

    _float fFovY = m_pCamera->Get_FovY();
    if (ImGui::InputFloat("##Fov", &fFovY, 0.01f, 179.f, "%.3f"))
        m_pCamera->Set_FovY(fFovY);

    ImGui::PopItemWidth();

}


void CTool_Super::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pCamera);
}
