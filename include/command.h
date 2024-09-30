/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef COMMAND_H
#define COMMAND_H
#include <string>
#include <vector>
#include "player.h"

class Command {
public:
	virtual ~Command();
	virtual const std::string name() const = 0;
	virtual const std::vector<std::string> aliases() const = 0;
	virtual const std::string usage() const = 0;
	virtual int handle(const Player*, unsigned int, const char* const*) = 0;
};
void registerDefaultCommands();
void registerCommand(std::unique_ptr<Command>);
void unregisterCommand(std::unique_ptr<Command>);
void unregisterCommand(std::string);
void unregisterAllCommands();
int handleCommand(const Player*, const char*);
int handleCommand(const Player*, std::string);
int handleCommand(const Player*, unsigned int, const char* const*);
#endif
