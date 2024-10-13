/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stddef.h>
#include "prop.h"

#define PROP_MINMAX(_name, _min, _max) {\
	.id = _name,\
	.name = #_name,\
	.min = _min,\
	.max = _max\
}
#define PROP(name) PROP_MINMAX(name, 0x8000000000000000, 0x7fffffffffffffff)
#define PROP_MIN(name, min) PROP_MINMAX(name, min, 0x7fffffffffffffff)

const PropMinMaxData PlayerProps[] = {
	PROP(PROP_NONE), // null
	PROP_MIN(PROP_EXP, 0), // traveler/current character exp?
	PROP(PROP_BREAK_LEVEL), // traveler/current character ascension level?
	PROP(PROP_SATIATION_VAL), // travler/current character satiation?
	PROP(PROP_SATIATION_PENALTY_TIME),
	PROP_MINMAX(PROP_LEVEL, 0, 90), // traveler/current character level?
	PROP(PROP_LAST_CHANGE_AVATAR_TIME), // Time the player last swapped on-field avatars, used to enforce the cooldown server side
	PROP_MINMAX(PROP_MAX_SPRING_VOLUME, 0, 8500000), // Max spare recovery volume of the statues
	PROP_MIN(PROP_CUR_SPRING_VOLUME, true), // Current spare recovery volume of the statues(?)
	PROP(PROP_IS_SPRING_AUTO_USE), // Whether the statues auto recover HP
	PROP_MINMAX(PROP_SPRING_AUTO_USE_PERCENT, 0, 100), // Percent of avatars' Max HP to recover when near statues
	PROP(PROP_IS_FLYABLE), // Whether gliding is enabled
	PROP(PROP_IS_WEATHER_LOCKED), // Whether the weather cycle is enabled
	PROP(PROP_IS_GAME_TIME_LOCKED), // Whether in-game time progresses normally
	PROP(PROP_IS_TRANSFERABLE), // something co op related???
	PROP_MINMAX(PROP_MAX_STAMINA, 0, 24000), // Max stanima
	PROP_MIN(PROP_CUR_PERSIST_STAMINA, true), // Current stanima (?)
	PROP(PROP_CUR_TEMPORARY_STAMINA), // Current stanima (?)
	PROP_MINMAX(PROP_PLAYER_LEVEL, 1, 60), // AR
	PROP_MIN(PROP_PLAYER_EXP, 0), // Adventure EXP
	PROP(PROP_PLAYER_HCOIN), // Primogems
	PROP(PROP_PLAYER_SCOIN), // Mora
	PROP_MINMAX(PROP_PLAYER_MP_SETTING_TYPE, 0, 2), // Co-op setting (reject, allow, ask)
	PROP_MINMAX(PROP_IS_MP_MODE_AVAILABLE, 0, 1), // Whether co-op is enabled
	PROP_MINMAX(PROP_PLAYER_WORLD_LEVEL, 0, 9), // World level
	PROP_MINMAX(PROP_PLAYER_RESIN, 0, 2000), // Resin
	PROP(PROP_PLAYER_WAIT_SUB_HCOIN), // ?
	PROP(PROP_PLAYER_WAIT_SUB_SCOIN), // ?
	PROP(PROP_IS_ONLY_MP_WITH_PS_PLAYER), // On PSN accounts, whether co-op requests can only be received by other PSN players
	PROP(PROP_PLAYER_MCOIN), // Genesis Crystals
	PROP(PROP_PLAYER_WAIT_SUB_MCOIN), // ?
	PROP_MINMAX(PROP_PLAYER_LEGENDARY_KEY, 0, 3), // Story Keys
	PROP(PROP_IS_HAS_FIRST_SHARE), // In-game sharing has been done once
	PROP_MINMAX(PROP_PLAYER_FORGE_POINT, 0, 300000), // Total weapon enhancement exp obtained from forging
	PROP(PROP_CUR_CLIMATE_METER), // Sheer Cold/Blazing Heat timer?
	PROP(PROP_CUR_CLIMATE_TYPE), // ???
	PROP(PROP_CUR_CLIMATE_AREA_ID), // ???
	PROP(PROP_CUR_CLIMATE_AREA_CLIMATE_TYPE), // ???
	PROP_MINMAX(PROP_PLAYER_WORLD_LEVEL_LIMIT, 0, 9), // Max World Level
	PROP(PROP_PLAYER_WORLD_LEVEL_ADJUST_CD), // World level adjustment timer
	PROP(PROP_PLAYER_LEGENDARY_DAILY_TASK_NUM), // Number of commissions to complete for another Story Key
};
