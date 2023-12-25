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
			"create table if not exists accounts (aid integer primary key, username text unique, password text, email text, token text, sessionKey text);",
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
	int ret = sqlite3_prepare(db, "select username, password, email, token, sessionKey from accounts where aid = ?1 limit 1;", -1, &stmt, NULL);
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
		account->setUsername((const char*) sqlite3_column_text(stmt, 1));
		account->setPasswordHash((const char*) sqlite3_column_text(stmt, 2));
		account->setEmail((const char*) sqlite3_column_text(stmt, 3));
		account->setToken((const char*) sqlite3_column_text(stmt, 4));
		account->setSessionKey((const char*) sqlite3_column_text(stmt, 5));
		sqlite3_finalize(stmt);
		return account;
	}
}

Account* dbGate::getAccountByUid(unsigned int uid) {
	// TODO "select aid from players where uid = ?1 limit 1;"
	return NULL;
}

Account* dbGate::getAccountByUsername(const char* username) {
	sqlite3_stmt* stmt;
	Account* account;
	int ret = sqlite3_prepare(db, "select aid, password, email, token, sessionKey from accounts where username = ?1 limit 1;", -1, &stmt, NULL);
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
		account->setPasswordHash((const char*) sqlite3_column_text(stmt, 2));
		account->setEmail((const char*) sqlite3_column_text(stmt, 3));
		account->setToken((const char*) sqlite3_column_text(stmt, 4));
		account->setSessionKey((const char*) sqlite3_column_text(stmt, 5));
		sqlite3_finalize(stmt);
		return account;
	}
}

Account* dbGate::getAccountByToken(const char* token) {
	sqlite3_stmt* stmt;
	Account* account;
	int ret = sqlite3_prepare(db, "select aid, username, password, email, sessionKey from accounts where token = ?1 limit 1;", -1, &stmt, NULL);
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
		account->setUsername((const char*) sqlite3_column_text(stmt, 2));
		account->setPasswordHash((const char*) sqlite3_column_text(stmt, 3));
		account->setEmail((const char*) sqlite3_column_text(stmt, 4));
		account->setSessionKey((const char*) sqlite3_column_text(stmt, 5));
		sqlite3_finalize(stmt);
		return account;
	}
}

Account* dbGate::getAccountBySessionKey(const char* sessionKey) {
	sqlite3_stmt* stmt;
	Account* account;
	int ret = sqlite3_prepare(db, "select aid, username, password, email, token from accounts where sessionKey = ?1 limit 1;", -1, &stmt, NULL);
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
		account->setUsername((const char*) sqlite3_column_text(stmt, 2));
		account->setPasswordHash((const char*) sqlite3_column_text(stmt, 3));
		account->setEmail((const char*) sqlite3_column_text(stmt, 4));
		account->setToken((const char*) sqlite3_column_text(stmt, 5));
		sqlite3_finalize(stmt);
		return account;
	}
}

Account* dbGate::createAccount(const char* username) {
	sqlite3_stmt* stmt;
	Account* account = new Account();
	account->setUsername(username);
	int ret = sqlite3_prepare(db, "insert into accounts(username, token, sessionKey) values (?1, ?2, ?3);", -1, &stmt, NULL);
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
	ret = sqlite3_bind_text(stmt, 3, account->getSessionKey().c_str(), -1, SQLITE_STATIC);
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
	int ret = sqlite3_prepare(db, "update accounts set username = ?1, password = ?2, email = ?3, token = ?4, sessionKey = ?5 where aid = ?6;", -1, &stmt, NULL);
	if (ret != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare sql - errcode %d\n", -ret);
		return -3;
	}
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
	int ret = sqlite3_prepare(db, "delete from accounts where aid = ?1;", -1, &stmt, NULL);
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
