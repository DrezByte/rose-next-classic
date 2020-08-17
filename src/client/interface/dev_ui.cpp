#include "stdafx.h"

#include "dev_ui.h"

#include "ccamera.h"
#include "system/cgame.h"
#include "system/cgamestate.h"
#include "jcommandstate.h"

#include "fmt/format.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "zz_hash_table.h"
#include "zz_node.h"
#include "zz_system.h"
#include "zz_visible.h"

static bool target_window_open = false;

#ifdef _DEBUG
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam);

void
draw_camera_info() {
    CCamera* cam = CCamera::Instance();
    if (!cam) {
        return;
    }

    float distance = cam->distance();
    float pitch = cam->pitch();
    float yaw = cam->yaw();

    ImGui::Checkbox("Follow mode", &cam->m_bFollowMode);
    ImGui::SliderFloat("Distance", &distance, 0.0f, Rose::GameStaticConfig::CAMERA_MAX_ZOOM * 10);
    ImGui::SliderFloat("Pitch", &pitch, 0.0f, 1.0f);
    ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f);

    cam->set_follow_mode(cam->m_bFollowMode);
    cam->set_distance(distance);
    cam->set_pitch(pitch);
    cam->set_yaw(yaw);
}

void
draw_target_window() {
    if (!target_window_open) {
        return;
    }

    ImGui::Begin("Developer - Target", &target_window_open, ImGuiWindowFlags_AlwaysAutoResize);

    int target_id = g_UserInputSystem.GetCurrentTarget();
    if (target_id < 0 || target_id > MAX_CLIENT_OBJECTS) {
        ImGui::Text("Target ID: Invalid");
        ImGui::End();
        return;
    }

    ImGui::Text(fmt::format("ID: {}", target_id).c_str());
    if (target_id == 0) {
        ImGui::End();
        return;
    }

    CGameOBJ* obj = g_pObjMGR->m_pOBJECTS[target_id];
    if (!obj) {
        ImGui::Text("Object with this id does not exist.");
        ImGui::End();
        return;
    }

    int obj_type_id = obj->Get_TYPE();
    std::string obj_type_str = obj_type_string(static_cast<t_ObjTAG>(obj_type_id));
    ImGui::SameLine();
    ImGui::Text(fmt::format("Type: {}({})", obj_type_str, obj_type_id).c_str());
    ImGui::Separator();

    switch (obj->Get_TYPE()) {
        case OBJ_USER:
        case OBJ_AVATAR:
        case OBJ_NPC:
        case OBJ_MOB: {
            CObjCHAR* character = reinterpret_cast<CObjCHAR*>(obj);
            int str = 0;
            int dex = 0;
            int intt = 0;
            int con = 0;
            int cha = character->Get_CHARM();
            int sen = character->Get_SENSE();

            if (character->IsUSER()) {
                CObjAVT* avatar = reinterpret_cast<CObjAVT*>(obj);
                intt = avatar->Get_INT();
                con = avatar->Get_CON();
            }

            if (character->Get_TYPE() == OBJ_USER) {
                CObjUSER* user = reinterpret_cast<CObjUSER*>(obj);
                str = user->Get_STR();
                dex = user->Get_DEX();
            }

            if (character) {
                ImGui::Text("General");
                ImGui::Separator();
                ImGui::Text("Name: %s", character->Get_NAME());
                ImGui::Text("Level: %d", character->Get_LEVEL());
                ImGui::Text("HP: %d/%d", character->Get_HP(), character->Get_MaxHP());
                ImGui::SameLine();
                ImGui::Text("MP: %d/%d", character->Get_MP(), character->Get_MaxMP());
                ImGui::Text("Position: (%.3f, %.3f, %.3f)",
                    character->Get_CurXPOS(),
                    character->Get_CurYPOS(),
                    character->Get_CurZPOS());
                ImGui::Separator();

                ImGui::Text("Stats");
                ImGui::Separator();
                ImGui::Columns(2);
                ImGui::Text("Str: %d", str);
                ImGui::Text("Dex: %d", dex);
                ImGui::Text("Int: %d", intt);
                ImGui::Text("Con: %d", con);
                ImGui::Text("Cha: %d", cha);
                ImGui::Text("Sen: %d", sen);
                ImGui::NextColumn();
                ImGui::Text("Attack: %d", character->stats.attack_power);
                ImGui::Text("Def: %d", character->Get_DEF());
                ImGui::Text("M-Res: %d", character->Get_RES());
                ImGui::Text("Hit: %d", character->stats.hit_rate);
                ImGui::Text("Crit: %d", character->Get_CRITICAL());
                ImGui::Text("Dodge: %d", character->Get_AVOID());
                ImGui::Text("A-spd: %d", character->stats.attack_speed);
                ImGui::Text("M-spd: %d", character->stats.move_speed);
                ImGui::Columns(1);
                ImGui::Separator();

                ImGui::Text("Animation");
                ImGui::Separator();
                tagMOTION* anim = character->m_pCurMOTION;
                ImGui::Text("Motion id: %d", anim->m_nActionIdx);
                ImGui::Text("Model speed: %.3f", character->Get_ModelSPEED());
                ImGui::Text("Move speed: %.3f", character->Get_MoveAniSPEED());
                ImGui::Text("Attack speed: %.3f", character->Get_fAttackSPEED());
                ImGui::Separator();
            }
        }
    }

    switch (obj->Get_TYPE()) {
        case OBJ_USER: {
            ImGui::Text("THAS YOU");
            break;
        }
        case OBJ_AVATAR: {
            ImGui::Text("THAS AN AVATAR");
            break;
        }
        case OBJ_NPC: {
            ImGui::Text("THAS AN NPC");
            break;
        }
        case OBJ_MOB: {
            ImGui::Text("THAS A MOB");
            break;
        }
        default: {
            ImGui::Text("Currently no information available for this type.");
        }
    }

    ImGui::End();
}

void
dev_ui_init(HWND handle) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(handle);
    ImGui_ImplDX9_Init(reinterpret_cast<IDirect3DDevice9*>(::getDevice()));
}

void
dev_ui_frame() {
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    zz_system* zz = reinterpret_cast<zz_system*>(::getZnzin());

    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    ImGui::Begin("Developer",
        &CGame::GetInstance().active_state->dev_ui_enabled,
        ImGuiWindowFlags_AlwaysAutoResize);

    bool wireframe = ::getUseWireMode();
    bool observer_camera = ::GetObserverCameraOnOff();

    // Fog
    bool use_fog = ::getUseFog();
    float fog_color[3] = {zz->get_rs()->fog_color[0],
        zz->get_rs()->fog_color[1],
        zz->get_rs()->fog_color[2]};
    float fog[2] = {zz->get_rs()->fog_start, zz->get_rs()->fog_end};
    float alpha_fog[2] = {zz->get_rs()->alpha_fog_start, zz->get_rs()->alpha_fog_end};

    if (ImGui::Button("Target Window")) {
        target_window_open = true;
    }

    if (ImGui::CollapsingHeader("General")) {
        ImGui::Checkbox("Disable UI", &g_GameDATA.m_bNoUI);
        ImGui::SameLine();
        ImGui::Checkbox("Filming mode", &g_GameDATA.m_bFilmingMode);
        ImGui::SameLine();
        ImGui::Checkbox("Observer mode", &g_GameDATA.m_bObserverCameraMode);

        if (ImGui::Button("Reload UI")) {
            g_itMGR.reload_dialogs();
        }
    }

    if (ImGui::CollapsingHeader("Camera")) {
        draw_camera_info();
    }

    if (ImGui::CollapsingHeader("Engine settings")) {
        ImGui::Checkbox("Wireframe", &wireframe);
        ImGui::Checkbox("Use Fog", &use_fog);
        ImGui::ColorEdit3("Fog color", fog_color);
        ImGui::SliderFloat2("Fog range", fog, 0.0f, 1000.0f);
        ImGui::SliderFloat2("Alpha fog range", alpha_fog, 0.0f, 1000.0f);
    }

    if (ImGui::CollapsingHeader("Scene Tree")) {
        if (ImGui::TreeNode("Visibles")) {
            zz_visible* visible;

            zz_hash_table<zz_node*>::iterator it;
            zz_hash_table<zz_node*>* nodes = zz->visibles->get_hash_table();

            for (it = nodes->begin(); it != nodes->end(); it++) {
                visible = (zz_visible*)(*it);
                if (visible->is_visible()) {
                    ImGui::Text(visible->get_name());
                }
            }
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Class Information")) {
        ImGui::Text("111 Soldier");
        ImGui::Text("121 Knight");
        ImGui::Text("122 Champion");
        ImGui::Text("");
        ImGui::Text("211 Muse");
        ImGui::Text("221 Magician");
        ImGui::Text("222 Cleric");
        ImGui::Text("");
        ImGui::Text("311 Hawker");
        ImGui::Text("311 Scout");
        ImGui::Text("321 Raider");
        ImGui::Text("");
        ImGui::Text("411 Dealer");
        ImGui::Text("421 Artisan");
        ImGui::Text("422 Bourgeois");
    }

    if (ImGui::CollapsingHeader("Item Information")) {
        ImGui::Text("1 Mask");
        ImGui::Text("2 Hat");
        ImGui::Text("3 Armor");
        ImGui::Text("4 Gloves");
        ImGui::Text("5 Boots");
        ImGui::Text("6 Back Items");
        ImGui::Text("7 Accessories");
        ImGui::Text("8 Weapons");
        ImGui::Text("9 Sub-Weapons");
        ImGui::Text("10 Consumables");
        ImGui::Text("11 Gems");
        ImGui::Text("12 Materials");
        ImGui::Text("13 Quest Item");
        ImGui::Text("14 Vehicles");
    }
    ImGui::End();

    draw_target_window();

    ImGui::EndFrame();

    ::useWireMode(wireframe);
    ::UserObserverCamera(observer_camera);
    ::useFog(use_fog);
    ::setFogColor(fog_color[0], fog_color[1], fog_color[2]);
    ::setFogRange(fog[0] * ZZ_SCALE_OUT, fog[1] * ZZ_SCALE_OUT);
    ::setAlphaFogRange(alpha_fog[0] * ZZ_SCALE_OUT, alpha_fog[1] * ZZ_SCALE_OUT);
}

void
dev_ui_render() {
    IDirect3DDevice9* device = reinterpret_cast<IDirect3DDevice9*>(::getDevice());

    DWORD prev_fill_mode = D3DFILL_SOLID;
    device->GetRenderState(D3DRS_FILLMODE, &prev_fill_mode);

    // Never render dev UI as wireframe
    device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    device->SetRenderState(D3DRS_FILLMODE, prev_fill_mode);
}

void
dev_ui_destroy() {
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

bool
dev_ui_proc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (!ImGui::GetCurrentContext()) {
        return false;
    }

    if (ImGui_ImplWin32_WndProcHandler(handle, msg, wParam, lParam)) {
        return false;
    }

    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

#else
void
dev_ui_init(HWND handle) {}

void
dev_ui_frame() {}

void
dev_ui_render() {}

void
dev_ui_destroy() {}

bool
dev_ui_proc(HWND handle, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
    return false;
}
#endif