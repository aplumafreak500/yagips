/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <sqlite3.h>
#include <leveldb/db.h>
#include <stdexcept>
#include "account.h"
#include "dbgate.h"

extern "C" {
	static int createTables(sqlite3*);
}

dbGate* globalDbGate;

dbGate::dbGate(const char* path, const char* ldbpath) {
	int ret = sqlite3_open_v2(path, &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
	static char b[1024];
	if (ret != SQLITE_OK) {
		snprintf(b, 1024, "SQLite connection couldn't be made, error %d", -ret);
		throw std::runtime_error(b);
	}
	if (createTables(db)) {
		throw std::runtime_error("Unable to create at least one database table.");
	}
	// Don't compress on LevelDB side, since (soon:tm:) we apply Zlib compression to most input data.
	ldbopt.compression = leveldb::kNoCompression;
	ldbopt.create_if_missing = true;
	leveldb::Status s = leveldb::DB::Open(ldbopt, ldbpath, &ldb);
	if (!s.ok()) {
		snprintf(b, 1024, "LevelDB object couldn't be created, error: %s", s.ToString().c_str());
		throw std::runtime_error(b);
	}
	load();
}

dbGate::~dbGate() {
	save();
	sqlite3_close_v2(db);
	if (ldb != NULL) delete ldb;
}

int dbGate::load() {
	std::string val = getLdbObject("nextID");
	if (val.empty()) return 0;
	if (!next_ids.ParseFromString(val)) return -1;
	return 0;
}

int dbGate::save() {
	std::string val;
	if (!next_ids.SerializeToString(&val)) {
		return -1;
	}
	return setLdbObject("nextID", val);
}

extern "C" {
	static int createTables(sqlite3* db) {
		// TODO create more tables
		// Table structures subject to change
		int ret = sqlite3_exec(db,
			"CREATE TABLE IF NOT EXISTS accounts (aid INTEGER PRIMARY KEY, timeCreated INTEGER NOT NULL DEFAULT (strftime('%s', 'now')), username TEXT UNIQUE, password TEXT, deviceId TEXT, email TEXT, token TEXT, sessionKeyCreated INTEGER, sessionKey TEXT);",
		NULL, NULL, NULL);
		if (ret != SQLITE_OK) {
			fprintf(stderr, "Unable to create accounts table - errcode %d\n", -ret);
			return -1;
		}
		// CREATE TABLE bans (aid INTEGER, uid INTEGER, ip TEXT, reason TEXT, start INTEGER DEFAULT (strftime('%s', 'now')), end INTEGER);
		// TODO More data than just uid and aid
		ret = sqlite3_exec(db,
			"CREATE TABLE IF NOT EXISTS players (uid INTEGER PRIMARY KEY, aid INTEGER);",
		NULL, NULL, NULL);
		if (ret != SQLITE_OK) {
			fprintf(stderr, "Unable to create accounts table - errcode %d\n", -ret);
			return -1;
		}
		return 0;
	}
}

Account* dbGate::getAccountByAid(unsigned int aid) {
	sqlite3_stmt* stmt;
	Account* account;
	const char* username;
	const char* password;
	const char* deviceId;
	const char* email;
	const char* token;
	const char* sessionKey;
	int ret = sqlite3_prepare(db, "SELECT username, password, email, token, sessionKey, deviceId, sessionKeyCreated FROM accounts WHERE aid = ?1 limit 1;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_bind_int(stmt, 1, aid);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_step(stmt);
	switch (ret) {
	default: // error
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_DONE: // no results
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_ROW: // got a hit
		account = new Account();
		account->setAccountId(aid);
		username = (const char*) sqlite3_column_text(stmt, 0);
		if (username != NULL) {
			account->setUsername(username);
			account->setIsGuest(0);
		}
		else {
			account->setIsGuest(1);
		}
		password = (const char*) sqlite3_column_text(stmt, 1);
		if (password != NULL) account->setPasswordHash(password);
		email = (const char*) sqlite3_column_text(stmt, 2);
		if (email != NULL) account->setEmail(email);
		token = (const char*) sqlite3_column_text(stmt, 3);
		if (token != NULL) account->setToken(token);
		sessionKey = (const char*) sqlite3_column_text(stmt, 4);
		if (sessionKey != NULL) account->setSessionKey(sessionKey);
		deviceId = (const char*) sqlite3_column_text(stmt, 5);
		if (deviceId != NULL) account->setDeviceId(deviceId);
		account->setSessionKeyTimestamp(sqlite3_column_int(stmt, 6));
		sqlite3_finalize(stmt);
		return account;
	}
}

Account* dbGate::getAccountByUid(unsigned int uid) {
	Player* player = getPlayerByUid(uid);
	if (player == NULL) return NULL;
	return (Account*) player->getAccount();
}

Account* dbGate::getAccountByUsername(const char* username) {
	if (username == NULL) return NULL;
	sqlite3_stmt* stmt;
	Account* account;
	const char* password;
	const char* deviceId;
	const char* email;
	const char* token;
	const char* sessionKey;
	int ret = sqlite3_prepare(db, "SELECT aid, password, email, token, sessionKey, deviceId, sessionKeyCreated FROM accounts WHERE username = ?1 limit 1;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_step(stmt);
	switch (ret) {
	default: // error
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_DONE: // no results
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_ROW: // got a hit
		account = new Account();
		account->setUsername(username);
		account->setAccountId(sqlite3_column_int(stmt, 0));
		password = (const char*) sqlite3_column_text(stmt, 1);
		if (password != NULL) account->setPasswordHash(password);
		email = (const char*) sqlite3_column_text(stmt, 2);
		if (email != NULL) account->setEmail(email);
		token = (const char*) sqlite3_column_text(stmt, 3);
		if (token != NULL) account->setToken(token);
		sessionKey = (const char*) sqlite3_column_text(stmt, 4);
		if (sessionKey != NULL) account->setSessionKey(sessionKey);
		deviceId = (const char*) sqlite3_column_text(stmt, 5);
		if (deviceId != NULL) account->setDeviceId(deviceId);
		account->setSessionKeyTimestamp(sqlite3_column_int(stmt, 6));
		sqlite3_finalize(stmt);
		return account;
	}
}

Account* dbGate::getAccountByToken(const char* token) {
	sqlite3_stmt* stmt;
	Account* account;
	const char* username;
	const char* password;
	const char* deviceId;
	const char* email;
	const char* sessionKey;
	int ret = sqlite3_prepare(db, "SELECT aid, username, password, email, sessionKey, deviceId, sessionKeyCreated FROM accounts WHERE token = ?1 limit 1;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_bind_text(stmt, 1, token, -1, SQLITE_STATIC);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_step(stmt);
	switch (ret) {
	default: // error
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_DONE: // no results
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_ROW: // got a hit
		account = new Account();
		account->setToken(token);
		account->setAccountId(sqlite3_column_int(stmt, 0));
		username = (const char*) sqlite3_column_text(stmt, 1);
		if (username != NULL) {
			account->setUsername(username);
			account->setIsGuest(0);
		}
		else {
			account->setIsGuest(1);
		}
		password = (const char*) sqlite3_column_text(stmt, 2);
		if (password != NULL) account->setPasswordHash(password);
		email = (const char*) sqlite3_column_text(stmt, 3);
		if (email != NULL) account->setEmail(email);
		sessionKey = (const char*) sqlite3_column_text(stmt, 4);
		if (sessionKey != NULL) account->setSessionKey(sessionKey);
		deviceId = (const char*) sqlite3_column_text(stmt, 5);
		if (deviceId != NULL) account->setDeviceId(deviceId);
		account->setSessionKeyTimestamp(sqlite3_column_int(stmt, 6));
		sqlite3_finalize(stmt);
		return account;
	}
}

Account* dbGate::getAccountBySessionKey(const char* sessionKey) {
	sqlite3_stmt* stmt;
	Account* account;
	const char* username;
	const char* password;
	const char* deviceId;
	const char* email;
	const char* token;
	int ret = sqlite3_prepare(db, "SELECT aid, username, password, email, token, deviceId, sessionKeyCreated FROM accounts WHERE sessionKey = ?1 limit 1;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_bind_text(stmt, 1, sessionKey, -1, SQLITE_STATIC);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_step(stmt);
	switch (ret) {
	default: // error
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_DONE: // no results
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_ROW: // got a hit
		account = new Account();
		account->setSessionKey(sessionKey);
		account->setAccountId(sqlite3_column_int(stmt, 0));
		username = (const char*) sqlite3_column_text(stmt, 1);
		if (username != NULL) {
			account->setUsername(username);
			account->setIsGuest(0);
		}
		else {
			account->setIsGuest(1);
		}
		password = (const char*) sqlite3_column_text(stmt, 2);
		if (password != NULL) account->setPasswordHash(password);
		email = (const char*) sqlite3_column_text(stmt, 3);
		if (email != NULL) account->setEmail(email);
		token = (const char*) sqlite3_column_text(stmt, 4);
		if (token != NULL) account->setToken(token);
		deviceId = (const char*) sqlite3_column_text(stmt, 5);
		if (deviceId != NULL) account->setDeviceId(deviceId);
		account->setSessionKeyTimestamp(sqlite3_column_int(stmt, 6));
		sqlite3_finalize(stmt);
		return account;
	}
}

Account* dbGate::getAccountByDeviceId(const char* deviceId) {
	sqlite3_stmt* stmt;
	Account* account;
	const char* username;
	const char* password;
	const char* email;
	const char* token;
	const char* sessionKey;
	int ret = sqlite3_prepare(db, "SELECT aid, username, password, email, token, sessionKey, sessionKeyCreated FROM accounts WHERE sessionKey = ?1 limit 1;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_bind_text(stmt, 1, deviceId, -1, SQLITE_STATIC);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_step(stmt);
	switch (ret) {
	default: // error
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_DONE: // no results
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_ROW: // got a hit
		account = new Account();
		account->setDeviceId(deviceId);
		account->setAccountId(sqlite3_column_int(stmt, 0));
		username = (const char*) sqlite3_column_text(stmt, 1);
		if (username != NULL) {
			account->setUsername(username);
			account->setIsGuest(0);
		}
		else {
			account->setIsGuest(1);
		}
		password = (const char*) sqlite3_column_text(stmt, 2);
		if (password != NULL) account->setPasswordHash(password);
		email = (const char*) sqlite3_column_text(stmt, 3);
		if (email != NULL) account->setEmail(email);
		token = (const char*) sqlite3_column_text(stmt, 4);
		if (token != NULL) account->setToken(token);
		sessionKey = (const char*) sqlite3_column_text(stmt, 5);
		if (sessionKey != NULL) account->setSessionKey(sessionKey);
		account->setSessionKeyTimestamp(sqlite3_column_int(stmt, 6));
		sqlite3_finalize(stmt);
		return account;
	}
}

Account* dbGate::createAccount(const char* username) {
	sqlite3_stmt* stmt;
	Account* account = new Account();
	if (username != NULL) {
		account->setUsername(username);
		account->setIsGuest(0);
	}
	else {
		account->setIsGuest(1);
	}
	int ret = sqlite3_prepare(db, "INSERT INTO accounts(username, token, sessionKey) VALUES (?1, ?2, ?3);", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		delete account;
		return NULL;
	}
	ret = sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		delete account;
		return NULL;
	}
	ret = sqlite3_bind_text(stmt, 2, account->getNewToken().c_str(), -1, SQLITE_STATIC);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		delete account;
		return NULL;
	}
	ret = sqlite3_bind_text(stmt, 3, account->getNewSessionKey().c_str(), -1, SQLITE_STATIC);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		delete account;
		return NULL;
	}
	ret = sqlite3_step(stmt);
	if (ret != SQLITE_DONE) {
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		delete account;
		return NULL;
	}
	sqlite3_finalize(stmt);
	account->setAccountId(sqlite3_last_insert_rowid(db));
	saveAccount(*account);
	return account;
}

int dbGate::saveAccount(const Account& account) {
	sqlite3_stmt* stmt;
	int ret = sqlite3_prepare(db, "UPDATE accounts SET username = ?1, password = ?2, email = ?3, token = ?4, sessionKey = ?5, deviceId = ?7, sessionKeyCreated = ?8 WHERE aid = ?6;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -3;
	}
	if (!account.isGuest()) {
		ret = sqlite3_bind_text(stmt, 1, account.getUsername().c_str(), -1, SQLITE_STATIC);
		if (ret != SQLITE_OK) {
			fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
			return -2;
		}
		ret = sqlite3_bind_text(stmt, 2, account.getPasswordHash().c_str(), -1, SQLITE_STATIC);
		if (ret != SQLITE_OK) {
			fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
			return -2;
		}
		ret = sqlite3_bind_text(stmt, 3, account.getEmail().c_str(), -1, SQLITE_STATIC);
		if (ret != SQLITE_OK) {
			fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
			return -2;
		}
	}
	// TODO Turn empty into null
	ret = sqlite3_bind_text(stmt, 4, account.getToken().c_str(), -1, SQLITE_STATIC);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -2;
	}
	ret = sqlite3_bind_text(stmt, 5, account.getSessionKey().c_str(), -1, SQLITE_STATIC);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -2;
	}
	ret = sqlite3_bind_int(stmt, 6, account.getAccountId());
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -2;
	}
	ret = sqlite3_bind_text(stmt, 7, account.getDeviceId().c_str(), -1, SQLITE_STATIC);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -2;
	}
	ret = sqlite3_bind_int(stmt, 8, account.getSessionKeyTimestamp());
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -2;
	}
	ret = sqlite3_step(stmt);
	if (ret != SQLITE_DONE) {
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		return -1;
	}
	sqlite3_finalize(stmt);
	return 0;
}

int dbGate::deleteAccount(const Account& account) {
	sqlite3_stmt* stmt;
	int ret = sqlite3_prepare(db, "DELETE FROM accounts WHERE aid = ?1;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -2;
	}
	ret = sqlite3_bind_int(stmt, 1, account.getAccountId());
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -2;
	}
	ret = sqlite3_step(stmt);
	if (ret != SQLITE_DONE) {
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		return -1;
	}
	sqlite3_finalize(stmt);
	return 0;
}

Player* dbGate::getPlayerByAccount(const Account& account) {
	sqlite3_stmt* stmt;
	Player* player;
	unsigned int aid = account.getAccountId();
	int ret = sqlite3_prepare(db, "SELECT uid FROM players WHERE aid = ?1 limit 1;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_bind_int(stmt, 1, aid);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_step(stmt);
	switch (ret) {
	default: // error
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_DONE: // no results
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_ROW: // got a hit
		player = new Player();
		player->setUid(sqlite3_column_int(stmt, 0));
		player->setAccount(&account);
		return player;
	}
}

Player* dbGate::getPlayerByAid(unsigned int aid) {
	const Account* account = getAccountByAid(aid);
	if (account != NULL) return getPlayerByAccount(*account);
	// If null, try a query anyways. As crazy as it sounds, valid players can sometimes have invalid accounts.
	sqlite3_stmt* stmt;
	Player* player;
	int ret = sqlite3_prepare(db, "SELECT uid FROM players WHERE aid = ?1 limit 1;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_bind_int(stmt, 1, aid);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_step(stmt);
	switch (ret) {
	default: // error
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_DONE: // no results
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_ROW: // got a hit
		player = new Player();
		player->setUid(sqlite3_column_int(stmt, 0));
		player->setAccount(account);
		sqlite3_finalize(stmt);
		return player;
	}
}

Player* dbGate::getPlayerByUid(unsigned int uid) {
	sqlite3_stmt* stmt;
	Player* player;
	unsigned int aid;
	int ret = sqlite3_prepare(db, "SELECT aid FROM players WHERE uid = ?1 limit 1;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_bind_int(stmt, 1, uid);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_step(stmt);
	switch (ret) {
	default: // error
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_DONE: // no results
		sqlite3_finalize(stmt);
		return NULL;
	case SQLITE_ROW: // got a hit
		player = new Player();
		player->setUid(uid);
		aid = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
		player->setAccount(getAccountByAid(aid));
		return player;
	}
}

Player* dbGate::newPlayer() {
	// TODO Allow reserving uid values
	sqlite3_stmt* stmt;
	int ret = sqlite3_prepare(db, "INSERT INTO players DEFAULT VALUES;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return NULL;
	}
	ret = sqlite3_step(stmt);
	if (ret != SQLITE_DONE) {
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		return NULL;
	}
	sqlite3_finalize(stmt);
	Player* player = new Player();
	player->setUid(sqlite3_last_insert_rowid(db));
	return player;
}

int dbGate::savePlayer(const Player& player) {
	// TODO More data than just uid and aid
	const Account* account = player.getAccount();
	// TODO Set aid to 0 rather than return immediately once more data needs to be saved
	if (account == NULL) return -1;
	sqlite3_stmt* stmt;
	int ret = sqlite3_prepare(db, "UPDATE players SET aid = ?1 WHERE uid = ?2;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -3;
	}
	ret = sqlite3_bind_int(stmt, 1, player.getUid());
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -2;
	}
	ret = sqlite3_bind_int(stmt, 2, account->getAccountId());
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -2;
	}
	ret = sqlite3_step(stmt);
	if (ret != SQLITE_DONE) {
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		return -1;
	}
	sqlite3_finalize(stmt);
	return 0;
}

int dbGate::deletePlayer(const Player& player) {
	sqlite3_stmt* stmt;
	int ret = sqlite3_prepare(db, "DELETE FROM players WHERE uid = ?1;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -2;
	}
	ret = sqlite3_bind_int(stmt, 1, player.getUid());
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -2;
	}
	ret = sqlite3_step(stmt);
	if (ret != SQLITE_DONE) {
		fprintf(stderr, "Unable to execute sql - errcode %d\n", -ret);
		return -1;
	}
	sqlite3_finalize(stmt);
	return 0;
}

std::string dbGate::getLdbObject(const std::string& key) {
	std::string ret;
	leveldb::Status s = ldb->Get(leveldb::ReadOptions(), key, &ret);
	// If a key isn't found in the db, don't indicate an error unless another error occured.
	if (!(s.ok() || s.IsNotFound())) {
		fprintf(stderr, "Warning: Error getting leveldb key: %s\n", s.ToString().c_str());
	}
	return ret;
}
int dbGate::setLdbObject(const std::string& key, const std::string& val) {
	int ret = 0;
	leveldb::Status s = ldb->Put(leveldb::WriteOptions(), key, val);
	if (!s.ok()) {
		fprintf(stderr, "Warning: Error setting leveldb key: %s\n", s.ToString().c_str());
		ret = -1;
	}
	return ret;
}

int dbGate::delLdbObject(const std::string& key) {
	int ret = 0;
	leveldb::Status s = ldb->Delete(leveldb::WriteOptions(), key);
	if (!s.ok()) {
		fprintf(stderr, "Warning: Error deleting leveldb key: %s\n", s.ToString().c_str());
		ret = -1;
	}
	return ret;
}

Avatar* dbGate::getAvatarByGuid(unsigned long long guid) {
	proto::AvatarInfo* v = getAvatarPbByGuid(guid);
	if (v == NULL) return NULL;
	Avatar* ret = new Avatar(*v);
	delete v;
	return ret;
}

proto::AvatarInfo* dbGate::getAvatarPbByGuid(unsigned long long guid) {
	static char key_c[12];
	unsigned int* key_i = (unsigned int*) key_c;
	key_i[0] = INVENTORY;
	key_i[1] = guid >> 32;
	key_i[2] = guid & -1;
	std::string key(key_c, 12);
	std::string val = getLdbObject(key);
	if (val.empty()) return NULL;
	proto::AvatarInfo* ret = new proto::AvatarInfo();
	if (!ret->ParseFromString(val)) {
		delete ret;
		return NULL;
	}
	return ret;
}

int dbGate::saveAvatar(const Avatar& a) {
	return saveAvatar(a);
}

int dbGate::saveAvatar(const proto::AvatarInfo& a) {
	unsigned long long guid = a.guid();
	static char key_c[12];
	unsigned int* key_i = (unsigned int*) key_c;
	key_i[0] = INVENTORY;
	key_i[1] = guid >> 32;
	key_i[2] = guid & -1;
	std::string key(key_c, 12);
	std::string val;
	if (!a.SerializeToString(&val)) return -1;
	return setLdbObject(key, val);
}

int dbGate::deleteAvatar(const Avatar& a) {
	return deleteByGuid(a.getGuid());
}

int dbGate::deleteAvatar(const proto::AvatarInfo& a) {
	return deleteByGuid(a.guid());
}

Item* dbGate::getItemByGuid(unsigned long long guid) {
	proto::Item* v = getItemPbByGuid(guid);
	if (v == NULL) return NULL;
	Item* ret = new Item(*v);
	delete v;
	return ret;
}

proto::Item* dbGate::getItemPbByGuid(unsigned long long guid) {
	static char key_c[12];
	unsigned int* key_i = (unsigned int*) key_c;
	key_i[0] = INVENTORY;
	key_i[1] = guid >> 32;
	key_i[2] = guid & -1;
	std::string key(key_c, 12);
	std::string val = getLdbObject(key);
	if (val.empty()) return NULL;
	proto::Item* ret = new proto::Item();
	if (!ret->ParseFromString(val)) {
		delete ret;
		return NULL;
	}
	return ret;
}

int dbGate::saveItem(const Item& i) {
	return saveItem(i);
}

int dbGate::saveItem(const proto::Item& i) {
	unsigned long long guid = i.guid();
	static char key_c[12];
	unsigned int* key_i = (unsigned int*) key_c;
	key_i[0] = INVENTORY;
	key_i[1] = guid >> 32;
	key_i[2] = guid & -1;
	std::string key(key_c, 12);
	std::string val;
	if (!i.SerializeToString(&val)) return -1;
	return setLdbObject(key, val);
}

int dbGate::deleteItem(const Item& i) {
	return deleteByGuid(i.guid);
}

int dbGate::deleteItem(const proto::Item& i) {
	return deleteByGuid(i.guid());
}

int dbGate::deleteByGuid(unsigned long long guid) {
	static char key_c[12];
	unsigned int* key_i = (unsigned int*) key_c;
	key_i[0] = INVENTORY;
	key_i[1] = guid >> 32;
	key_i[2] = guid & -1;
	std::string key(key_c, 12);
	return delLdbObject(key);
}
