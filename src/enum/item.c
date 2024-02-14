/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stddef.h>
#include "enum/item.h"

const EnumTblEnt ItemQualityTbl[] = {
	ENUM(QUALITY_NONE),
	ENUM(QUALITY_WHITE),
	ENUM(QUALITY_GREEN),
	ENUM(QUALITY_BLUE),
	ENUM(QUALITY_PURPLE),
	ENUM(QUALITY_ORANGE),
	ENUM(QUALITY_ORANGE_SP),
	{-1, NULL}
};

const EnumTblEnt WeaponTypeTbl[] = {
	ENUM(WEAPON_NONE),
	ENUM(WEAPON_SWORD_ONE_HAND),
	ENUM(WEAPON_CROSSBOW),
	ENUM(WEAPON_STAFF),
	ENUM(WEAPON_DOUBLE_DAGGER),
	ENUM(WEAPON_KATANA),
	ENUM(WEAPON_SHURIKEN),
	ENUM(WEAPON_STICK),
	ENUM(WEAPON_SPEAR),
	ENUM(WEAPON_SHIELD_SMALL),
	ENUM(WEAPON_CATALYST),
	ENUM(WEAPON_CLAYMORE),
	ENUM(WEAPON_BOW),
	ENUM(WEAPON_POLE),
	{-1, NULL}
};
