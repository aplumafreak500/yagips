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
        OPEN_STATE_COND_PLAYER_LEVEL,
        OPEN_STATE_COND_QUEST,
        OPEN_STATE_OFFERING_LEVEL,
        OPEN_STATE_CITY_REPUTATION_LEVEL,
        OPEN_STATE_COND_PARENT_QUEST
};
extern const EnumTblEnt OpenStateCondTbl[];
#ifdef __cplusplus
}
#endif
#endif
