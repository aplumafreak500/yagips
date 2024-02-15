/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef AVATAR_H
#define AVATAR_H
#include <string>
#include "item.h"
#include "define.pb.h"

class Avatar {
public:
	Avatar();
	~Avatar();
	Avatar(unsigned int);
	Avatar(const proto::AvatarInfo&);
	operator proto::AvatarInfo() const;
private:
	unsigned int id;
	unsigned long long guid;
	unsigned short weapon_id;
	Weapon* weapon;
	Artifact* artifacts[5];
	unsigned int level;
	unsigned int exp;
	unsigned int ascension;
	unsigned int curHp;
	unsigned int costume;
	unsigned int wings;
	unsigned int friendship;
	unsigned int friendship_exp;
	unsigned int talent_level[3]; // normal, skill, and burst
	unsigned int constellation;
	unsigned long long bornTime;
	unsigned int skillDepotId;
	unsigned int full;
	unsigned int full_cd;
	unsigned int energy; // TODO make type a float?
	// TODO Skill map
	// TODO Talent ids
	// TODO Fight props
	// TODO Avatar type
	// TODO Props
	// TODO Core proud skill level (?)
	// TODO Inherit proud skill list (?)
	// TODO Proud skill extra level map (?)
	// TODO anything else?
};
#endif
