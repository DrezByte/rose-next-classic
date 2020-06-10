#include "stdafx.h"

#include "cuserdata.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

#ifndef __WORLDSERVER
void to_json(json& j, const tagQuestData& q) {
    json episode_vars = json::array();
    for (size_t i = 0; i < QUEST_EPISODE_VAR_CNT; ++i) {
        episode_vars.push_back(q.m_nEpisodeVAR[i]);
    }

    json job_vars = json::array();
    for (size_t i = 0; i < QUEST_JOB_VAR_CNT; ++i) {
        job_vars.push_back(q.m_nJobVAR[i]);
    }

    json planet_vars = json::array();
    for (size_t i = 0; i < QUEST_PLANET_VAR_CNT; ++i) {
        planet_vars.push_back(q.m_nPlanetVAR[i]);
    }

    json union_vars = json::array();
    for (size_t i = 0; i < QUEST_UNION_VAR_CNT; ++i) {
        union_vars.push_back(q.m_nUnionVAR[i]);
    }

    json quest_switches = json::array();
    for (size_t i = 0; i < QUEST_SWITCH_CNT / 8; ++i) {
        const uint8_t b = q.m_btSWITCHES[i];
        quest_switches.push_back((b >> 0) & 0x1);
        quest_switches.push_back((b >> 1) & 0x1);
        quest_switches.push_back((b >> 2) & 0x1);
        quest_switches.push_back((b >> 3) & 0x1);
        quest_switches.push_back((b >> 4) & 0x1);
        quest_switches.push_back((b >> 5) & 0x1);
        quest_switches.push_back((b >> 6) & 0x1);
        quest_switches.push_back((b >> 7) & 0x1);
    }

    json quests = json::array();
    for (size_t i = 0; i < QUEST_PER_PLAYER; ++i) {
        quests.push_back(json(q.m_QUEST[i]));
    }

    j["episode"] = episode_vars;
    j["job"] = job_vars;
    j["planet"] = planet_vars;
    j["union"] = union_vars;
    j["switches"] = quest_switches;
    j["quests"] = quests;
}

void
from_json(const json& j, tagQuestData& q) {
    if (!j.is_object()) {
        return;
    }

    if (j.contains("episode") && j["episode"].is_array()) {
        json episode_vars = j["episode"];
        for (size_t i = 0; i < min(episode_vars.size(), QUEST_EPISODE_VAR_CNT); ++i) {
            q.m_nEpisodeVAR[i] = episode_vars[i];
        }
    }

    if (j.contains("job") && j["job"].is_array()) {
        json job_vars = j["job"];
        for (size_t i = 0; i < min(job_vars.size(), QUEST_JOB_VAR_CNT); ++i) {
            q.m_nJobVAR[i] = job_vars[i];
        }
    }

    if (j.contains("planet") && j["planet"].is_array()) {
        json planet_vars = j["planet"];
        for (size_t i = 0; i < min(planet_vars.size(), QUEST_PLANET_VAR_CNT); ++i) {
            q.m_nPlanetVAR[i] = planet_vars[i];
        }
    }

    if (j.contains("union") && j["union"].is_array()) {
        json planet_vars = j["union"];
        for (size_t i = 0; i < min(planet_vars.size(), QUEST_UNION_VAR_CNT); ++i) {
            q.m_nUnionVAR[i] = planet_vars[i];
        }
    }

    if (j.contains("switches") && j["switches"].is_array()) {
        json quest_switches = j["switches"];
        for (size_t i = 0; i < min(quest_switches.size(), QUEST_SWITCH_CNT) / 8; ++i) {
            uint8_t b = 0;
            b |= quest_switches[i] << 0;
            b |= quest_switches[i] << 1;
            b |= quest_switches[i] << 2;
            b |= quest_switches[i] << 3;
            b |= quest_switches[i] << 4;
            b |= quest_switches[i] << 5;
            b |= quest_switches[i] << 6;
            b |= quest_switches[i] << 7;
            q.m_btSWITCHES[i] = b;
        }
    }

    if (j.contains("quests") && j["quests"].is_array()) {
        json quests = j["quests"];
        for (size_t i = 0; i < min(quests.size(), QUEST_PER_PLAYER); ++i) {
            from_json(quests[i], q.m_QUEST[i]);
        }
    }
}
#endif

void
to_json(nlohmann::json& j, const tagSkillAbility& s) {
    j = json::array();
    for (size_t i = 0; i < MAX_LEARNED_SKILL_CNT; ++i) {
        j.push_back(s.m_nSkillINDEX[i]);
    }
}

void
from_json(const nlohmann::json& j, tagSkillAbility& s) {
    if (!j.is_array()) {
        return;
    }

    size_t skill_count = min(j.size(), MAX_LEARNED_SKILL_CNT);
    for (size_t idx = 0; idx < skill_count; ++idx) {
        s.m_nSkillINDEX[idx] = j[idx];
    }
}