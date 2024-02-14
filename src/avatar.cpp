/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include "util.h"
#include "avatar.h"
#include "data/avatar_data.h"
#include "define.pb.h"

Avatar::Avatar() {
	
}

Avatar::~Avatar() {}

Avatar::Avatar(unsigned int _id) {
	id = _id;
	
}

Avatar::operator proto::AvatarInfo() const {
	proto::AvatarInfo pb;
	pb.set_avatar_id(id);
	pb.set_guid(guid);
	pb.set_born_time(bornTime);
	pb.set_skill_depot_id(skillDepotId);
	// TODO lots of other proto fields
	return pb;
}
