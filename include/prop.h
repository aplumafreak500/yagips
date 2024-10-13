/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef PROP_H
#define PROP_H
#ifdef __cplusplus
extern "C" {
#endif

enum {
	PROP_NONE = 0,
	PROP_EXP = 1001,
	PROP_BREAK_LEVEL = 1002,
	PROP_SATIATION_VAL = 1003,
	PROP_SATIATION_PENALTY_TIME = 1004,
	PROP_LEVEL = 4001,
	PROP_LAST_CHANGE_AVATAR_TIME = 10001,
	PROP_MAX_SPRING_VOLUME = 10002,
	PROP_CUR_SPRING_VOLUME = 10003,
	PROP_IS_SPRING_AUTO_USE = 10004,
	PROP_SPRING_AUTO_USE_PERCENT = 10005,
	PROP_IS_FLYABLE = 10006,
	PROP_IS_WEATHER_LOCKED = 10007,
	PROP_IS_GAME_TIME_LOCKED = 10008,
	PROP_IS_TRANSFERABLE = 10009,
	PROP_MAX_STAMINA = 10010,
	PROP_CUR_PERSIST_STAMINA = 10011,
	PROP_CUR_TEMPORARY_STAMINA = 10012,
	PROP_PLAYER_LEVEL = 10013,
	PROP_PLAYER_EXP = 10014,
	PROP_PLAYER_HCOIN = 10015,
	PROP_PLAYER_SCOIN = 10016,
	PROP_PLAYER_MP_SETTING_TYPE = 10017,
	PROP_IS_MP_MODE_AVAILABLE = 10018,
	PROP_PLAYER_WORLD_LEVEL = 10019,
	PROP_PLAYER_RESIN = 10020,
	PROP_PLAYER_WAIT_SUB_HCOIN = 10022,
	PROP_PLAYER_WAIT_SUB_SCOIN = 10023,
	PROP_IS_ONLY_MP_WITH_PS_PLAYER = 10024,
	PROP_PLAYER_MCOIN = 10025,
	PROP_PLAYER_WAIT_SUB_MCOIN = 10026,
	PROP_PLAYER_LEGENDARY_KEY = 10027,
	PROP_IS_HAS_FIRST_SHARE = 10028,
	PROP_PLAYER_FORGE_POINT = 10029,
	PROP_CUR_CLIMATE_METER = 10035,
	PROP_CUR_CLIMATE_TYPE = 10036,
	PROP_CUR_CLIMATE_AREA_ID = 10037,
	PROP_CUR_CLIMATE_AREA_CLIMATE_TYPE = 10038,
	PROP_PLAYER_WORLD_LEVEL_LIMIT = 10039,
	PROP_PLAYER_WORLD_LEVEL_ADJUST_CD = 10040,
	PROP_PLAYER_LEGENDARY_DAILY_TASK_NUM = 10041,
};

typedef struct {
	unsigned int id;
	const char* name;
	long long min;
	long long max;
} PropMinMaxData;

extern const PropMinMaxData PlayerProps[];
#ifdef __cplusplus
}
#endif
#endif
