/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <leveldb/db.h>
#include <stdexcept>
#include "account.h"
#include "dbgate.h"
#include "util.h"
#include "define.pb.h"
#include "storage.pb.h"

dbGate* globalDbGate;

dbGate::dbGate(const char* path) {
	static char b[1024];
	// Don't compress since (soon:tm:) we apply Zlib compression to most input data.
	opt.compression = leveldb::kNoCompression;
	opt.create_if_missing = true;
	leveldb::Status s = leveldb::DB::Open(opt, path, &db);
	if (!s.ok()) {
		snprintf(b, 1024, "LevelDB object couldn't be created, error: %s", s.ToString().c_str());
		throw std::runtime_error(b);
	}
	load();
}

dbGate::~dbGate() {
	save();
	if (db != NULL) delete db;
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

Account* dbGate::getAccountByAid(unsigned int aid) {
	static char key_c[8];
	unsigned int* key_i = (unsigned int*) key_c;
	key_i[0] = ACCOUNT;
	key_i[1] = aid;
	std::string key(key_c, 8);
	std::string val = getLdbObject(key);
	if (val.empty()) return NULL;
	storage::AccountInfo* pval = new storage::AccountInfo();
	if (!pval->ParseFromString(val)) {
		delete pval;
		return NULL;
	}
	Account* account = new Account(*pval);
	delete pval;
	return account;
}

Account* dbGate::getAccountByUid(unsigned int uid) {
	Player* player = getPlayerByUid(uid);
	if (player == NULL) return NULL;
	return (Account*) player->getAccount();
}

Account* dbGate::getAccountByUsername(const char* username) {
	if (username == NULL) return NULL;
	Account* account = NULL;
	unsigned int key_type;
	storage::AccountInfo* pval = new storage::AccountInfo();
	std::string val;
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		memcpy(&key_type, it->key().ToString().c_str(), sizeof(unsigned int));
		if (key_type != ACCOUNT) continue;
		val = it->value().ToString();
		if (!pval->ParseFromString(val)) continue;
		if (pval->username() == username) {
			account = new Account(*pval);
			break;
		}
		pval->Clear();
	}
	if (!it->status().ok()) {
		fprintf(stderr, "Warning: Error searching leveldb keys: %s\n", it->status().ToString().c_str());
	}
	delete it;
	delete pval;
	return account;
}

Account* dbGate::getAccountByToken(const char* token) {
	if (token == NULL) return NULL;
	Account* account = NULL;
	unsigned int key_type;
	storage::AccountInfo* pval = new storage::AccountInfo();
	std::string val;
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		memcpy(&key_type, it->key().ToString().c_str(), sizeof(unsigned int));
		if (key_type != ACCOUNT) continue;
		val = it->value().ToString();
		if (!pval->ParseFromString(val)) continue;
		if (pval->token() == token) {
			account = new Account(*pval);
			break;
		}
		pval->Clear();
	}
	if (!it->status().ok()) {
		fprintf(stderr, "Warning: Error searching leveldb keys: %s\n", it->status().ToString().c_str());
	}
	delete it;
	delete pval;
	return account;
}

Account* dbGate::getAccountBySessionKey(const char* sessionKey) {
	if (sessionKey == NULL) return NULL;
	Account* account = NULL;
	unsigned int key_type;
	storage::AccountInfo* pval = new storage::AccountInfo();
	std::string val;
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		memcpy(&key_type, it->key().ToString().c_str(), sizeof(unsigned int));
		if (key_type != ACCOUNT) continue;
		val = it->value().ToString();
		if (!pval->ParseFromString(val)) continue;
		if (pval->session_key() == sessionKey) {
			account = new Account(*pval);
			break;
		}
		pval->Clear();
	}
	if (!it->status().ok()) {
		fprintf(stderr, "Warning: Error searching leveldb keys: %s\n", it->status().ToString().c_str());
	}
	delete it;
	delete pval;
	return account;
}

Account* dbGate::getAccountByDeviceId(const char* deviceId) {
	if (deviceId == NULL) return NULL;
	Account* account = NULL;
	unsigned int key_type;
	storage::AccountInfo* pval = new storage::AccountInfo();
	std::string val;
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		memcpy(&key_type, it->key().ToString().c_str(), sizeof(unsigned int));
		if (key_type != ACCOUNT) continue;
		val = it->value().ToString();
		if (!pval->ParseFromString(val)) continue;
		if (pval->device_id() == deviceId) {
			account = new Account(*pval);
			break;
		}
		pval->Clear();
	}
	if (!it->status().ok()) {
		fprintf(stderr, "Warning: Error searching leveldb keys: %s\n", it->status().ToString().c_str());
	}
	delete it;
	delete pval;
	return account;
}

// TODO Allow reserving uid values
Account* dbGate::createAccount(const char* username) {
	Account* account;
	if (username != NULL) {
		account = getAccountByUsername(username);
		if (account != NULL) {
			// TODO: extra "check if existing" parameter
			return account;
		}
	}
	unsigned int next_aid = next_ids.next_aid();
	if (next_aid == 0) {
		next_aid = 1;
	}
	static char key_c[8];
	unsigned int* key_i = (unsigned int*) key_c;
	std::string key;
	std::string val;
	key_i[0] = ACCOUNT;
	while(1) {
		key_i[1] = next_aid;
		key.assign(key_c, 8);
		val = getLdbObject(key);
		if (val.empty()) break;
		next_aid++;
	}
	next_ids.set_next_aid(next_aid);
	account = new Account();
	account->setAccountId(next_aid);
	if (username != NULL) {
		account->setUsername(username);
		account->setIsGuest(0);
	}
	else {
		account->setIsGuest(1);
	}
	account->getNewToken();
	account->getNewSessionKey();
	saveAccount(*account);
	save();
	return account;
}

int dbGate::saveAccount(const Account& account) {
	static char key_c[8];
	unsigned int* key_i = (unsigned int*) key_c;
	key_i[0] = ACCOUNT;
	key_i[1] = account.getAccountId();
	std::string key(key_c, 8);
	std::string val;
	storage::AccountInfo a = account;
	if (!a.SerializeToString(&val)) return -1;
	return setLdbObject(key, val);
}

int dbGate::deleteAccount(const Account& account) {
	// TODO Delete player objects owned by this aid
	static char key_c[8];
	unsigned int* key_i = (unsigned int*) key_c;
	key_i[0] = ACCOUNT;
	key_i[1] = account.getAccountId();
	std::string key(key_c, 8);
	return delLdbObject(key);
}

Player* dbGate::getPlayerByAccount(const Account& account) {
	return getPlayerByAid(account.getAccountId());
}

Player* dbGate::getPlayerByAid(unsigned int aid) {
	Player* player = NULL;
	unsigned int key_type;
	storage::PlayerInfo* pval = new storage::PlayerInfo();
	std::string val;
	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		memcpy(&key_type, it->key().ToString().c_str(), sizeof(unsigned int));
		if (key_type != PLAYER) continue;
		val = it->value().ToString();
		if (!pval->ParseFromString(val)) continue;
		if (pval->aid() == aid) {
			player = new Player(*pval);
			break;
		}
		pval->Clear();
	}
	if (!it->status().ok()) {
		fprintf(stderr, "Warning: Error searching leveldb keys: %s\n", it->status().ToString().c_str());
	}
	delete it;
	delete pval;
	return player;
}

Player* dbGate::getPlayerByUid(unsigned int uid) {
	static char key_c[8];
	unsigned int* key_i = (unsigned int*) key_c;
	key_i[0] = PLAYER;
	key_i[1] = uid;
	std::string key(key_c, 8);
	std::string val = getLdbObject(key);
	if (val.empty()) return NULL;
	storage::PlayerInfo* pval = new storage::PlayerInfo();
	if (!pval->ParseFromString(val)) {
		delete pval;
		return NULL;
	}
	Player* player = new Player(*pval);
	delete pval;
	return player;
}

Player* dbGate::newPlayer() {
	unsigned int next_uid = next_ids.next_uid();
	if (next_uid == 0) {
		next_uid = 1;
	}
	static char key_c[8];
	unsigned int* key_i = (unsigned int*) key_c;
	std::string key;
	std::string val;
	key_i[0] = PLAYER;
	// TODO Allow reserving uid values
	while(1) {
		key_i[1] = next_uid;
		key.assign(key_c, 8);
		val = getLdbObject(key);
		if (val.empty()) break;
		next_uid++;
	}
	next_ids.set_next_uid(next_uid);
	Player* player = new Player();
	player->setUid(next_uid);
	savePlayer(*player);
	save();
	return player;
}

int dbGate::savePlayer(const Player& player) {
	static char key_c[8];
	unsigned int* key_i = (unsigned int*) key_c;
	key_i[0] = PLAYER;
	key_i[1] = player.getUid();
	std::string key(key_c, 8);
	std::string val;
	storage::PlayerInfo p = player;
	if (!p.SerializeToString(&val)) return -1;
	return setLdbObject(key, val);
}

int dbGate::deletePlayer(const Player& player) {
	// TODO Delete objects owned by this uid
	static char key_c[8];
	unsigned int* key_i = (unsigned int*) key_c;
	key_i[0] = PLAYER;
	key_i[1] = player.getUid();
	std::string key(key_c, 8);
	return delLdbObject(key);
}

std::string dbGate::getLdbObject(const std::string& key) {
	std::string ret;
	leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &ret);
	// If a key isn't found in the db, don't indicate an error unless another error occured.
	if (!(s.ok() || s.IsNotFound())) {
		fprintf(stderr, "Warning: Error getting leveldb key: %s\n", s.ToString().c_str());
	}
	fprintf(stderr, "successful ldb key read\nHexdump of key:\n");
	DbgHexdump((unsigned char*) key.c_str(), key.size());
	fprintf(stderr, "Hexdump of data:\n");
	DbgHexdump((unsigned char*) ret.c_str(), ret.size());
	return ret;
}
int dbGate::setLdbObject(const std::string& key, const std::string& val) {
	int ret = 0;
	leveldb::Status s = db->Put(leveldb::WriteOptions(), key, val);
	if (!s.ok()) {
		fprintf(stderr, "Warning: Error setting leveldb key: %s\n", s.ToString().c_str());
		ret = -1;
	}
	fprintf(stderr, "successful ldb key write\nHexdump of key:\n");
	DbgHexdump((unsigned char*) key.c_str(), key.size());
	fprintf(stderr, "Hexdump of data:\n");
	DbgHexdump((unsigned char*) val.c_str(), val.size());
	return ret;
}

int dbGate::delLdbObject(const std::string& key) {
	int ret = 0;
	leveldb::Status s = db->Delete(leveldb::WriteOptions(), key);
	if (!s.ok()) {
		fprintf(stderr, "Warning: Error deleting leveldb key: %s\n", s.ToString().c_str());
		ret = -1;
	}
	fprintf(stderr, "successful ldb key drop\nHexdump of key:\n");
	DbgHexdump((unsigned char*) key.c_str(), key.size());
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
	proto::AvatarInfo ap = a;
	return saveAvatar(ap);
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
	proto::Item ip = i;
	return saveItem(ip);
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
