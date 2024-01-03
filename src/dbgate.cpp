/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <sqlite3.h>
#include <stdexcept>
#include "account.h"
#include "dbgate.h"

extern "C" {
	static int createTables(sqlite3*);
}

dbGate* globalDbGate;

dbGate::dbGate(const char* path) {
	int ret = sqlite3_open_v2(path, &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
	if (ret != SQLITE_OK) {
		char b[1024];
		snprintf(b, 1024, "SQLite connection couldn't be made, error %d", -ret);
		throw std::runtime_error(b);
	}
	if (createTables(db)) {
		throw std::runtime_error("Unable to create at least one database table.");
	}
}

dbGate::~dbGate() {
	sqlite3_close_v2(db);
}

extern "C" {
	static int createTables(sqlite3* db) {
		int ret = sqlite3_exec(db,
			// TODO create more tables
			"CREATE TABLE IF NOT EXISTS accounts (aid INTEGER PRIMARY KEY, timeCreated DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP, username TEXT UNIQUE, password TEXT, deviceId TEXT, email TEXT, token TEXT, tokenCreated DATETIME, sessionKey TEXT);",
		NULL, NULL, NULL);
		if (ret != SQLITE_OK) {
			fprintf(stderr, "Unable to create accounts table - errcode %d\n", -ret);
			return -1;
		}
		// CREATE TABLE bans (aid INTEGER, uid INTEGER, ip TEXT, reason TEXT, start DATETIME DEFAULT CURRENT_TIMESTAMP, end DATETIME);
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
	int ret = sqlite3_prepare(db, "SELECT username, password, email, token, sessionKey, deviceId, tokenCreated FROM accounts WHERE aid = ?1 limit 1;", -1, &stmt, NULL);
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
		deviceId = (const char*) sqlite3_column_text(stmt, 6);
		if (deviceId != NULL) account->setDeviceId(deviceId);
		account->setTokenTimestamp(sqlite3_column_int(stmt, 7));
		sqlite3_finalize(stmt);
		return account;
	}
}

Account* dbGate::getAccountByUid(unsigned int uid) {
	// TODO "select aid from players where uid = ?1 limit 1;" and then `return getAccountByAid(aid);`
	return NULL;
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
	int ret = sqlite3_prepare(db, "SELECT aid, password, email, token, sessionKey, deviceId, tokenCreated FROM accounts WHERE username = ?1 limit 1;", -1, &stmt, NULL);
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
		account->setAccountId(sqlite3_column_int(stmt, 1));
		password = (const char*) sqlite3_column_text(stmt, 2);
		if (password != NULL) account->setPasswordHash(password);
		email = (const char*) sqlite3_column_text(stmt, 3);
		if (email != NULL) account->setEmail(email);
		token = (const char*) sqlite3_column_text(stmt, 4);
		if (token != NULL) account->setToken(token);
		sessionKey = (const char*) sqlite3_column_text(stmt, 5);
		if (sessionKey != NULL) account->setSessionKey(sessionKey);
		deviceId = (const char*) sqlite3_column_text(stmt, 6);
		if (deviceId != NULL) account->setDeviceId(deviceId);
		account->setTokenTimestamp(sqlite3_column_int(stmt, 7));
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
	int ret = sqlite3_prepare(db, "SELECT aid, username, password, email, sessionKey, deviceId, tokenCreated FROM accounts WHERE token = ?1 limit 1;", -1, &stmt, NULL);
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
		account->setAccountId(sqlite3_column_int(stmt, 1));
		username = (const char*) sqlite3_column_text(stmt, 2);
		if (username != NULL) {
			account->setUsername(username);
			account->setIsGuest(0);
		}
		else {
			account->setIsGuest(1);
		}
		password = (const char*) sqlite3_column_text(stmt, 3);
		if (password != NULL) account->setPasswordHash(password);
		email = (const char*) sqlite3_column_text(stmt, 4);
		if (email != NULL) account->setEmail(email);
		sessionKey = (const char*) sqlite3_column_text(stmt, 5);
		if (sessionKey != NULL) account->setSessionKey(sessionKey);
		deviceId = (const char*) sqlite3_column_text(stmt, 6);
		if (deviceId != NULL) account->setDeviceId(deviceId);
		account->setTokenTimestamp(sqlite3_column_int(stmt, 7));
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
	int ret = sqlite3_prepare(db, "SELECT aid, username, password, email, token, deviceId, tokenCreated FROM accounts WHERE sessionKey = ?1 limit 1;", -1, &stmt, NULL);
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
		account->setAccountId(sqlite3_column_int(stmt, 1));
		username = (const char*) sqlite3_column_text(stmt, 2);
		if (username != NULL) {
			account->setUsername(username);
			account->setIsGuest(0);
		}
		else {
			account->setIsGuest(1);
		}
		password = (const char*) sqlite3_column_text(stmt, 3);
		if (password != NULL) account->setPasswordHash(password);
		email = (const char*) sqlite3_column_text(stmt, 4);
		if (email != NULL) account->setEmail(email);
		token = (const char*) sqlite3_column_text(stmt, 5);
		if (token != NULL) account->setToken(token);
		deviceId = (const char*) sqlite3_column_text(stmt, 6);
		if (deviceId != NULL) account->setDeviceId(deviceId);
		account->setTokenTimestamp(sqlite3_column_int(stmt, 7));
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
	int ret = sqlite3_prepare(db, "SELECT aid, username, password, email, token, sessionKey, tokenCreated FROM accounts WHERE sessionKey = ?1 limit 1;", -1, &stmt, NULL);
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
		account->setAccountId(sqlite3_column_int(stmt, 1));
		username = (const char*) sqlite3_column_text(stmt, 2);
		if (username != NULL) {
			account->setUsername(username);
			account->setIsGuest(0);
		}
		else {
			account->setIsGuest(1);
		}
		password = (const char*) sqlite3_column_text(stmt, 3);
		if (password != NULL) account->setPasswordHash(password);
		email = (const char*) sqlite3_column_text(stmt, 4);
		if (email != NULL) account->setEmail(email);
		token = (const char*) sqlite3_column_text(stmt, 5);
		if (token != NULL) account->setToken(token);
		sessionKey = (const char*) sqlite3_column_text(stmt, 6);
		if (sessionKey != NULL) account->setSessionKey(sessionKey);
		account->setTokenTimestamp(sqlite3_column_int(stmt, 7));
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
	return account;
}

int dbGate::saveAccount(const Account& account) {
	sqlite3_stmt* stmt;
	int ret = sqlite3_prepare(db, "UPDATE accounts SET username = ?1, password = ?2, email = ?3, token = ?4, sessionKey = ?5, deviceId = ?7, tokenCreated = ?8 WHERE aid = ?6;", -1, &stmt, NULL);
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
	ret = sqlite3_bind_int(stmt, 8, account.getTokenTimestamp());
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
