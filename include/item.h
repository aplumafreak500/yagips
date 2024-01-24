/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef ITEM_H
#define ITEM_H

#include "define.pb.h"

enum {
	ITEM_MATERIAL = 0,
	ITEM_ARTIFACT = 1,
	ITEM_WEAPON = 2,
};

struct Material {
	unsigned int count;
	// TODO: "countdown" (???) and "delay week countdown" (Transient Resin). These use a map-type in the proto so that individually obtained instances of the item can expire without the whole stack expiring.
	unsigned long expireTime;
};

struct Weapon {
	unsigned int exp;
	unsigned char locked;
	unsigned char level;
	unsigned char ascension;
	unsigned char refinement;
	// Store passive effect id's. This can save time looking up that data in the excelconfig and it's also useful to give one weapon the effects from a different weapon.
	unsigned int numAffixes;
	unsigned int affixes[];
};

struct ArtifactSubStat {
	unsigned short id;
	unsigned short count;
};

// Proto-level "promote_level" is not stored and is calculated automatically for artifacts.
struct Artifact {
	unsigned int exp;
	unsigned char locked;
	unsigned char level;
	unsigned short mainStat;
	unsigned short numSubStats;
	struct ArtifactSubStat subStats[];
};

struct Item {
	unsigned int id;
	unsigned int type;
	unsigned long long guid;
	union {
		struct Artifact artifact;
		struct Weapon weapon;
		struct Material material;
	} data;
	Item(proto::Item);
	operator proto::Item() const;
};
#endif
