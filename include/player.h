/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef PLAYER_H
#define PLAYER_H

class Player;

#include <map>
#include <list>
#include "account.h"
#include "session.h"
#include "vector.h"
#include "world.h"
#include "scene.h"
#include "storage.pb.h"

class Player {
public:
	Player();
	Player(const storage::PlayerInfo&);
	~Player();
	operator storage::PlayerInfo() const;
	int loadFromDb();
	int loadFromDb(unsigned int);
	int saveToDb() const;
	const Account* getAccount() const;
	void setAccount(const Account*);
	unsigned int getUid() const;
	void setUid(unsigned int);
	void onLogin(Session& session);
private:
	const Account* account;
	unsigned int uid;
	Session* session;
	World* world;
	Scene* curScene;
	Vector pos;
	unsigned int scene_id;
	unsigned int tpToken;
	unsigned int nextGuid;
	unsigned int worldLevel;
	unsigned int ar;
	unsigned long long ar_exp;
	std::string name;
	std::string signature;
	unsigned int namecard;
	unsigned int pfp;
	std::map<unsigned int, int> props;
	std::list<unsigned int> openstates;
};
#endif
