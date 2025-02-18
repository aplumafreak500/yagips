/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2025 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef AVATAR_SKILL_DEPOT_DATA_H
#define AVATAR_SKILL_DEPOT_DATA_H

#include <vector>
#include <string>

// Defined in the order they appear in AvatarSkillDepotData.txt
struct AvatarSkillDepotDataEnt {
	unsigned int id;
	unsigned int burstSkill;
	// blank field "天赋技能" ("Talents and Skills"), probably just a separator
	unsigned int skills[4]; // Normal attack and elemental skill. The two others are usually not set.
	std::vector<unsigned int> secondarySkills; // Unknown what these are used for
	unsigned int aimTalent; // Set to 20000 for all bow units and unset for all others
	unsigned int leaderTalent; // Only a few units have this, most of them from client version 1.0. Units that were added in later client versions tend not to have this, indicating it was likely a scrapped mechanic.
	unsigned int extraAbilities[3]; // These are usually unset.
	unsigned int constellationSkills[6];
	std::string talentConfigFile;
	unsigned int coreSkillGroupId; // "核心得意技组ID", nobody at all seems to have this set normally
	unsigned int requiredAscensionForExtraTalents; // Everybody has this either unset or set to 0 normally
	struct {
		unsigned int id;
		unsigned int requiredAscensionLevel;
	} ascensionPassiveSkills[5]; // The last 2 are usually unset or set to 0
	std::string abilityGroup; // Only the Traveler's skills have this normally
};

class AvatarSkillDepotData {
public:
	AvatarSkillDepotData();
	~AvatarSkillDepotData();
	int load();
	int load(const char*);
	const AvatarSkillDepotDataEnt* operator[](const unsigned int) const;
	const AvatarSkillDepotDataEnt* at(const unsigned int) const;
	size_t size() const;
private:
	std::vector<AvatarSkillDepotDataEnt> entries;
};
#endif
