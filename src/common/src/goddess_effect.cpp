#include <algorithm>

#include "rose/common/status_effect/goddess_effect.h"

using namespace Rose::Common;

const short GODDESS_MOVE_VALS[10] = { 100, 110, 120, 130, 140, 150, 160, 170, 180, 200 };
const short GODDESS_ATTACK_VALS[10] = { 25, 30, 35, 40, 45, 50, 55, 60, 65, 70 };
const short GODDESS_HIT_VALS[10] = { 30, 35, 40, 45, 50, 55, 60, 65, 70, 80 };
const short GODDESS_ASPEED_VALS[10] = { 18, 20, 22, 24, 26, 28, 30, 32, 34 };
const short GODDESS_CRIT_VALS[10] = { 40, 45, 50, 55, 60, 65, 70, 75, 80, 90 };
const short additional_damage_vals[10] = { 14, 16, 18, 20, 22, 24, 28, 30, 33 };

const short GODDESS_MOVE_BONUS = 23;
const short GODDESS_ATTACK_BONUS = 18;
const short GODDESS_HIT_BONUS = 20;
const short GODDESS_ASPEED_BONUS = 24;
const short GODDESS_CRIT_BONUS = 26;

GoddessEffect::GoddessEffect() :
	move_speed(0),
	attack_damage(0),
	hit(0),
	crit(0),
	attack_speed(0),
	additional_damage(0) {
}

void GoddessEffect::update(int level) {
	// The GoddessEffect works similarly to player buffs in that it scales off
	// some factor. Instead of scaling off the players int, we use their level
	// to generate a factor in the similar range of int (i.e. 15 -> 300 or
	// min_int -> max_int). This formula was created by calculating the line
	// that intersects (1, 15) and (130, 300) aka. (min_level, min_int) and
	// (max_level, max_int)
	float factor = ((2.21f * level) + 12.79f) / 100.0f;

	// Converts our factor to the range (1,2) where factor 15 is 1 and
	// factor 300 is 2. I.E. min level = 1, max level = 2
	float bonus_factor = (factor + 300.0f) / 315.0f;

	int idx = (level / 10) % 10;
	if (level >= 1) {
		int idx = std::min(level, 10) - 1;
		float val = (GODDESS_MOVE_VALS[idx] * factor) + (GODDESS_MOVE_BONUS * bonus_factor);
		this->move_speed = static_cast<short>(val);
	}
	if (level >= 11) {
		int idx = (std::min(level, 20) - 1) / 10 % 10;
		float val = (GODDESS_ATTACK_VALS[idx] * factor) + (GODDESS_ATTACK_BONUS * bonus_factor);
		this->attack_damage = static_cast<short>(val);
	}
	if (level >= 21) {
		int idx = (std::min(level, 30) - 1) / 10 % 10;
		float val = (GODDESS_HIT_VALS[idx] * factor) + (GODDESS_HIT_BONUS * bonus_factor);
		this->hit = static_cast<short>(val);
	}
	if (level >= 31) {
		int idx = (std::min(level, 40) - 1) / 10 % 10;
		float val = (GODDESS_ASPEED_VALS[idx] * factor) + (GODDESS_ASPEED_BONUS * bonus_factor);
		this->attack_speed = static_cast<short>(val);
	}
	if (level >= 41) {
		int idx = (std::min(level, 50) - 1) / 10 % 10;
		float val = (GODDESS_CRIT_VALS[idx] * factor) + (GODDESS_CRIT_BONUS * bonus_factor);
		this->crit = static_cast<short>(val);
	}
	if (level >= 51) {
		int idx = (std::min(level, 60) - 1) / 10 % 10;
		// Add extra damage
		// TODO: How does extra damage status work...?
	}
}