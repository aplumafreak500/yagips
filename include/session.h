/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2024 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef SESSION_H
#define SESSION_H
class Session; // Resolves two circular dependencies.

#include "gameserver.h"
#include "player.h"
#include "account.h"
#include "packet.h"
#include "kcpsession.h"

class Session {
public:
	Session(Gameserver*, sock_t*, unsigned long long);
	~Session();
	void update();
	void close(unsigned int);
	KcpSession* getKcpSession() const;
	unsigned int getState() const;
	void setState(unsigned int);
	Player* getPlayer() const;
	void setPlayer(Player*);
	const Account* getAccount() const;
	unsigned long long getSessionSeed() const;
	const unsigned char* getSessionKey() const;
	void generateSessionKey();
	unsigned int useSecretKey() const;
	void setUseSecretKey();
	void setUseSecretKey(unsigned int);
	void clearUseSecretKey();
	unsigned long long getLastPingTime() const;
	void setLastPingTime(unsigned long long);
	void updateLastPingTime();
	unsigned int getSeq() const;
	unsigned int nextSeq();
	void setSeq(unsigned int);
	int sendPacket(Packet&);
private:
	KcpSession* kcpSession;
	Player* player;
	unsigned int state;
	unsigned int use_secret_key;
	unsigned int sequence;
	unsigned long long sessionSeed;
	unsigned long long lastPingTime;
	unsigned char sessionKey[4096];
};

extern "C" {
	int SessionMain(Session*);
}
#endif
