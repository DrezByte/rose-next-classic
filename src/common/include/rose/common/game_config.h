#pragma once

namespace Rose {

namespace GameStaticConfig {
const char* const NAME = "ROSE Next";

const uint8_t MAX_LEVEL = 100;
const uint16_t MAX_STAT = 300;

const int MAX_CHARACTER_NAME = 20;
const int MAX_CHARACTERS = 5;
}; // namespace GameStaticConfig

struct GameConfig {
    uint32_t base_attack_damage;
    uint32_t base_attack_speed;
    uint32_t base_move_speed;
    uint32_t base_hit;
    uint32_t base_crit;

    GameConfig():
        base_attack_damage(150),
        base_attack_speed(60),
        base_move_speed(300),
        base_hit(150),
        base_crit(0) {}
};

}; // namespace Rose
