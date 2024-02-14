/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef AVATAR_DATA_H
#define AVATAR_DATA_H

#include <vector>
#include <unordered_map>
#include <string>

// Defined in the order they appear in AvatarData.txt, except for physical res/bonus, which are grouped with the other elements
struct AvatarDataEnt {
	unsigned int id;
	// std::string default_camp; // UNK, vanilla all empty
	long double base_hp;
	long double base_atk;
	long double base_def;
	long double crit_rate;
	long double crit_res; // ???
	long double crit_dmg;
	long double element_res[8];
	long double element_bonus[8];
	struct {
		unsigned int prop;
		unsigned int type;
	} prop_curves[3];
	long double em;
	// phys res/bonus usually goes here
	unsigned int useType;
	unsigned int quality;
	unsigned int charge_efficiency; // synonym for er?
	// unsigned int treatment_effect; // UNK, vanilla all 0
	// unsigned int therapeutic_effect; // UNK, vanilla all 0
	std::string battle_config_file;
	unsigned int has_range; // set for all bow units, unset for everyone else
	unsigned int initial_weapon;
	unsigned int weapon_type;
	unsigned int skill_depot_id;
	unsigned int stanima_recover_speed;
	std::vector<unsigned int> candidate_skill_depot_ids; // only the Traveler has these normally
	unsigned int identity; // normally 0 for the traveler, 1 for everyone else
	unsigned int promote_id;
	// <promote level, promote reward pool>
	std::unordered_map<unsigned int, unsigned int> promote_rewards;
	unsigned int prop_group_id; // Normally the same as their regular id, except in test characters
};

class AvatarData {
public:
	AvatarData();
	~AvatarData();
	int load(const char*);
	const AvatarDataEnt* operator[](const unsigned int) const;
private:
	std::vector<AvatarDataEnt> entries;
};
#endif
