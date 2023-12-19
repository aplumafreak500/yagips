/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef ACCOUNT_H
#define ACCOUNT_H
class Account {
public:
	Account();
	~Account();
	const char* getUsername();
	void setUsername(const char*);
	const char* getPasswordHash();
	void setPasswordHash(const char*);
	const char* getEmail();
	void setEmail(const char*);
	const char* getToken();
	const char* getNewToken();
	const char* getSessionKey();
	const char* getNewSessionKey();
	unsigned int getUid();
	void setUid(unsigned int);
	unsigned int getAccountId(); // Read-only since this is entirely handled by the db
	void saveToDb();
private:
	const char* username;
	const char* password_hash;
	const char* email;
	const char* token;
	const char* sessionKey;
	unsigned int aid;
	unsigned int uid;
};
#endif
