#ifndef GODDESS_EFFECT_H
#define GODDESS_EFFECT_H
#pragma once

namespace Rose {
namespace Common {

/// A special base effect applied to all users
class GoddessEffect {
public:
	short move_speed;
	short attack_damage;
	short hit;
	short attack_speed;
	short crit;
	short additional_damage;

public:
	GoddessEffect();
	void update(int level);
};

} // Common
} // Rose

#endif // GODDESS_EFFECT_H