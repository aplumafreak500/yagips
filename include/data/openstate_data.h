/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef OPENSTATE_DATA_H
#define OPENSTATE_DATA_H

#include <vector>

// Defined in the order they appear in OpenStateData.txt
struct OpenStateDataEnt {
	unsigned int id;
	unsigned int is_default;
	unsigned int allowClientOpen;
	struct {
		unsigned int type;
		unsigned int param[2];
	} openCond[2];
	unsigned int uiOpenId;
};

class OpenStateData {
public:
	OpenStateData();
	~OpenStateData();
	int load();
	int load(const char*);
	const OpenStateDataEnt* operator[](const unsigned int) const;
private:
	std::vector<OpenStateDataEnt> entries;
};
#endif
