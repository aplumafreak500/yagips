/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#if !(defined(IN_COMMANDS_ALL) || defined(IN_ACCOUNT_CMD))
#error "This file should only be included inside include/commands/all.h or src/commands/account.cpp"
#endif
class AccountCommand : public Command {
	const std::string name() const;
	const std::string usage() const;
	const std::vector<std::string> aliases() const;
	const std::string description() const;
	int handle(const Player*, unsigned int, const char* const*);
};
