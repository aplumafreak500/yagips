/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef DBGATE_H
#define DBGATE_H
#include <sqlite3.h>
#include "account.h"
class dbGate {
public:
	dbGate(const char*);
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
private:
	sqlite3* db;
};
extern dbGate* globalDbGate;
#endif
