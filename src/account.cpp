/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <time.h>
#include <sys/random.h>
#include "dbgate.h"
#include "account.h"
#include "util.h"

Account::Account() {}
Account::~Account() {}

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
