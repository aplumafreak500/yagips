/* SPDX-License-Identifier: AGPL-3.0-or-later */
/* This file is part of yagips.

©2023 Alex Pensinger (ArcticLuma113)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#ifndef GAMESERVER_H
#define GAMESERVER_H
#include <sys/socket.h>
#include <arpa/inet.h>

// Session requires these two types.
class Gameserver;
extern "C" {
	typedef struct sockaddr_in6 sock_t;
}

#include "session.h"

extern "C" {
	typedef struct {
		const char* ip;
		unsigned short port;
	} gs_arg_t;

	typedef struct {
		unsigned int magic1;
		unsigned int sid1;
		unsigned int sid2;
		unsigned int cmd;
		unsigned int magic2;
	} kcp_handshake_t;

	extern volatile int GameserverSignal;

	Session* getSessionById(unsigned long long);
	Session* getSessionByUid(unsigned int);
	void* GameserverMain(void*);
}

class Gameserver {
public:
	Gameserver(const char* ip = "::", unsigned short port = 22102);
	~Gameserver();
	int start();
	void stop();
	ssize_t recv(unsigned char*, size_t);
	ssize_t recv(sock_t*, unsigned char*, size_t);
	ssize_t send(sock_t*, const unsigned char*, size_t);
	int canRecv(unsigned long);
	unsigned long getKcpInterval() const;
	void setKcpInterval(unsigned long);
	unsigned int getMaxSessions() const;
	void setMaxSessions(unsigned int);
	Session** getSessions() const;
	Session* getSession(unsigned int) const;
	Session* getSessionById(unsigned long long) const;
	Session* getSessionByUid(unsigned int) const;
private:
	const char* ip;
	unsigned short port;
	int fd;
	sock_t srv;
	struct in6_addr addr;
	unsigned long kcpInterval = 100;
	unsigned int maxSessions = 10;
	Session** sessionList;
};
#endif
