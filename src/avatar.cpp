/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <time.h>
#include "util.h"
#include "avatar.h"
#include "data.h"
#include "data/avatar_data.h"
#include "define.pb.h"

Avatar::Avatar() {
	bornTime = time(NULL);
	level = 1;
	exp = 0;
	friendship = 1;
	friendship_exp = 0;
	constellation = 0;
	ascension = 0;
	wings = 14001;
	talent_level[0] = 1;
	talent_level[1] = 1;
	talent_level[2] = 1;
	energy = 0;
}

Avatar::~Avatar() {}

Avatar::Avatar(unsigned int _id) {
	bornTime = time(NULL);
	const AvatarData* data = globalGameData->avatar_data;
	const AvatarDataEnt* ent = (*data)[_id];
	id = _id;
	skillDepotId = ent->skill_depot_id;
	level = 1;
	exp = 0;
	friendship = 1;
	friendship_exp = 0;
	constellation = 0;
	ascension = 0;
	wings = 14001;
	talent_level[0] = 1;
	talent_level[1] = 1;
	talent_level[2] = 1;
	energy = 0;
	// TODO Weapon
	// TODO Costume
}

Avatar::operator proto::AvatarInfo() const {
	proto::AvatarFetterInfo* fi = new proto::AvatarFetterInfo;
	fi->set_exp_level(friendship);
	if (friendship < 10) {
		fi->set_exp_number(friendship_exp);
	}
	// TODO Auxiliary friendship data
	// TODO Has obtained namecard
	proto::AvatarInfo pb;
	pb.set_avatar_id(id);
	pb.set_guid(guid);
	pb.set_born_time(bornTime);
	pb.set_skill_depot_id(skillDepotId);
	pb.set_allocated_fetter_info(fi);
	pb.set_wearing_flycloak_id(wings);
	// TODO Costume
	// TODO Check current hp first
	pb.set_life_state(1);
	// TODO Artifacts/weapon
	// TODO Skill map
	// TODO Talent ids
	// TODO Skill level map (talent levels)
	// TODO Fight props
	// TODO Avatar type
	// TODO Props (including current/max hp, level, exp, ascension, and satiation
	// TODO Core proud skill level (?)
	// TODO Inherit proud skill list (?)
	// TODO Proud skill extra level map (?)
	return pb;
}
