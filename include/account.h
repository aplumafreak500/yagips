/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <string>
class Account; // Resolves a circular dependency.
#include "session.h"
class Account {
public:
	Account();
	~Account();
	const std::string& getUsername() const;
	void setUsername(const std::string&);
	const std::string& getPasswordHash() const;
	void setPasswordHash(const std::string&);
	const std::string& getDeviceId() const;
	void setDeviceId(const std::string&);
	const std::string& getEmail() const;
	void setEmail(const std::string&);
	const std::string& getToken() const;
	void setToken(const std::string&);
	const std::string& getNewToken();
	const std::string& getSessionKey() const;
	void setSessionKey(const std::string&);
	const std::string& getNewSessionKey();
	unsigned int getAccountId() const;
	void setAccountId(unsigned int);
	unsigned int isGuest() const;
	void setIsGuest(unsigned int);
	long long getSessionKeyTimestamp() const;
	void setSessionKeyTimestamp();
	void setSessionKeyTimestamp(long long);
	const Session* getSession() const;
	void setSession(const Session*);
	int saveToDb() const;
private:
	std::string username;
	std::string password_hash;
	std::string deviceId;
	std::string email;
	std::string token;
	std::string sessionKey;
	unsigned int aid;
	unsigned int guest;
	long long sessionKeyTimestamp;
	const Session* session;
};
#endif
