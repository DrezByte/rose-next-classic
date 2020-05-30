#include "stdafx.h"

#include "cuserdata.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

json
tagQuestData::to_json() {
    json j;

    json episode_vars = json::array();
    for (size_t i = 0; i < QUEST_EPISODE_VAR_CNT; ++i) {
        episode_vars.push_back(this->m_nEpisodeVAR[i]);
    }

    json job_vars = json::array();
    for (size_t i = 0; i < QUEST_JOB_VAR_CNT; ++i) {
        job_vars.push_back(this->m_nJobVAR[i]);
    }

    json planet_vars = json::array();
    for (size_t i = 0; i < QUEST_PLANET_VAR_CNT; ++i) {
        planet_vars.push_back(this->m_nPlanetVAR[i]);
    }

    json union_vars = json::array();
    for (size_t i = 0; i < QUEST_UNION_VAR_CNT; ++i) {
        union_vars.push_back(this->m_nUnionVAR[i]);
    }

    json quest_switches = json::array();
    for (size_t i = 0; i < QUEST_SWITCH_CNT / 8; ++i) {
        uint8_t b = m_btSWITCHES[i];
        quest_switches.push_back((b >> 0) & 0x1);
        quest_switches.push_back((b >> 1) & 0x1);
        quest_switches.push_back((b >> 2) & 0x1);
        quest_switches.push_back((b >> 3) & 0x1);
        quest_switches.push_back((b >> 4) & 0x1);
        quest_switches.push_back((b >> 5) & 0x1);
        quest_switches.push_back((b >> 6) & 0x1);
        quest_switches.push_back((b >> 7) & 0x1);
    }

    // TODO: CQUEST m_QUEST[QUEST_PER_PLAYER];

    j["episode"] = episode_vars;
    j["job"] = job_vars;
    j["planet"] = planet_vars;
    j["union"] = union_vars;
    j["switches"] = quest_switches;

    return j;
}

void
tagQuestData::from_json(const json& j) {

}