/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stddef.h>
#include "enum/grow_curve.h"

const EnumTblEnt GrowCurveTbl[] = {
	ENUM(GROW_CURVE_NONE),
	ENUM(GROW_CURVE_HP),
	ENUM(GROW_CURVE_ATTACK),
	ENUM(GROW_CURVE_STAMINA),
	ENUM(GROW_CURVE_STRIKE),
	ENUM(GROW_CURVE_ANTI_STRIKE),
	ENUM(GROW_CURVE_ANTI_STRIKE1),
	ENUM(GROW_CURVE_ANTI_STRIKE2),
	ENUM(GROW_CURVE_ANTI_STRIKE3),
	ENUM(GROW_CURVE_STRIKE_HURT),
	ENUM(GROW_CURVE_ELEMENT),
	ENUM(GROW_CURVE_KILL_EXP),
	ENUM(GROW_CURVE_DEFENSE),
	ENUM(GROW_CURVE_ATTACK_BOMB),
	ENUM(GROW_CURVE_HP_LITTLEMONSTER),
	ENUM(GROW_CURVE_ELEMENT_MASTERY),
	ENUM(GROW_CURVE_PROGRESSION),
	ENUM(GROW_CURVE_DEFENDING),
	ENUM(GROW_CURVE_MHP),
	ENUM(GROW_CURVE_MATK),
	ENUM(GROW_CURVE_TOWERATK),
	ENUM(GROW_CURVE_HP_S5),
	ENUM(GROW_CURVE_HP_S4),
	ENUM(GROW_CURVE_HP_2),
	ENUM(GROW_CURVE_ATTACK_S5),
	ENUM(GROW_CURVE_ATTACK_S4),
	ENUM(GROW_CURVE_ATTACK_S3),
	ENUM(GROW_CURVE_STRIKE_S5),
	ENUM(GROW_CURVE_DEFENSE_S5),
	ENUM(GROW_CURVE_DEFENSE_S4),
	ENUM(GROW_CURVE_ATTACK_101),
	ENUM(GROW_CURVE_ATTACK_102),
	ENUM(GROW_CURVE_ATTACK_103),
	ENUM(GROW_CURVE_ATTACK_104),
	ENUM(GROW_CURVE_ATTACK_105),
	ENUM(GROW_CURVE_ATTACK_201),
	ENUM(GROW_CURVE_ATTACK_202),
	ENUM(GROW_CURVE_ATTACK_203),
	ENUM(GROW_CURVE_ATTACK_204),
	ENUM(GROW_CURVE_ATTACK_205),
	ENUM(GROW_CURVE_ATTACK_301),
	ENUM(GROW_CURVE_ATTACK_302),
	ENUM(GROW_CURVE_ATTACK_303),
	ENUM(GROW_CURVE_ATTACK_304),
	ENUM(GROW_CURVE_ATTACK_305),
	ENUM(GROW_CURVE_CRITICAL_101),
	ENUM(GROW_CURVE_CRITICAL_102),
	ENUM(GROW_CURVE_CRITICAL_103),
	ENUM(GROW_CURVE_CRITICAL_104),
	ENUM(GROW_CURVE_CRITICAL_105),
	ENUM(GROW_CURVE_CRITICAL_201),
	ENUM(GROW_CURVE_CRITICAL_202),
	ENUM(GROW_CURVE_CRITICAL_203),
	ENUM(GROW_CURVE_CRITICAL_204),
	ENUM(GROW_CURVE_CRITICAL_205),
	ENUM(GROW_CURVE_CRITICAL_301),
	ENUM(GROW_CURVE_CRITICAL_302),
	ENUM(GROW_CURVE_CRITICAL_303),
	ENUM(GROW_CURVE_CRITICAL_304),
	ENUM(GROW_CURVE_CRITICAL_305),
	{-1, NULL}
};
