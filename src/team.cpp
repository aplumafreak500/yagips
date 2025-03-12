/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2025 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <string>
#include <list>
#include "avatar.h"
#include "team.h"
#include "player.h"
#include "avatar.pb.h"

AvatarTeam::AvatarTeam() {}
AvatarTeam::~AvatarTeam() {}

AvatarTeam::AvatarTeam(const proto::AvatarTeam& p, const Player& player) {
	const Avatar* a;
	name = p.team_name();
	for (int i = 0; i < p.avatar_guid_list_size(); i++) {
		a = player.getAvatarByGuid(p.avatar_guid_list(i));
		avatars.push_back(a);
	}
}

AvatarTeam::operator proto::AvatarTeam() const {
	proto::AvatarTeam p;
	p.set_team_name(name);
	for (auto i = avatars.cbegin(); i != avatars.cend(); i++) {
		if (*i != NULL) {
			p.add_avatar_guid_list((*i)->getGuid());
		}
	}
	return p;
}

const std::string& AvatarTeam::getName() const {
	return name;
}

void AvatarTeam::setName(const std::string& n) {
	name = n;
}

const std::list<const Avatar*>& AvatarTeam::getAvatars() const {
	return avatars;
}

void AvatarTeam::setAvatars(const std::list<const Avatar*>& a) {
	avatars = a;
}

int AvatarTeam::addAvatar(const Avatar* a) {
	if (a == NULL) return -1;
	int ret = 0;
	for (auto i = avatars.cbegin(); i != avatars.cend(); i++) {
		if (*i == NULL) {
			avatars.erase(i);
			continue;
		}
		if (*i == a || (*i)->getGuid() == a->getGuid())	return ret;
		ret++;
	}
	if (ret >= 4) return -1;
	avatars.push_back(a);
	return ret + 1;
}

void AvatarTeam::delAvatar(const Avatar* a) {
	if (a == NULL) return;
	for (auto i = avatars.cbegin(); i != avatars.cend(); i++) {
		if (*i == NULL || *i == a || (*i)->getGuid() == a->getGuid()) {
			avatars.erase(i);
		}
	}
}

void AvatarTeam::delAvatar(unsigned int i) {
	unsigned int d = 0;
	for (auto t = avatars.cbegin(); t != avatars.cend(); t++) {
		if (*t == NULL || d == i) {
			avatars.erase(t);
		}
		d++;
	}
}

void AvatarTeam::delAvatar(unsigned long long guid) {
	for (auto i = avatars.cbegin(); i != avatars.cend(); i++) {
		if (*i == NULL || (*i)->getGuid() == guid) {
			avatars.erase(i);
		}
	}
}

const Avatar* AvatarTeam::getAvatar(unsigned int i) const {
	unsigned int d = 0;
	for (auto t = avatars.cbegin(); t != avatars.cend(); t++) {
		if (d == i) {
			return *t;
		}
		d++;
	}
	return NULL;
}

const Avatar* AvatarTeam::getAvatar(unsigned long long guid) const {
	for (auto i = avatars.cbegin(); i != avatars.cend(); i++) {
		if ((*i)->getGuid() == guid) {
			return *i;
		}
	}
	return NULL;
}

int AvatarTeam::setAvatar(const Avatar* a, unsigned int i) {
	unsigned int ret = 0;
	for (auto t = avatars.begin(); t != avatars.end(); t++) {
		if (*t == NULL) {
			avatars.erase(t);
			continue;
		}
		if (ret == i) {
			*t = a;
			return ret;
		}
		ret++;
	}
	return -1;
}

size_t AvatarTeam::size() const {
	return avatars.size();
}

bool AvatarTeam::operator ==(const AvatarTeam& t) const {
	if (name != t.getName()) return false;
	std::list<const Avatar*> c_avatars = t.getAvatars();
	if (avatars == c_avatars) {
		// high confidence, since we only store pointers
		return true;
	}
	if (avatars.size() != t.size()) return false;
	auto i = avatars.cbegin();
	auto j = c_avatars.cbegin();
	while (1) {
		if (&(*i) != &(*j)) {
			//if (*i != *j) return false; // TODO
			if ((*i)->getGuid() != (*j)->getGuid()) return false;
		}
		i++;
		if (i == avatars.cend()) {
			if (j == c_avatars.cend()) break;
			return false;
		}
		i++;
		j++;
	}
	return true;
}

bool AvatarTeam::operator !=(const AvatarTeam& t) const {
	return !(*this == t);
}
