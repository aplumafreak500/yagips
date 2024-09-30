/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include "command.h"
#include "player.h"
#include "commands/all.h"

Command::~Command() {}

static std::vector<std::unique_ptr<Command>> commands;

void registerDefaultCommands() {
	commands.emplace_back(new AccountCommand());
}

void registerCommand(std::unique_ptr<Command> cmd) {
	commands.push_back(std::move(cmd));
}

void unregisterCommand(std::unique_ptr<Command> cmd) {
	for (auto i = commands.cbegin(); i != commands.cend(); i++) {
		if (*i == cmd) {
			commands.erase(i);
		}
	}
}

void unregisterCommand(std::string cmd) {
	for (auto i = commands.cbegin(); i != commands.cend(); i++) {
		if ((*i)->name() == cmd) {
			commands.erase(i);
			continue;
		}
		const std::vector<std::string> a = (*i)->aliases();
		for (auto j = a.cbegin(); j != a.cend(); j++) {
			if (*j == cmd) {
				commands.erase(i);
				break;
			}
		}
	}
}

void unregisterAllCommands() {
	commands.empty();
}

int handleCommand(const Player* player, const char* cmd) {
	static const char* argv[32];
	unsigned int argc = 0;
	static char argbuf[1024];
	char* arg_ptr = argbuf;
	char* arg_dst = argbuf;
	const char* c = cmd;
	unsigned int quotes = 0;
	size_t len = 0;
	argbuf[1023] = '\0';
	while (*c != '\0') {
		if (argc >= 32) break;
		len = (size_t) arg_dst - (size_t) argbuf;
		if (len >= 1023) break;
		switch(*c) {
		case '\'':
			if (quotes == 1) {
				quotes = 0;
				c++;
				break;
			}
			else if (quotes == 0) {
				quotes = 1;
				c++;
				break;
			}
			else {
				*arg_dst = *c;
				c++;
				arg_dst++;
				break;
			}
		case '"':
			if (quotes == 2) {
				quotes = 0;
				c++;
				break;
			}
			else if (quotes == 0) {
				quotes = 2;
				c++;
				break;
			}
			else {
				*arg_dst = *c;
				c++;
				arg_dst++;
				break;
			}
		case ' ':
			if (quotes) {
				*arg_dst = *c;
				c++;
				arg_dst++;
				break;
			}
			else {
				*arg_dst = '\0';
				c++;
				arg_dst++;
				argv[argc] = arg_ptr;
				argc++;
				arg_ptr = arg_dst;
				break;
			}
		default:
			*arg_dst = *c;
			c++;
			arg_dst++;
			break;
		}
	}
	return handleCommand(player, argc, argv);
}

int handleCommand(const Player* p, std::string c) {
	return handleCommand(p, c.c_str());
}

int handleCommand(const Player* player, unsigned int argc, const char* const* argv) {
	if (argc == 0) return -1;
	std::string cmd(argv[0]);
	for (auto i = commands.cbegin(); i != commands.cend(); i++) {
		if ((*i)->name() == cmd) {
			return (*i)->handle(player, argc - 1, &argv[1]);
		}
		const std::vector<std::string> a = (*i)->aliases();
		for (auto j = a.cbegin(); j != a.cend(); j++) {
			if (*j == cmd) {
	 			return (*i)->handle(player, argc - 1, &argv[1]);
			}
		}
	}
	return -2;
}
