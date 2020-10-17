#include "stdafx.h"

#include "dev_ui.h"

#include "ccamera.h"
#include "network/cnetwork.h"
#include "interface/ctdrawimpl.h"
#include "interface/io_imageres.h"
#include "rose/io/stb.h"
#include "system/cgame.h"
#include "system/cgamestate.h"
#include "jcommandstate.h"

#include "fmt/format.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "zz_hash_table.h"
#include "zz_node.h"
#include "zz_texture.h"
#include "zz_visible.h"

#ifdef _DEBUG

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam);

static std::vector<std::tuple<size_t, std::string>> stat_names;
static bool game_window_open = false;
static bool target_window_open = false;
static unsigned int selected_item_type = ITEM_TYPE_FACE_ITEM;

static const std::array<std::tuple<unsigned int, STBDATA*>, 14> item_data = {
    std::make_tuple(ITEM_TYPE_FACE_ITEM, &g_TblFACEITEM),
    std::make_tuple(ITEM_TYPE_HELMET, &g_TblHELMET),
    std::make_tuple(ITEM_TYPE_ARMOR, &g_TblARMOR),
    std::make_tuple(ITEM_TYPE_GAUNTLET, &g_TblGAUNTLET),
    std::make_tuple(ITEM_TYPE_BOOTS, &g_TblBOOTS),
    std::make_tuple(ITEM_TYPE_KNAPSACK, &g_TblBACKITEM),
    std::make_tuple(ITEM_TYPE_JEWEL, &g_TblJEWELITEM),
    std::make_tuple(ITEM_TYPE_WEAPON, &g_TblWEAPON),
    std::make_tuple(ITEM_TYPE_SUBWPN, &g_TblSUBWPN),
    std::make_tuple(ITEM_TYPE_USE, &g_TblUSEITEM),
    std::make_tuple(ITEM_TYPE_GEM, &g_TblGEMITEM),
    std::make_tuple(ITEM_TYPE_NATURAL, &g_TblNATUAL),
    std::make_tuple(ITEM_TYPE_QUEST, &g_TblQUESTITEM),
    std::make_tuple(ITEM_TYPE_RIDE_PART, &g_PatITEM),
};

constexpr const char*
item_type_label(unsigned int type) {
    switch (type) {
        case ITEM_TYPE_FACE_ITEM:
            return "Mask";
        case ITEM_TYPE_HELMET:
            return "Hat";
        case ITEM_TYPE_ARMOR:
            return "Chest";
        case ITEM_TYPE_GAUNTLET:
            return "Gloves";
        case ITEM_TYPE_BOOTS:
            return "Shoes";
        case ITEM_TYPE_KNAPSACK:
            return "Back";
        case ITEM_TYPE_JEWEL:
            return "Accessory";
        case ITEM_TYPE_WEAPON:
            return "Weapon";
        case ITEM_TYPE_SUBWPN:
            return "Subweapon";
        case ITEM_TYPE_USE:
            return "Consumable";
        case ITEM_TYPE_GEM:
            return "Gem";
        case ITEM_TYPE_NATURAL:
            return "ETC";
        case ITEM_TYPE_QUEST:
            return "Quest";
        case ITEM_TYPE_RIDE_PART:
            return "Cart";
        default:
            return "";
    }
}

void
load_stat_data() {
    for (size_t row_idx = 0; row_idx < g_TblGEMITEM.row_count; ++row_idx) {
        std::string label;
        // Append gem name
        if (row_idx > 300) {
            label += ITEM_NAME(ITEM_TYPE_GEM, row_idx);
        } else {
            // Append stat info
            for (int j = 0; j < 2; j++) {
                short nAttribute = GEMITEM_ADD_DATA_TYPE(row_idx, j);
                if (nAttribute != 0) {
                    if (j == 1) {
                        label += " / ";
                    }
                    std::string attr = CStringManager::GetSingleton().GetAbility(nAttribute);
                    int amount = GEMITEM_ADD_DATA_VALUE(row_idx, j);
                    if (nAttribute == AT_SAVE_MP) {
                        label += fmt::format("{} {}%", attr, amount);
                    } else {
                        label += fmt::format("{} {}", attr, amount);
                    }
                }
            }
        }

        if (!label.empty()) {
            stat_names.push_back({row_idx, label});
        }
    }
}

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
draw_item_info() {
    CImageRes* item_image_manager = CImageResManager::GetSingleton().GetImageRes(IMAGE_RES_ITEM);
    if (!item_image_manager) {
        return;
    }

    static char filter_item_name[128] = "";

    static int spawn_item_quantity = 1;
    static int spawn_item_refine = 0;
    static int spawn_item_durability = 120;
    static int spawn_item_lifespan = 100;
    static int spawn_item_stat_id = 0;
    static bool spawn_item_socket = true;

    {
        if (!ImGui::BeginChild("item_window_left_pane", ImVec2(150, 0), true)) {
            return ImGui::EndChild();
        }

        for (auto [item_type, _]: item_data) {
            if (ImGui::Selectable(item_type_label(item_type), selected_item_type == item_type)) {
                selected_item_type = item_type;
            }
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    {
        if (!ImGui::BeginChild("item_window_details", ImVec2(225, 0), true)) {
            return ImGui::EndChild();
        }
        ImGui::Text("Filters");
        ImGui::Separator();
        ImGui::InputText("Name##filter_item", filter_item_name, IM_ARRAYSIZE(filter_item_name));

        ImGui::Text("Spawn");
        ImGui::Separator();
        ImGui::SliderInt("Quantity##spawn_item", &spawn_item_quantity, 1, 999);
        ImGui::SliderInt("Refine##spawn_item", &spawn_item_refine, 0, 9);
        ImGui::SliderInt("Durability##spawn_item", &spawn_item_durability, 0, 120);
        ImGui::SliderInt("Lifespan##spawn_item", &spawn_item_lifespan, 0, 100, "%d%%");
        static std::string combo_label = "";
        if (ImGui::BeginCombo("Stat/Gem", combo_label.c_str())) {
            for (const auto [id, label]: stat_names) {
                const bool is_selected = (spawn_item_stat_id == id);
                if (ImGui::Selectable(label.c_str(), is_selected)) {
                    spawn_item_stat_id = id;
                    combo_label = label;
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::Checkbox("Socket##spawn_item", &spawn_item_socket);
        ImGui::EndChild();
    }
    ImGui::SameLine();
    {
        if (!ImGui::BeginChild("item_window_right_pane",
                ImVec2(0, 0),
                true,
                ImGuiWindowFlags_HorizontalScrollbar)) {
            return ImGui::EndChild();
        }

        for (auto [item_type, stb]: item_data) {
            if (item_type != selected_item_type) {
                continue;
            }

            for (short item_id = 0; item_id < stb->row_count; ++item_id) {
                std::string item_name(ITEM_NAME(item_type, item_id));
                if (item_name.empty()) {
                    continue;
                }

                if (filter_item_name) {
                    std::string name(item_name);
                    std::string filter(filter_item_name);
                    auto to_lower = [](unsigned char c) { return std::tolower(c); };
                    std::transform(name.begin(), name.end(), name.begin(), to_lower);
                    std::transform(filter.begin(), filter.end(), filter.begin(), to_lower);
                    if (name.find(filter) == std::string::npos) {
                        continue;
                    }
                }

                const int icon_id = ITEM_ICON_NO(item_type, item_id);
                if (!icon_id) {
                    continue;
                }

                stTexture* texture_data = item_image_manager->GetTexture(icon_id);
                stSprite* sprite_data = item_image_manager->GetSprite(icon_id);
                if (!texture_data || !sprite_data) {
                    continue;
                }

                zz_texture* tex = reinterpret_cast<zz_texture*>(texture_data->m_Texture);
                if (!tex) {
                    continue;
                }
                HNODE tex_d3d_id = ::getTexturePointer(texture_data->m_Texture);
                if (!tex_d3d_id) {
                    continue;
                }
                float x1 = static_cast<float>(sprite_data->m_Rect.left) / tex->get_width();
                float y1 = static_cast<float>(sprite_data->m_Rect.top) / tex->get_width();
                float x2 = static_cast<float>(sprite_data->m_Rect.right) / tex->get_width();
                float y2 = static_cast<float>(sprite_data->m_Rect.bottom) / tex->get_width();

                ImTextureID im_tex_id = reinterpret_cast<ImTextureID>(tex_d3d_id);
                ImVec2 size(40, 40);
                ImVec2 uv1(x1, y1);
                ImVec2 uv2(x2, y2);

                std::string button_label = fmt::format("Spawn##{}_{}", item_type, item_id);
                if (ImGui::Button(button_label.c_str())) {
                    std::string spawn_cmd;
                    if (tagBaseITEM::is_stackable(item_type)) {
                        spawn_cmd =
                            fmt::format("/item {} {} {}", item_type, item_id, spawn_item_quantity);
                    } else {
                        spawn_cmd = fmt::format("/item {} {} {} {}",
                            item_type,
                            item_id,
                            spawn_item_stat_id,
                            spawn_item_socket ? 1 : 0);
                    }
                    g_pNet->Send_cli_CHAT(const_cast<char*>(spawn_cmd.c_str()));
                }
                ImGui::SameLine(65.0f);
                ImGui::Text("%d:%d", item_type, item_id);
                ImGui::SameLine(115.0f);
                ImGui::Image(im_tex_id, size, uv1, uv2);
                ImGui::SameLine(165.0f);
                ImGui::Text("%s", item_name.c_str());
            }
        }
        ImGui::EndChild();
    }
}

void
draw_skill_info() {
    CImageRes* skill_image_manager =
        CImageResManager::GetSingleton().GetImageRes(IMAGE_RES_SKILL_ICON);
    if (!skill_image_manager) {
        return;
    }

    static int selected_skill_job = 0;
    std::vector<std::tuple<int, const char*>> skill_job_data = {
        {0, "General"},
        {41, "Soldier"},
        {42, "Muse"},
        {43, "Hawker"},
        {44, "Dealer"},
    };

    {
        if (!ImGui::BeginChild("skill_window_left_pane", ImVec2(150, 0), true)) {
            return ImGui::EndChild();
        }

        for (auto const& [job_id, job_name]: skill_job_data) {
            if (ImGui::Selectable(job_name, selected_skill_job == job_id)) {
                selected_skill_job = job_id;
            }
        }

        ImGui::EndChild();
    }
    ImGui::SameLine();
    {
        if (!ImGui::BeginChild("skill_window_right_pane", ImVec2(0, 0), true)) {
            return ImGui::EndChild();
        }

        const STBDATA& stb = g_SkillList.m_SkillDATA;
        for (size_t skill_id = 0; skill_id < stb.row_count; ++skill_id) {
            const std::string skill_name(SKILL_NAME(skill_id));
            if (skill_name.empty()) {
                continue;
            }

            // Only include first level of skills or individual skills
            const int skill_core_id(SKILL_1LEV_INDEX(skill_id));
            if (skill_core_id != skill_id) {
                continue;
            }

            const int skill_icon(SKILL_ICON_NO(skill_id));
            if (!skill_icon) {
                continue;
            }

            int skill_job(SKILL_AVAILBLE_CLASS_SET(skill_id));
            switch (skill_job) {
                // Soldier
                case 61:
                case 62:
                    skill_job = 41;
                    break;
                // Muse
                case 63:
                case 64:
                    skill_job = 42;
                    break;
                case 65:
                case 66:
                    skill_job = 43;
                    break;
                case 67:
                case 68:
                    skill_job = 44;
                    break;
            }

            if (skill_job != selected_skill_job) {
                continue;
            }

            stTexture* texture_data = skill_image_manager->GetTexture(skill_icon);
            stSprite* sprite_data = skill_image_manager->GetSprite(skill_icon);
            if (!texture_data || !sprite_data) {
                continue;
            }

            zz_texture* tex = reinterpret_cast<zz_texture*>(texture_data->m_Texture);
            if (!tex) {
                continue;
            }
            HNODE tex_d3d_id = ::getTexturePointer(texture_data->m_Texture);
            if (!tex_d3d_id) {
                continue;
            }
            float x1 = static_cast<float>(sprite_data->m_Rect.left) / tex->get_width();
            float y1 = static_cast<float>(sprite_data->m_Rect.top) / tex->get_width();
            float x2 = static_cast<float>(sprite_data->m_Rect.right) / tex->get_width();
            float y2 = static_cast<float>(sprite_data->m_Rect.bottom) / tex->get_width();

            ImTextureID im_tex_id = reinterpret_cast<ImTextureID>(tex_d3d_id);
            ImVec2 size(40, 40);
            ImVec2 uv1(x1, y1);
            ImVec2 uv2(x2, y2);

            // Button to learn skill
            std::string button_label = fmt::format("Learn##{}", skill_id);
            if (ImGui::Button(button_label.c_str())) {
                std::string cmd = fmt::format("/add skill {}",skill_id);
                g_pNet->Send_cli_CHAT(const_cast<char*>(cmd.c_str()));
            }
            ImGui::SameLine(65.0f);
            ImGui::Image(im_tex_id, size, uv1, uv2);
            ImGui::SameLine(115.0f);
            ImGui::Text("%s", skill_name.c_str());
        }

        ImGui::EndChild();
    }
}

void
draw_map_info() {
    static int selected_map_id = 1;

    {
        if (!ImGui::BeginChild("map_window_left_pane", ImVec2(225, 0), true)) {
            return ImGui::EndChild();
        }

        for (size_t map_id = 0; map_id < g_TblZONE.row_count; ++map_id) {
            std::string map_name(ZONE_NAME(map_id));
            if (map_name.empty()) {
                continue;
            }

            if (ImGui::Selectable(map_name.c_str(), selected_map_id == map_id)) {
                selected_map_id = map_id;
            }
        }
        ImGui::EndChild();
    }
    ImGui::SameLine();
    {
        if (!ImGui::BeginChild("map_window_right_pane", ImVec2(0, 0), true)) {
            return ImGui::EndChild();
        }

        ImGui::Text(ZONE_NAME(selected_map_id));
        ImGui::Separator();
        if (ImGui::Button("Teleport")) {
            std::string cmd = fmt::format("/tp {}", selected_map_id);
            g_pNet->Send_cli_CHAT(const_cast<char*>(cmd.c_str()));
        }

        ImGui::EndChild();
    }
}

void
draw_game_window() {
    if (!ImGui::BeginTabBar("game_data_tabs")) {
        return;
    }

    if (ImGui::BeginTabItem("Item")) {
        draw_item_info();
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Skill")) {
        draw_skill_info();
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Map")) {
        draw_map_info();
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
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

    // Setup data
    load_stat_data();
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

    // Buttons
    {
        if (ImGui::Button("Target")) {
            target_window_open = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Game")) {
            game_window_open = true;
        }
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

    if (ImGui::CollapsingHeader("Items")) {
        draw_item_info();
    }
    ImGui::End();

    draw_target_window();

    // Game data window
    if (game_window_open && ImGui::Begin("Developer - Game", &game_window_open)) {
        draw_game_window();
        ImGui::End();
    }

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