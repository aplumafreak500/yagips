/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <time.h>
#include <sys/random.h>
#include "dbgate.h"
#include "account.h"
#include "session.h"
#include "util.h"
#include "proto/storage.pb.h"

Account::Account() {
	session = NULL;
}

Account::Account(const storage::AccountInfo& p) {
	aid = p.id();
	username = p.username();
	password_hash = p.password();
	email = p.email();
	deviceId = p.device_id();
	token = hexenc(p.token());
	sessionKey = b64enc(p.session_key());
	sessionKeyTimestamp = p.session_key_ts();
	guest = p.is_guest();
	// TODO Reserved uid
	session = NULL;
	for (int i = 0; i < p.permissions_size(); i++) {
		permissions.push_back(static_cast<Permission>(p.permissions(i)));
	}
}

Account::~Account() {}

Account::operator storage::AccountInfo() const {
	storage::AccountInfo ret;
	ret.set_id(aid);
	ret.set_username(username);
	ret.set_password(password_hash);
	ret.set_email(email);
	ret.set_device_id(deviceId);
	ret.set_token(hexdec(token));
	ret.set_session_key(b64dec(sessionKey));
	ret.set_session_key_ts(sessionKeyTimestamp);
	// TODO Reserved uid
	for (auto i = permissions.cbegin(); i != permissions.cend(); i++) {
		ret.add_permissions(static_cast<unsigned int>(*i));
	}
	return ret;
}

unsigned int Account::getAccountId() const {
	return aid;
}

void Account::setAccountId(unsigned int a) {
	aid = a;
}

const std::string& Account::getUsername() const {
	return username;
}

void Account::setUsername(const std::string& u) {
	username = u;
}

const std::string& Account::getDeviceId() const {
	return deviceId;
}

void Account::setDeviceId(const std::string& d) {
	deviceId = d;
}

const std::string& Account::getPasswordHash() const {
	return password_hash;
}

void Account::setPasswordHash(const std::string& p) {
	password_hash = p;
}

const std::string& Account::getEmail() const {
	return email;
}

void Account::setEmail(const std::string& e) {
	email = e;
}

const std::string& Account::getToken() const {
	return token;
}

void Account::setToken(const std::string& t) {
	token = t;
}

const std::string& Account::getNewToken() {
	char rawtoken[16]; // for a 32-character token. TODO pull from config
	getrandom(rawtoken, 16, 0);
	token = hexenc(std::string(rawtoken, 16));
	return token;
}

const std::string& Account::getSessionKey() const {
	return sessionKey;
}

void Account::setSessionKey(const std::string& k) {
	sessionKey = k;
}

const std::string& Account::getNewSessionKey() {
	char rawtoken[24]; // for a 32-character token. TODO pull from config
	getrandom(rawtoken, 24, 0);
	sessionKey = b64enc(std::string(rawtoken, 24));
	sessionKeyTimestamp = time(NULL);
	return sessionKey;
}

long long Account::getSessionKeyTimestamp() const {
	return sessionKeyTimestamp;
}

void Account::setSessionKeyTimestamp() {
	sessionKeyTimestamp = time(NULL);
}

void Account::setSessionKeyTimestamp(long long t) {
	sessionKeyTimestamp = t;
}

unsigned int Account::isGuest() const {
	return guest ? 1 : 0;
}

void Account::setIsGuest(unsigned int g) {
	guest = g ? 1 : 0;
}

int Account::saveToDb() const {
	return globalDbGate->saveAccount(*this);
}

const Session* Account::getSession() const {
	return session;
}

void Account::setSession(const Session* s) {
	session = s;
}

const std::list<Permission>& Account::getPermissions() const {
	return permissions;
}

void Account::setPermissions(const std::list<Permission>& l) {
	permissions = l;
}

void Account::clearPermissions() {
	permissions.clear();
}

void Account::addPermission(Permission p) {
	permissions.insert(permissions.end(), p);
}

void Account::removePermission(Permission p) {
	for (auto i = permissions.cbegin(); i != permissions.cend(); i++) {
		if (*i == p) {
			permissions.erase(i);
		}
	}
}

unsigned int Account::hasPermission(Permission p) const {
	for (auto i = permissions.cbegin(); i != permissions.cend(); i++) {
		if (*i == p) {
			return 1;
		}
	}
	return 0;
}
