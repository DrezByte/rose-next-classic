#pragma once

namespace Rose {

namespace GameStaticConfig {
const char* const NAME = "ROSE Next";

const uint8_t MAX_LEVEL = 88;
const uint16_t MAX_STAT = 300;
const uint16_t MAX_STAMINA = 5000;

const int MAX_CHARACTER_NAME = 20;
const int MAX_CHARACTERS = 5;

const int MAX_MAIL_MESSAGES = 50;

const int DELETE_TIME_SEC = 3600; // 1-hour

const int DEFAULT_WALK_SPEED = 200;

const int MAX_DROP_MULTIPLIER = 10;
const int MAX_CONSUMABLE_DROP_MULTIPLIER = 1;

const float CAMERA_MIN_ZOOM = 300.0f;
const float CAMERA_MAX_ZOOM = 2700.0f;

}; // namespace GameStaticConfig

namespace Common {

struct GameConfig {
    uint32_t base_attack_power;
    uint32_t base_attack_speed;
    uint32_t base_move_speed;
    uint32_t base_hit_rate;
    uint32_t base_crit;

    GameConfig():
        base_attack_power(0),
        base_attack_speed(0),
        base_move_speed(0),
        base_hit_rate(0),
        base_crit(0) {}
};

} // namespace Common
}; // namespace Rose
