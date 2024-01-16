/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef SESSION_H
#define SESSION_H
class Session; // Resolves a circular dependency.

#include "gameserver.h"
#include "kcpsession.h"

class Session {
public:
	Session(Gameserver*, sock_t*, unsigned long long);
	~Session();
	void update();
	void close(unsigned int);
	KcpSession* getKcpSession();
	unsigned int getState();
	void setState(unsigned int);
	unsigned long long getSessionSeed() const;
	const unsigned char* getSessionKey() const;
	void generateSessionKey();
private:
	KcpSession* kcpSession;
	unsigned int state;
	unsigned long long sessionSeed;
	unsigned char sessionKey[4096];
};

extern "C" {
	int SessionMain(Session*);
}
#endif
