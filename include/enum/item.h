/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or  = at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef ENUM_ITEM_H
#define ENUM_ITEM_H
#include "enum.h"
#ifdef __cplusplus
extern "C" {
#endif
enum {
	QUALITY_NONE = 0,
	QUALITY_WHITE = 1,
	QUALITY_GREEN = 2,
	QUALITY_BLUE = 3,
	QUALITY_PURPLE = 4,
	QUALITY_ORANGE = 5,
	QUALITY_ORANGE_SP = 105
};

enum {
	WEAPON_NONE = 0,
	WEAPON_SWORD_ONE_HAND = 1,
	WEAPON_CROSSBOW = 2,
	WEAPON_STAFF = 3,
	WEAPON_DOUBLE_DAGGER = 4,
	WEAPON_KATANA = 5,
	WEAPON_SHURIKEN = 6,
	WEAPON_STICK = 7,
	WEAPON_SPEAR = 8,
	WEAPON_SHIELD_SMALL = 9,
	WEAPON_CATALYST = 10,
	WEAPON_CLAYMORE = 11,
	WEAPON_BOW = 12,
	WEAPON_POLE = 13
};

extern const EnumTblEnt ItemQualityTbl[];
extern const EnumTblEnt WeaponTypeTbl[];
#ifdef __cplusplus
}
#endif
#endif
