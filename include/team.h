/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef TEAM_H
#define TEAM_H
class AvatarTeam;
#include <string>
#include <list>
#include "avatar.h"
#include "avatar.pb.h"
class AvatarTeam {
public:
	AvatarTeam();
	~AvatarTeam();
	const std::string& getName() const;
	void setName(const std::string&);
	const std::list<const Avatar*>& getAvatars() const;
	void setAvatars(const std::list<const Avatar*>&);
	int addAvatar(const Avatar*);
	void delAvatar(const Avatar*);
	void delAvatar(unsigned int);
	void delAvatar(unsigned long long);
	const Avatar* getAvatar(unsigned int) const;
	const Avatar* getAvatar(unsigned long long) const;
	int setAvatar(const Avatar*, unsigned int);
	size_t size() const;
	operator proto::AvatarTeam() const;
private:
	std::string name;
	std::list<const Avatar*> avatars;
};
#endif
