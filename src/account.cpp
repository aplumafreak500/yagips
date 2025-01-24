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
	realName = p.real_name();
	idNumber = p.id_number();
	deviceId = p.device_id();
	authToken = b64enc(p.auth_token());
	authTokenTimestamp = p.auth_token_ts();
	comboToken = hexenc(p.combo_token());
	binderToken = b64enc(p.bind_token());
	binderTokenTimestamp = p.bind_token_ts();
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
	ret.set_real_name(realName);
	ret.set_id_number(idNumber);
	ret.set_device_id(deviceId);
	ret.set_auth_token(b64dec(authToken));
	ret.set_auth_token_ts(authTokenTimestamp);
	ret.set_combo_token(hexdec(comboToken));
	ret.set_bind_token(b64dec(binderToken));
	ret.set_bind_token_ts(binderTokenTimestamp);
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

const std::string& Account::getRealName() const {
	return realName;
}

void Account::setRealName(const std::string& r) {
	realName = r;
}

const std::string& Account::getIdNumber() const {
	return idNumber;
}

void Account::setIdNumber(const std::string& i) {
	idNumber = i;
}

const std::string& Account::getAuthToken() const {
	return authToken;
}

void Account::setAuthToken(const std::string& t) {
	authToken = t;
}

const std::string& Account::getNewAuthToken() {
	char rawtoken[24]; // for a 32-character token. TODO pull from config
	getrandom(rawtoken, 24, 0);
	authToken = b64enc(std::string(rawtoken, 24));
	authTokenTimestamp = time(NULL);
	return authToken;
}

const std::string& Account::getComboToken() const {
	return comboToken;
}

void Account::setComboToken(const std::string& t) {
	comboToken = t;
}

const std::string& Account::getNewComboToken() {
	char rawtoken[16]; // for a 32-character token. TODO pull from config
	getrandom(rawtoken, 16, 0);
	comboToken = hexenc(std::string(rawtoken, 16));
	return comboToken;
}

const std::string& Account::getBinderToken() const {
	return binderToken;
}

void Account::setBinderToken(const std::string& t) {
	binderToken = t;
}

const std::string& Account::getNewBinderToken() {
	char rawtoken[6]; // for an 8-character token. TODO pull from config
	getrandom(rawtoken, 6, 0);
	binderToken = b64enc(std::string(rawtoken, 6));
	binderTokenTimestamp = time(NULL);
	return binderToken;
}

long long Account::getAuthTokenTimestamp() const {
	return authTokenTimestamp;
}

void Account::setAuthTokenTimestamp() {
	authTokenTimestamp = time(NULL);
}

void Account::setAuthTokenTimestamp(long long t) {
	authTokenTimestamp = t;
}

long long Account::getBinderTokenTimestamp() const {
	return authTokenTimestamp;
}

void Account::setBinderTokenTimestamp() {
	binderTokenTimestamp = time(NULL);
}

void Account::setBinderTokenTimestamp(long long t) {
	binderTokenTimestamp = t;
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
