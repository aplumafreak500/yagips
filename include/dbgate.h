/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef DBGATE_H
#define DBGATE_H
#include <sqlite3.h>
#include <leveldb/db.h>
#include "account.h"
#include "player.h"
#include "proto/define.pb.h"

class dbGate {
public:
	dbGate(const char*, const char*);
	~dbGate();
	/* Account manager */
	Account* getAccountByAid(unsigned int);
	Account* getAccountByUid(unsigned int);
	Account* getAccountByUsername(const char*);
	Account* getAccountByDeviceId(const char*);
	Account* getAccountByToken(const char*);
	Account* getAccountBySessionKey(const char*);
	Account* createAccount(const char*);
	int saveAccount(const Account&);
	int deleteAccount(const Account&);
	/* Player manager */
	Player* getPlayerByAccount(const Account&);
	Player* getPlayerByAid(unsigned int);
	Player* getPlayerByUid(unsigned int);
	Player* newPlayer();
	int savePlayer(const Player&);
	int deletePlayer(const Player&);
	std::string getLdbObject(const std::string&);
	int setLdbObject(const std::string&, const std::string&);
	int delLdbObject(const std::string&);
	enum LevelDbKeyType : unsigned int {
		INVENTORY = 0, // items and avatars
		ACCOUNT, // account objects (unused for now)
		PLAYER, // player objects
		FRIEND, // friendships
		GACHA, // gacha log
		MAIL, // mail
		QUEST, // quests
		BATTLE_PASS, // bp missions
		ACHIEVEMENT, // achievements
		ACTIVITY, // events
		TOWER, // spiral abyss
		WORLD, // saved group/block data
		CODEX, // in-game archive
	};
private:
	sqlite3* db;
	leveldb::DB* ldb;
	leveldb::Options ldbopt;
};
extern dbGate* globalDbGate;
#endif
